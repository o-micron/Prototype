// The following code snippet is not part of the core game engine implementation
// This is a simple plugin example demonstrating how to communicate with the core engine
// The following snippet is expecting you to use version 0.0 of the Prototype Engine
// The Prototype Engine is work in progress, expect a better and more useful interface in the future

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

extern crate libc;

use ::std::os::raw::c_char;
use lazy_static::lazy_static;
use std::collections::HashMap;
use std::ffi::CStr;
use std::sync::{Arc, RwLock};

// include the prototype bindings code
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

// Create a static shared data
// Sometimes We might need to have a single variable for each object that attaches the plugin
// thus, we need to have like a hashmap that holds a separate state for each object on its own
// we must reduce the usage of such static variables as much as possible
struct StaticSharedData {
    // save whether each attached object has hit something or not
    hasHit: HashMap<i64, bool>,
}

lazy_static! {
    static ref SharedData: Arc<RwLock<StaticSharedData>> =
        Arc::new(RwLock::new(StaticSharedData {
            hasHit: HashMap::new()
        }));
}

#[no_mangle]
extern "C" fn PluginLoadProtocol(
    engineContext: *mut PrototypeEngineContext,
    loggerData: *mut PrototypeLoggerData,
) -> bool {
    // we must call LoadContext in order to sync with the engine's states
    // this is unsafe since we call C code here and Rust cannot ensure safety in that case
    unsafe {
        LoadContext(engineContext, loggerData);
    }
    true
}

#[no_mangle]
extern "C" fn PluginReloadProtocol(
    engineContext: *mut PrototypeEngineContext,
    loggerData: *mut PrototypeLoggerData,
) -> bool {
    // we must call ReloadContext in order to sync with the engine's states again
    // this is unsafe since we call C code here and Rust cannot ensure safety in that case
    unsafe {
        ReloadContext(engineContext, loggerData);
    }
    true
}

#[no_mangle]
extern "C" fn PluginStartProtocol(object: *mut ::std::os::raw::c_void) -> bool {
    let mut id: i64 = -1;
    unsafe {
        ObjectGetId(object, &mut id);
    }
    if id != -1 {
        SharedData
            .write()
            .unwrap()
            .hasHit
            .entry(id)
            .or_insert(false);
    }
    true
}

