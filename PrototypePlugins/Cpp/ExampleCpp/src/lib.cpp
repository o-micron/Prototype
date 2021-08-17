
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

// PluginLoadProtocol()               <--- Called only when the program first loads this plugin
//     PluginReloadProtocol()         <--- Called in case hot reloaded to handle linking existing pointers etc ..
//         PluginStartProtocol()      <--- Called one time after PluginReloadProtocol(), put your onStart code there
//             PluginUpdateProtocol() <--- Called every tick, regular update function called in the game loop before rendering ..
//         PluginEndProtocol()        <--- Called when plugin is dettached from an object or when the object gets destroyed
// PluginUnloadProtocol()             <--- Called once the engine exits or when you delete the plugin from the engine (unlinkely)

// Called when:
//     - The engine loads the plugin on startup
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginLoadProtocol(PrototypeEngineContext* engineContext, PrototypeLoggerData* loggerData)
{
    LoadContext(engineContext, loggerData);
    consoleLog(nullptr, __FILE__, __LINE__, "ExampleCpp is now loaded.");
    return true;
}

// Called when:
//     - The plugin is being loaded after calling PluginLoadProtocol()
//     - The engine loads the plugin on startup
//     - The engine detects a change in the plugin code (recompiled)
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginReloadProtocol(PrototypeEngineContext* engineContext, PrototypeLoggerData* loggerData)
{
    ReloadContext(engineContext, loggerData);
    consoleLog(nullptr, __FILE__, __LINE__, "ExampleCpp is now reloaded.");
    return true;
}

// Called when:
//     - The plugin is attached to an object's script trait
//     - The plugin is being recompiled after calling PluginReloadProtocol()
//
// Note:
//     - This is being called for each object which has attached a script instance of this plugin
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginStartProtocol(void* object)
{
    const char* objectName;
    ObjectGetName(object, &objectName);
    char formatBuffer[256];
    snprintf(formatBuffer, 256, "ExampleCpp is now attached to %s", objectName);
    consoleLog(object, __FILE__, __LINE__, formatBuffer);
    return true;
}

// Called when:
//     - The plugin is already attached to an object and before rendering the scene
//
// Note:
//     - This is being called for each object which has attached a script instance of this plugin
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginUpdateProtocol(void* object)
{
    return true;
}

// Called when:
//     - The plugin is dettached from an object's script trait
//     - The plugin is reloaded, before loading the newer compiled version of the plugin
//
// Note:
//     - This is being called for each object which has attached a script instance of this plugin
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginEndProtocol(void* object)
{
    const char* objectName;
    ObjectGetName(object, &objectName);
    char formatBuffer[256];
    snprintf(formatBuffer, 256, "ExampleCpp is now dettached from %s", objectName);
    consoleLog(object, __FILE__, __LINE__, formatBuffer);
    return true;
}

// Called when:
//     - The engine is shutting down
//     - You want to delete the plugin from your project
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API bool
PluginUnloadProtocol()
{
    consoleLog(nullptr, __FILE__, __LINE__, "ExampleCpp is now unloaded.");
    return true;
}

// Called when:
//     - The engine detected a mouse click
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnMouse(void* object, int32_t button, int32_t action, int32_t mods)
{}

// Called when:
//     - The engine detected a mouse cursor movement
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnMouseMove(void* object, double x, double y)
{}

// Called when:
//     - The engine detected a mouse drag action
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnMouseDrag(void* object, int32_t button, double x, double y)
{}

// Called when:
//     - The engine detected a mouse scroll action
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnMouseScroll(void* object, double x, double y)
{}

// Called when:
//     - The engine detected a keyboard key action
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnKeyboard(void* object, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{}

// Called when:
//     - The engine detected a window resize action
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowResize(void* object, int32_t width, int32_t height)
{}

// Called when:
//     - The engine detected some file(s) dropped on the window
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowDragDrop(void* object, int32_t numFiles, const char** names)
{}

// Called when:
//     - The engine detected the window is iconified
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowIconify(void* object)
{}

// Called when:
//     - The engine detected the window is back from being iconified
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowIconifyRestore(void* object)
{}

// Called when:
//     - The engine detected the window is maximized
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowMaximize(void* object)
{}

// Called when:
//     - The engine detected the window is back from being maximized
PROTOTYPE_PLUGIN_EXTERN PROTOTYPE_PLUGIN_API void
PluginOnWindowMaximizeRestore(void* object)
{}
