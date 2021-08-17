// The following code snippet is not part of the core game engine implementation
// This is a simple plugin example demonstrating how to communicate with the core engine
// The following snippet is expecting you to use version 0.0 of the Prototype Engine
// The Prototype Engine is work in progress, expect a better and more useful interface in the future

#pragma once

#if defined(__EMSCRIPTEN__)
#define PROTOTYPE_PLUGIN_PLATFORM_NAME "WEBKIT"
#define PROTOTYPE_PLUGIN_PLATFORM_WEBKIT
#define PROTOTYPE_PLUGIN_FORCE_INLINE inline
#define PROTOTYPE_PLUGIN_FORCE_NO_INLINE
#define PROTOTYPE_PLUGIN_DYNAMIC_FN_CALL
#elif defined(_WIN32) || defined(_WIN64)
#define PROTOTYPE_PLUGIN_PLATFORM_NAME "WINDOWS"
#define PROTOTYPE_PLUGIN_PLATFORM_WINDOWS
#define PROTOTYPE_PLUGIN_FORCE_INLINE    __forceinline
#define PROTOTYPE_PLUGIN_FORCE_NO_INLINE _declspec(noinline)
#define PROTOTYPE_PLUGIN_DYNAMIC_FN_CALL __stdcall
#elif defined(__linux__)
#define PROTOTYPE_PLUGIN_PLATFORM_NAME "LINUX"
#define PROTOTYPE_PLUGIN_PLATFORM_LINUX
#define PROTOTYPE_PLUGIN_FORCE_INLINE inline
#define PROTOTYPE_PLUGIN_FORCE_NO_INLINE
#define PROTOTYPE_PLUGIN_DYNAMIC_FN_CALL
#elif defined(__APPLE__)
#define PROTOTYPE_PLUGIN_PLATFORM_NAME "DARWIN"
#define PROTOTYPE_PLUGIN_PLATFORM_DARWIN
#define PROTOTYPE_PLUGIN_FORCE_INLINE inline
#define PROTOTYPE_PLUGIN_FORCE_NO_INLINE
#define PROTOTYPE_PLUGIN_DYNAMIC_FN_CALL
#else
#error "Platform not supported."
#endif

#if defined(__cplusplus)
#define PROTOTYPE_PLUGIN_EXTERN extern "C"
#else
#define PROTOTYPE_PLUGIN_EXTERN extern
#endif

#if defined(PROTOTYPE_PLUGIN_PLATFORM_WEBKIT) && defined(PROTOTYPE_PLUGIN_EXPORT_DYNAMIC)
#define PROTOTYPE_PLUGIN_API __attribute__((visibility("default")))
#elif defined(PROTOTYPE_PLUGIN_PLATFORM_WINDOWS) && defined(PROTOTYPE_PLUGIN_EXPORT_DYNAMIC)
#define PROTOTYPE_PLUGIN_API __declspec(dllexport)
#elif defined(PROTOTYPE_PLUGIN_PLATFORM_WINDOWS) && defined(PROTOTYPE_PLUGIN_IMPORT_DYNAMIC)
#define PROTOTYPE_PLUGIN_API __declspec(dllimport)
#elif defined(PROTOTYPE_PLUGIN_PLATFORM_DARWIN) && defined(PROTOTYPE_PLUGIN_EXPORT_DYNAMIC)
#define PROTOTYPE_PLUGIN_API __attribute__((visibility("default")))
#elif defined(PROTOTYPE_PLUGIN_PLATFORM_LINUX) && defined(PROTOTYPE_PLUGIN_EXPORT_DYNAMIC)
#define PROTOTYPE_PLUGIN_API __attribute__((visibility("default")))
#else
#define PROTOTYPE_PLUGIN_API
#endif

#include <PrototypeInterface/PrototypeInterface.h>

#include <stdio.h>
#include <unordered_map>

// Create a static shared data
// Sometimes We might need to have a single variable for each object that attaches the plugin
// thus, we need to have like a hashmap that holds a separate state for each object on its own
// we must reduce the usage of such static variables as much as possible
struct StaticSharedData
{
    // save whether each attached object has hit something or not
    static std::unordered_map<int64_t, bool> hasHit;
};

