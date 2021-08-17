R"~~#%#~~(
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

extern crate libc;

use ::std::os::raw::c_char;
use lazy_static::lazy_static;
use std::sync::{Arc, Mutex, RwLock};
use std::{
    borrow::BorrowMut,
    ffi::{CStr, CString},
};
use std::{
    collections::HashMap,
    fmt::format,
    ptr::{null, NonNull},
};

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

struct StaticSharedData {
    filename: Vec<c_char>,
    links: HashMap<&'static str, bool>,
    hasHit: bool,
}

// PluginLoadProtocol()               <--- Called only when the program first loads this plugin
//     PluginReloadProtocol()         <--- Called in case hot reloaded to handle linking existing pointers etc ..
//         PluginStartProtocol()      <--- Called one time after PluginReloadProtocol(), put your onStart code there
//             PluginUpdateProtocol() <--- Called every tick, regular update function called in the game loop before rendering ..
//         PluginEndProtocol()        <--- Called when plugin is dettached from an object or when the object gets destroyed
// PluginUnloadProtocol()             <--- Called once the engine exits or when you delete the plugin from the engine (unlinkely)

// Called when:
//     - The engine loads the plugin on startup
#[no_mangle]
extern "C" fn PluginLoadProtocol(
    engineContext: *mut PrototypeEngineContext,
    loggerData: *mut PrototypeLoggerData,
) -> bool {
    unsafe {
        LoadContext(engineContext, loggerData);
        consoleLog(
            std::ptr::null::<::std::os::raw::c_void>() as *mut ::std::os::raw::c_void,
            concat!(std::file!(), '\0').as_ptr() as *const ::std::os::raw::c_char,
            std::line!() as i32,
            "{{ PrototypePluginProjectName }} is now loaded\0".as_ptr() as *const ::std::os::raw::c_char,
        );
    }
    true
}

// Called when:
//     - The plugin is being loaded after calling PluginLoadProtocol()
//     - The engine loads the plugin on startup
//     - The engine detects a change in the plugin code (recompiled)
#[no_mangle]
extern "C" fn PluginReloadProtocol(
    engineContext: *mut PrototypeEngineContext,
    loggerData: *mut PrototypeLoggerData,
) -> bool {
    unsafe {
        ReloadContext(engineContext, loggerData);
        consoleLog(
            std::ptr::null::<::std::os::raw::c_void>() as *mut ::std::os::raw::c_void,
            concat!(std::file!(), '\0').as_ptr() as *const ::std::os::raw::c_char,
            std::line!() as i32,
            "{{ PrototypePluginProjectName }} is now reloaded\0".as_ptr() as *const ::std::os::raw::c_char,
        );
    }
    true
}

// Called when:
//     - The plugin is attached to an object's script trait
//     - The plugin is being recompiled after calling PluginReloadProtocol()
//
// Note:
//     - This is being called for each object which has attached a script instance of this plugin
#[no_mangle]
extern "C" fn PluginStartProtocol(object: *mut ::std::os::raw::c_void) -> bool {
    unsafe {
        let mut objectName: *const c_char = std::mem::MaybeUninit::uninit().assume_init();
        ObjectGetName(object, &mut objectName as *mut *const c_char);
        let objectNameStringCStrSlice = CStr::from_ptr(objectName).to_str().unwrap().to_string();
        let message = format!("{{ PrototypePluginProjectName }} is now attached to {}\0", objectNameStringCStrSlice);
        consoleLog(
            object,
            concat!(std::file!(), '\0').as_ptr() as *const ::std::os::raw::c_char,
            std::line!() as i32,
            message.as_ptr() as *const ::std::os::raw::c_char,
        );
    }
    true
}

// Called when:
//     - The plugin is already attached to an object and before rendering the scene
//
// Note:
//     - This is being called for each object which has attached a script instance of this plugin
#[no_mangle]
extern "C" fn PluginUpdateProtocol(object: *mut ::std::os::raw::c_void) -> bool {
    true
}