#[no_mangle]
extern "C" fn PluginUpdateProtocol(object: *mut ::std::os::raw::c_void) -> bool {
    unsafe {
        let mut id: i64 = -1;
        ObjectGetId(object, &mut id);
        let hasHit = *SharedData
            .read()
            .unwrap()
            .hasHit
            .get_key_value(&id)
            .unwrap()
            .1;
        if !hasHit {
            // the length of the ray we're going to cast
            let rayLength: f32 = 10.0;

            // we need padding because otherwise, the ray is going to hit the object that the ray is being casted from
            //
            //              +-----+                                 +---+
            //              |  x  |    |--->----->------>-------->  |   |
            //              +-----+    ^                            +---+
            //                 ^       |
            //                 |       |
            //        padding  +-------+
            //
            let rayPadding: f32 = 3.0;

            // get the current translation vector of the object
            let mut translation: FieldVec3 = FieldVec3 {
                x: 0.0,
                y: 0.0,
                z: 0.0,
            };
            TransformTraitGetTranslation(object, &mut translation);

            // get the current forward vector of the object (normalized)
            let mut forward: FieldVec3 = FieldVec3 {
                x: 0.0,
                y: 0.0,
                z: 0.0,
            };
            TransformTraitGetForward(object, &mut forward);

            // create a new vector representing the origin of the ray
            // this is basically the translation vector padded in the same direction for the forward vector
            let origin = FieldVec3 {
                x: -(translation.x + forward.x * rayPadding),
                y: -(translation.y + forward.y * rayPadding),
                z: -(translation.z + forward.z * rayPadding),
            };

            // cast a ray and get the object that the ray is going to hit if any, we might hit nothing ..
            let mut hitObject: *mut ::std::os::raw::c_void = std::ptr::null_mut();
            PhysicsRaycastFromLocationAndDirection(&mut hitObject, &origin, &forward, rayLength);

            // check if we hit an object
            if !hitObject.is_null() {
                // we hit some object
                // get the name of the current attached object
                let mut objectName: *const c_char = std::mem::MaybeUninit::uninit().assume_init();
                ObjectGetName(object, &mut objectName as *mut *const c_char);

                // get the name of the object that we just hit with the ray
                let mut hitObjectName: *const c_char =
                    std::mem::MaybeUninit::uninit().assume_init();
                ObjectGetName(hitObject, &mut hitObjectName as *mut *const c_char);

                // print a message to console explaining which object hit which object
                // Note:
                // ------
                // Rust Strings don't end with null terminator
                // That's why we need to do jump around and try to do some conversions
                // Notice that we are dealing with the core engine which is mostly C/C++
                // So we need to take care of that, in case you see strange string errors
                // or in case the plugin crashes the engine, remember this could be the case.
                let objectNameStringCStrSlice = CStr::from_ptr(objectName);
                let objectNameStringCStrSlice =
                    objectNameStringCStrSlice.to_str().unwrap().to_string();
                let hitObjectNameStringCStrSlice = CStr::from_ptr(hitObjectName);
                let hitObjectNameStringCStrSlice =
                    hitObjectNameStringCStrSlice.to_str().unwrap().to_string();
                // See here we need to add null terminator '\0' in the end
                // Rust doesn't do that by default here ..
                let message = format!(
                    "Object {} did hit {}\0",
                    objectNameStringCStrSlice, hitObjectNameStringCStrSlice
                );
                consoleLog(
                    object,
                    concat!(std::file!(), '\0').as_ptr() as *const ::std::os::raw::c_char,
                    std::line!() as i32,
                    message.as_ptr() as *const ::std::os::raw::c_char,
                );

                // set hasHit to true so we stop, we won't check any further
                *SharedData.write().unwrap().hasHit.get_mut(&id).unwrap() = true;
            } else {
                // we hit nothing
                // now let's move the object forward a little bit and check again ..
                translation.x += DeltaTime() as f32 * forward.x;
                translation.y += DeltaTime() as f32 * forward.y;
                translation.z += DeltaTime() as f32 * forward.z;
                TransformTraitSetTranslation(object, &translation);
            }
        }
    }
    true
}

#[no_mangle]
extern "C" fn PluginEndProtocol(object: *mut ::std::os::raw::c_void) -> bool {
    true
}

#[no_mangle]
extern "C" fn PluginUnloadProtocol() -> bool {
    true
}

#[no_mangle]
extern "C" fn PluginOnMouse(
    object: *mut ::std::os::raw::c_void,
    button: i32,
    action: i32,
    mods: i32,
) {
}

#[no_mangle]
extern "C" fn PluginOnMouseMove(object: *mut ::std::os::raw::c_void, x: f64, y: f64) {}

#[no_mangle]
extern "C" fn PluginOnMouseDrag(object: *mut ::std::os::raw::c_void, button: i32, x: f64, y: f64) {}

#[no_mangle]
extern "C" fn PluginOnMouseScroll(object: *mut ::std::os::raw::c_void, x: f64, y: f64) {}

#[no_mangle]
extern "C" fn PluginOnKeyboard(
    object: *mut ::std::os::raw::c_void,
    key: i32,
    scancode: i32,
    action: i32,
    mods: i32,
) {
}

#[no_mangle]
extern "C" fn PluginOnWindowResize(object: *mut ::std::os::raw::c_void, width: i32, height: i32) {}

#[no_mangle]
extern "C" fn PluginOnWindowDragDrop(
    object: *mut ::std::os::raw::c_void,
    numFiles: i32,
    names: *mut *const ::std::os::raw::c_char,
) {
}

#[no_mangle]
extern "C" fn PluginOnWindowIconify(object: *mut ::std::os::raw::c_void) {}

#[no_mangle]
extern "C" fn PluginOnWindowIconifyRestore(object: *mut ::std::os::raw::c_void) {}

#[no_mangle]
extern "C" fn PluginOnWindowMaximize(object: *mut ::std::os::raw::c_void) {}

#[no_mangle]
extern "C" fn PluginOnWindowMaximizeRestore(object: *mut ::std::os::raw::c_void) {}