std::unordered_map<int64_t, bool> StaticSharedData::hasHit;

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginLoadProtocol(PrototypeEngineContext* engineContext, PrototypeLoggerData* loggerData)
{
    // we must call LoadContext in order to sync with the engine's states
    LoadContext(engineContext, loggerData);
    return true;
}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginReloadProtocol(PrototypeEngineContext* engineContext, PrototypeLoggerData* loggerData)
{
    // we must call ReloadContext in order to sync with the engine's states again
    ReloadContext(engineContext, loggerData);
    return true;
}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginStartProtocol(void* object)
{
    int64_t id = -1;
    ObjectGetId(object, &id);
    if (id != -1) { StaticSharedData::hasHit[id] = false; }
    return true;
}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginUpdateProtocol(void* object)
{
    // static variable to save the state whether we've ever hit and object or not
    int64_t id = -1;
    ObjectGetId(object, &id);
    bool hasHit = StaticSharedData::hasHit[id];

    //  In case we haven't hit anything
    if (!hasHit) {
        // the length of the ray we're going to cast
        const float rayLength = 10.0f;

        // we need padding because otherwise, the ray is going to hit the object that the ray is being casted from
        //
        //              +-----+                                 +---+
        //              |  x  |    |--->----->------>-------->  |   |
        //              +-----+    ^                            +---+
        //                 ^       |
        //                 |       |
        //        padding  +-------+
        //
        const float rayPadding = 3.0f;

        // get the current translation vector of the object
        FieldVec3 translation;
        TransformTraitGetTranslation(object, translation);

        // get the current forward vector of the object (normalized)
        FieldVec3 forward;
        TransformTraitGetForward(object, forward);

        // create a new vector representing the origin of the ray
        // this is basically the translation vector padded in the same direction for the forward vector
        FieldVec3 origin = {};
        origin.x         = (translation.x + forward.x * rayPadding);
        origin.y         = (translation.y + forward.y * rayPadding);
        origin.z         = (translation.z + forward.z * rayPadding);

        // we can log the vectors in the console tab in case we need to make sure we have the expected values
        char log[64];
        snprintf(log, 64, "translation [%f, %f, %f]", translation.x, translation.y, translation.z);
        consoleLog(object, __FILE__, __LINE__, log);
        snprintf(log, 64, "forward [%f, %f, %f]", forward.x, forward.y, forward.z);
        consoleLog(object, __FILE__, __LINE__, log);
        snprintf(log, 64, "origin [%f, %f, %f]", origin.x, origin.y, origin.z);
        consoleLog(object, __FILE__, __LINE__, log);

        // cast a ray and get the object that the ray is going to hit if any, we might hit nothing ..
        void* hitObject;
        PhysicsRaycastFromLocationAndDirection(&hitObject, origin, forward, rayLength);

        // check if we hit an object
        if (hitObject != nullptr) {
            // we hit some object
            // get the name of the current attached object
            const char* objectName;
            ObjectGetName(object, &objectName);

            // get the name of the object that we just hit with the ray
            const char* hitObjectName;
            ObjectGetName(hitObject, &hitObjectName);

            // print a message to console explaining which object hit which object
            char buffer[256];
            snprintf(buffer, 256, "Object <%s> did hit <%s>", objectName, hitObjectName);
            consoleLog(object, __FILE__, __LINE__, buffer);

            // set hasHit to true so we stop, we won't check any further
            StaticSharedData::hasHit[id] = true;
        } else {
            // we hit nothing
            // now let's move the object forward a little bit and check again ..
            translation.x += DeltaTime() * forward.x;
            translation.y += DeltaTime() * forward.y;
            translation.z += DeltaTime() * forward.z;
            TransformTraitSetTranslation(object, translation);
        }
    }

    return true;
}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginEndProtocol(void* object)
{
    return true;
}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginUnloadProtocol()
{
    return true;
}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnMouse(void* object, int32_t button, int32_t action, int32_t mods)
{}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnMouseMove(void* object, double x, double y)
{}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnMouseDrag(void* object, int32_t button, double x, double y)
{}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnMouseScroll(void* object, double x, double y)
{}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnKeyboard(void* object, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowResize(void* object, int32_t width, int32_t height)
{}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowDragDrop(void* object, int32_t numFiles, const char** names)
{}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowIconify(void* object)
{}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowIconifyRestore(void* object)
{}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowMaximize(void* object)
{}

PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowMaximizeRestore(void* object)
{}