// Called when:
//     - The plugin is dettached from an object's script trait
//     - The plugin is reloaded, before loading the newer compiled version of the plugin
//
// Note:
//     - This is being called for each object which has attached a script instance of this plugin
#[no_mangle]
extern "C" fn PluginEndProtocol(object: *mut ::std::os::raw::c_void) -> bool {
    unsafe {
        let mut objectName: *const c_char = std::mem::MaybeUninit::uninit().assume_init();
        ObjectGetName(object, &mut objectName as *mut *const c_char);
        let objectNameStringCStrSlice = CStr::from_ptr(objectName).to_str().unwrap().to_string();
        let message = format!("{{ PrototypePluginProjectName }} is now dettached from {}\0", objectNameStringCStrSlice);
        consoleLog(
            object,
            concat!(std::file!(), '\0').as_ptr() as *const ::std::os::raw::c_char,
            std::line!() as i32,
            message.as_ptr() as *const ::std::os::raw::c_char,
        );
    }
    true
}

// Called when:
//     - The engine is shutting down
//     - You want to delete the plugin from your project
#[no_mangle]
extern "C" fn PluginUnloadProtocol() -> bool {
    unsafe {
        consoleLog(
            std::ptr::null::<::std::os::raw::c_void>() as *mut ::std::os::raw::c_void,
            concat!(std::file!(), '\0').as_ptr() as *const ::std::os::raw::c_char,
            std::line!() as i32,
            "{{ PrototypePluginProjectName }} is now unloaded\0".as_ptr() as *const ::std::os::raw::c_char,
        );
    }
    true
}

// Called when:
//     - The engine detected a mouse click
#[no_mangle]
extern "C" fn PluginOnMouse(
    object: *mut ::std::os::raw::c_void,
    button: i32,
    action: i32,
    mods: i32,
) {
}

// Called when:
//     - The engine detected a mouse cursor movement
#[no_mangle]
extern "C" fn PluginOnMouseMove(object: *mut ::std::os::raw::c_void, x: f64, y: f64) {}

// Called when:
//     - The engine detected a mouse drag action
#[no_mangle]
extern "C" fn PluginOnMouseDrag(object: *mut ::std::os::raw::c_void, button: i32, x: f64, y: f64) {}

// Called when:
//     - The engine detected a mouse scroll action
#[no_mangle]
extern "C" fn PluginOnMouseScroll(object: *mut ::std::os::raw::c_void, x: f64, y: f64) {}

// Called when:
//     - The engine detected a keyboard key action
#[no_mangle]
extern "C" fn PluginOnKeyboard(
    object: *mut ::std::os::raw::c_void,
    key: i32,
    scancode: i32,
    action: i32,
    mods: i32,
) {
}

// Called when:
//     - The engine detected a window resize action
#[no_mangle]
extern "C" fn PluginOnWindowResize(object: *mut ::std::os::raw::c_void, width: i32, height: i32) {}

// Called when:
//     - The engine detected some file(s) dropped on the window
#[no_mangle]
extern "C" fn PluginOnWindowDragDrop(
    object: *mut ::std::os::raw::c_void,
    numFiles: i32,
    names: *mut *const ::std::os::raw::c_char,
) {
}

// Called when:
//     - The engine detected the window is iconified
#[no_mangle]
extern "C" fn PluginOnWindowIconify(object: *mut ::std::os::raw::c_void) {}

// Called when:
//     - The engine detected the window is back from being iconified
#[no_mangle]
extern "C" fn PluginOnWindowIconifyRestore(object: *mut ::std::os::raw::c_void) {}

// Called when:
//     - The engine detected the window is maximized
#[no_mangle]
extern "C" fn PluginOnWindowMaximize(object: *mut ::std::os::raw::c_void) {}

// Called when:
//     - The engine detected the window is back from being maximized
#[no_mangle]
extern "C" fn PluginOnWindowMaximizeRestore(object: *mut ::std::os::raw::c_void) {}
)~~#%#~~"