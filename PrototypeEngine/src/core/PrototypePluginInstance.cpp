/// Copyright 2021 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#include "PrototypePluginInstance.h"

#include "PrototypeEngine.h"
#include "PrototypeRenderer.h"
#include "PrototypeScene.h"
#include "PrototypeSceneNode.h"
#include "PrototypeUI.h"

#include <PrototypeCommon/IO.h>
#include <PrototypeCommon/Logger.h>
#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include <exception>
#include <filesystem>

#define TRY   try
#define CATCH catch (...)

static bool
defaultPluginLoadProtocol(PrototypeEngineContext*, PrototypeLoggerData*)
{
    return true;
}

static bool
defaultPluginReloadProtocol(PrototypeEngineContext*, PrototypeLoggerData*)
{
    return true;
}

static bool
defaultPluginStartProtocol(PrototypeObject*)
{
    return true;
}

static bool
defaultPluginUpdateProtocol(PrototypeObject*)
{
    return true;
}

static bool
defaultPluginEndProtocol(PrototypeObject*)
{
    return true;
}

static bool
defaultPluginUnloadProtocol()
{
    return true;
}

static void
defaultPluginOnMouse(PrototypeObject*, i32 button, i32 action, i32 mods)
{}

static void
defaultPluginOnMouseMove(PrototypeObject*, f64 x, f64 y)
{}

static void
defaultPluginOnMouseDrag(PrototypeObject*, i32 button, f64 x, f64 y)
{}

static void
defaultPluginOnMouseScroll(PrototypeObject*, f64 x, f64 y)
{}

static void
defaultPluginOnKeyboard(PrototypeObject*, i32 key, i32 scancode, i32 action, i32 mods)
{}

static void
defaultPluginOnWindowResize(PrototypeObject*, i32 width, i32 height)
{}

static void
defaultPluginOnWindowDragDrop(PrototypeObject*, i32 numFiles, const char** names)
{}

static void
defaultPluginOnWindowIconify(PrototypeObject*)
{}

static void
defaultPluginOnWindowIconifyRestore(PrototypeObject*)
{}

static void
defaultPluginOnWindowMaximize(PrototypeObject*)
{}

static void
defaultPluginOnWindowMaximizeRestore(PrototypeObject*)
{}

PrototypePluginInstance::PrototypePluginInstance(const std::string& filepath)
  : _filepath(filepath)
  , _LoadProtocol(nullptr)
  , _ReloadProtocol(nullptr)
  , _StartProtocol(nullptr)
  , _UpdateProtocol(nullptr)
  , _EndProtocol(nullptr)
  , _UnloadProtocol(nullptr)
  , _needsUpload(false)
  , _timestamp(0)
{
    std::filesystem::path p(_filepath);
    _name = p.filename().string();
}

PrototypePluginInstance::~PrototypePluginInstance() {}

void
PrototypePluginInstance::stageChange()
{
    _needsUpload = true;
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    PrototypeEngineInternalApplication::renderer->ui()->signalBuffersChanged(true);
#endif
}

void
PrototypePluginInstance::commitChange()
{
    if (_needsUpload) {
        _needsUpload = false;
        reload();
    }
}

bool
PrototypePluginInstance::load()
{
    _timestamp = PrototypeIo::filestamp(_filepath);
    PrototypeIo::copyFile(_filepath.c_str(), _name.c_str());
    _handle = PROTOTYPE_DLL_OPEN(_name.c_str(), RTLD_LAZY);
    if (!_handle) {
        PrototypeLogger::error("%s: Failed to load", _name.c_str());
        return false;
    }
    _LoadProtocol = (PluginLoadProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginLoadProtocol");
    if (_LoadProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginLoadProtocol", _name.c_str());
        _LoadProtocol = defaultPluginLoadProtocol;
    }
    _ReloadProtocol = (PluginReloadProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginReloadProtocol");
    if (_ReloadProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginReloadProtocol", _name.c_str());
        _ReloadProtocol = defaultPluginReloadProtocol;
    }
    _StartProtocol = (PluginStartProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginStartProtocol");
    if (_StartProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginStartProtocol", _name.c_str());
        _StartProtocol = defaultPluginStartProtocol;
    }
    _UpdateProtocol = (PluginUpdateProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginUpdateProtocol");
    if (_UpdateProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginUpdateProtocol", _name.c_str());
        _UpdateProtocol = defaultPluginUpdateProtocol;
    }
    _EndProtocol = (PluginEndProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginEndProtocol");
    if (_EndProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginEndProtocol", _name.c_str());
        _EndProtocol = defaultPluginEndProtocol;
    }
    _UnloadProtocol = (PluginUnloadProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginUnloadProtocol");
    if (_UnloadProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginUnloadProtocol", _name.c_str());
        _UnloadProtocol = defaultPluginUnloadProtocol;
    }
    _OnMouse = (PluginOnMouseFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnMouse");
    if (_OnMouse == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnMouse", _name.c_str());
        _OnMouse = defaultPluginOnMouse;
    }
    _OnMouseMove = (PluginOnMouseMoveFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnMouseMove");
    if (_OnMouseMove == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnMouseMove", _name.c_str());
        _OnMouseMove = defaultPluginOnMouseMove;
    }
    _OnMouseDrag = (PluginOnMouseDragFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnMouseDrag");
    if (_OnMouseDrag == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnMouseDrag", _name.c_str());
        _OnMouseDrag = defaultPluginOnMouseDrag;
    }
    _OnMouseScroll = (PluginOnMouseScrollFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnMouseScroll");
    if (_OnMouseScroll == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnMouseScroll", _name.c_str());
        _OnMouseScroll = defaultPluginOnMouseScroll;
    }
    _OnKeyboard = (PluginOnKeyboardFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnKeyboard");
    if (_OnKeyboard == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnKeyboard", _name.c_str());
        _OnKeyboard = defaultPluginOnKeyboard;
    }
    _OnWindowResize = (PluginOnWindowResizeFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowResize");
    if (_OnWindowResize == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowResize", _name.c_str());
        _OnWindowResize = defaultPluginOnWindowResize;
    }
    _OnWindowDragDrop = (PluginOnWindowDragDropFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowDragDrop");
    if (_OnWindowDragDrop == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowDragDrop", _name.c_str());
        _OnWindowDragDrop = defaultPluginOnWindowDragDrop;
    }
    _OnWindowIconify = (PluginOnWindowIconifyFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowIconify");
    if (_OnWindowIconify == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowIconify", _name.c_str());
        _OnWindowIconify = defaultPluginOnWindowIconify;
    }
    _OnWindowIconifyRestore =
      (PluginOnWindowIconifyRestoreFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowIconifyRestore");
    if (_OnWindowIconifyRestore == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowIconifyRestore", _name.c_str());
        _OnWindowIconifyRestore = defaultPluginOnWindowIconifyRestore;
    }
    _OnWindowMaximize = (PluginOnWindowMaximizeFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowMaximize");
    if (_OnWindowMaximize == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowMaximize", _name.c_str());
        _OnWindowMaximize = defaultPluginOnWindowMaximize;
    }
    _OnWindowMaximizeRestore =
      (PluginOnWindowMaximizeRestoreFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowMaximizeRestore");
    if (_OnWindowMaximizeRestore == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowMaximizeRestore", _name.c_str());
        _OnWindowMaximizeRestore = defaultPluginOnWindowMaximizeRestore;
    }
    PrototypeEngineContext context = {};
    context.application            = PrototypeEngineInternalApplication::application;
    context.shouldQuit             = PrototypeEngineInternalApplication::shouldQuit;
    context.database               = PrototypeEngineInternalApplication::database;
    context.window                 = PrototypeEngineInternalApplication::window;
    context.renderer               = PrototypeEngineInternalApplication::renderer;
    context.physics                = PrototypeEngineInternalApplication::physics;
    context.scene                  = PrototypeEngineInternalApplication::scene;
#if defined(PROTOTYPE_ENABLE_PROFILER)
    context.profiler = PrototypeEngineInternalApplication::profiler;
#endif
    context.traitSystemData = PrototypeEngineInternalApplication::traitSystemData;
    _LoadProtocol(&context, PrototypeLogger::data());
    return true;
}

bool
PrototypePluginInstance::reload()
{
    std::vector<std::pair<PrototypeObject*, Script*>> scripts;
    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (const auto& scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        auto    it     = script->codeLinks.find(_filepath);
        if (it != script->codeLinks.end()) {
            PrototypePluginInstance::safeCallEndProtocol(&it->second, scriptableObject);
            scripts.push_back({ scriptableObject, script });
            script->codeLinks.erase(_filepath);
        }
    }
    PROTOTYPE_DLL_CLOSE(_handle);
    _timestamp = PrototypeIo::filestamp(_filepath);
    PrototypeIo::copyFile(_filepath.c_str(), _name.c_str());
    _handle = PROTOTYPE_DLL_OPEN(_name.c_str(), RTLD_LAZY);
    if (!_handle) {
        PrototypeLogger::error("%s: Failed to reload", _name.c_str());
        return false;
    }
    _LoadProtocol = (PluginLoadProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginLoadProtocol");
    if (_LoadProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginLoadProtocol", _name.c_str());
        _LoadProtocol = defaultPluginLoadProtocol;
    }
    _ReloadProtocol = (PluginReloadProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginReloadProtocol");
    if (_ReloadProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginReloadProtocol", _name.c_str());
        _ReloadProtocol = defaultPluginReloadProtocol;
    }
    _StartProtocol = (PluginStartProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginStartProtocol");
    if (_StartProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginStartProtocol", _name.c_str());
        _StartProtocol = defaultPluginStartProtocol;
    }
    _UpdateProtocol = (PluginUpdateProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginUpdateProtocol");
    if (_UpdateProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginUpdateProtocol", _name.c_str());
        _UpdateProtocol = defaultPluginUpdateProtocol;
    }
    _EndProtocol = (PluginEndProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginEndProtocol");
    if (_EndProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginEndProtocol", _name.c_str());
        _EndProtocol = defaultPluginEndProtocol;
    }
    _UnloadProtocol = (PluginUnloadProtocolFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginUnloadProtocol");
    if (_UnloadProtocol == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginUnloadProtocol", _name.c_str());
        _UnloadProtocol = defaultPluginUnloadProtocol;
    }
    _OnMouse = (PluginOnMouseFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnMouse");
    if (_OnMouse == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnMouse", _name.c_str());
        _OnMouse = defaultPluginOnMouse;
    }
    _OnMouseMove = (PluginOnMouseMoveFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnMouseMove");
    if (_OnMouseMove == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnMouseMove", _name.c_str());
        _OnMouseMove = defaultPluginOnMouseMove;
    }
    _OnMouseDrag = (PluginOnMouseDragFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnMouseDrag");
    if (_OnMouseDrag == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnMouseDrag", _name.c_str());
        _OnMouseDrag = defaultPluginOnMouseDrag;
    }
    _OnMouseScroll = (PluginOnMouseScrollFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnMouseScroll");
    if (_OnMouseScroll == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnMouseScroll", _name.c_str());
        _OnMouseScroll = defaultPluginOnMouseScroll;
    }
    _OnKeyboard = (PluginOnKeyboardFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnKeyboard");
    if (_OnKeyboard == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnKeyboard", _name.c_str());
        _OnKeyboard = defaultPluginOnKeyboard;
    }
    _OnWindowResize = (PluginOnWindowResizeFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowResize");
    if (_OnWindowResize == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowResize", _name.c_str());
        _OnWindowResize = defaultPluginOnWindowResize;
    }
    _OnWindowDragDrop = (PluginOnWindowDragDropFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowDragDrop");
    if (_OnWindowDragDrop == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowDragDrop", _name.c_str());
        _OnWindowDragDrop = defaultPluginOnWindowDragDrop;
    }
    _OnWindowIconify = (PluginOnWindowIconifyFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowIconify");
    if (_OnWindowIconify == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowIconify", _name.c_str());
        _OnWindowIconify = defaultPluginOnWindowIconify;
    }
    _OnWindowIconifyRestore =
      (PluginOnWindowIconifyRestoreFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowIconifyRestore");
    if (_OnWindowIconifyRestore == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowIconifyRestore", _name.c_str());
        _OnWindowIconifyRestore = defaultPluginOnWindowIconifyRestore;
    }
    _OnWindowMaximize = (PluginOnWindowMaximizeFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowMaximize");
    if (_OnWindowMaximize == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowMaximize", _name.c_str());
        _OnWindowMaximize = defaultPluginOnWindowMaximize;
    }
    _OnWindowMaximizeRestore =
      (PluginOnWindowMaximizeRestoreFn)PROTOTYPE_DLL_GET_PROC_ADDRESS(_handle, "PluginOnWindowMaximizeRestore");
    if (_OnWindowMaximizeRestore == NULL) {
        PrototypeLogger::warn("%s: Failed to link with PluginOnWindowMaximizeRestore", _name.c_str());
        _OnWindowMaximizeRestore = defaultPluginOnWindowMaximizeRestore;
    }
    for (auto& pair : scripts) {
        Script*        script = pair.second;
        ScriptCodeLink link   = {};
        linkScript(&link);
        script->codeLinks.insert({ _filepath, std::move(link) });
    }

    PrototypeEngineContext context = {};
    context.application            = PrototypeEngineInternalApplication::application;
    context.shouldQuit             = PrototypeEngineInternalApplication::shouldQuit;
    context.database               = PrototypeEngineInternalApplication::database;
    context.window                 = PrototypeEngineInternalApplication::window;
    context.renderer               = PrototypeEngineInternalApplication::renderer;
    context.physics                = PrototypeEngineInternalApplication::physics;
    context.scene                  = PrototypeEngineInternalApplication::scene;
#if defined(PROTOTYPE_ENABLE_PROFILER)
    context.profiler = PrototypeEngineInternalApplication::profiler;
#endif
    context.traitSystemData = PrototypeEngineInternalApplication::traitSystemData;
    _ReloadProtocol(&context, PrototypeLogger::data());
    for (auto& pair : scripts) {
        PrototypeObject* object = pair.first;
        Script*          script = pair.second;
        auto             it     = script->codeLinks.find(_filepath);
        if (it != script->codeLinks.end()) { PrototypePluginInstance::safeCallStartProtocol(&it->second, object); }
    }

    return true;
}

bool
PrototypePluginInstance::update(PrototypeObject* object)
{
    return _UpdateProtocol(object);
}

bool
PrototypePluginInstance::unload()
{
    if (_handle != NULL) {
        _UnloadProtocol();
        PROTOTYPE_DLL_CLOSE(_handle);
    }
    return true;
}

void
PrototypePluginInstance::onMouse(PrototypeObject* object, i32 button, i32 action, i32 mods)
{
    _OnMouse(object, button, action, mods);
}

void
PrototypePluginInstance::onMouseMove(PrototypeObject* object, f64 x, f64 y)
{
    _OnMouseMove(object, x, y);
}

void
PrototypePluginInstance::onMouseDrag(PrototypeObject* object, i32 button, f64 x, f64 y)
{
    _OnMouseDrag(object, button, x, y);
}

void
PrototypePluginInstance::onMouseScroll(PrototypeObject* object, f64 x, f64 y)
{
    _OnMouseScroll(object, x, y);
}

void
PrototypePluginInstance::onKeyboard(PrototypeObject* object, i32 key, i32 scancode, i32 action, i32 mods)
{
    _OnKeyboard(object, key, scancode, action, mods);
}

void
PrototypePluginInstance::onWindowResize(PrototypeObject* object, i32 width, i32 height)
{
    _OnWindowResize(object, width, height);
}

void
PrototypePluginInstance::onWindowDragDrop(PrototypeObject* object, i32 numFiles, const char** names)
{
    _OnWindowDragDrop(object, numFiles, names);
}

void
PrototypePluginInstance::onWindowIconify(PrototypeObject* object)
{
    _OnWindowIconify(object);
}

void
PrototypePluginInstance::onWindowIconifyRestore(PrototypeObject* object)
{
    _OnWindowIconifyRestore(object);
}

void
PrototypePluginInstance::onWindowMaximize(PrototypeObject* object)
{
    _OnWindowMaximize(object);
}

void
PrototypePluginInstance::onWindowMaximizeRestore(PrototypeObject* object)
{
    _OnWindowMaximizeRestore(object);
}

const std::string&
PrototypePluginInstance::name() const
{
    return _name;
}

const std::string&
PrototypePluginInstance::filepath() const
{
    return _filepath;
}

time_t
PrototypePluginInstance::timestamp() const
{
    return _timestamp;
}

bool
PrototypePluginInstance::needsUpload() const
{
    return _needsUpload;
}

void
PrototypePluginInstance::linkScript(ScriptCodeLink* script)
{
    script->name                     = _name;
    script->filepath                 = _filepath;
    script->_LoadProtocol            = _LoadProtocol;
    script->_ReloadProtocol          = _ReloadProtocol;
    script->_StartProtocol           = _StartProtocol;
    script->_UpdateProtocol          = _UpdateProtocol;
    script->_EndProtocol             = _EndProtocol;
    script->_UnloadProtocol          = _UnloadProtocol;
    script->_OnMouse                 = _OnMouse;
    script->_OnMouseMove             = _OnMouseMove;
    script->_OnMouseDrag             = _OnMouseDrag;
    script->_OnMouseScroll           = _OnMouseScroll;
    script->_OnKeyboard              = _OnKeyboard;
    script->_OnWindowResize          = _OnWindowResize;
    script->_OnWindowDragDrop        = _OnWindowDragDrop;
    script->_OnWindowIconify         = _OnWindowIconify;
    script->_OnWindowIconifyRestore  = _OnWindowIconifyRestore;
    script->_OnWindowMaximize        = _OnWindowMaximize;
    script->_OnWindowMaximizeRestore = _OnWindowMaximizeRestore;
}

// void
// PrototypePluginInstance::safeCallLoadProtocol(ScriptCodeLink* script)
// {
//     PrototypeEngineContext context = {};
//     context.application            = PrototypeEngineInternalApplication::application;
//     context.shouldQuit             = PrototypeEngineInternalApplication::shouldQuit;
//     context.database               = PrototypeEngineInternalApplication::database;
//     context.window                 = PrototypeEngineInternalApplication::window;
//     context.renderer               = PrototypeEngineInternalApplication::renderer;
//     context.physics                = PrototypeEngineInternalApplication::physics;
//     context.scene                  = PrototypeEngineInternalApplication::scene;
//     context.profiler               = PrototypeEngineInternalApplication::profiler;
//     context.system                 = PrototypeEngineInternalApplication::system;
//     context.traitSystemData        = PrototypeEngineInternalApplication::traitSystemData;

//     TRY { script->_LoadProtocol(&context, PrototypeLogger::data()); }
//     CATCH
//     {
//         char buf[512];
//         snprintf(buf, sizeof(buf), "%s:%i\nException raised while calling LoadProtocol function", script->filepath.c_str(), 1);
//         PrototypeLogger::log(script->filepath.c_str(), 1, buf);
//     }
// }

void
PrototypePluginInstance::safeCallStartProtocol(const ScriptCodeLink* script, PrototypeObject* object)
{
    TRY { script->_StartProtocol(object); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling LoadProtocol function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallUpdateProtocol(const ScriptCodeLink* script, PrototypeObject* object)
{
    TRY { script->_UpdateProtocol(object); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling UpdateProtocol function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallEndProtocol(const ScriptCodeLink* script, PrototypeObject* object)
{
    TRY { script->_EndProtocol(object); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling EndProtocol function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallOnMouse(const ScriptCodeLink* script, PrototypeObject* object, i32 button, i32 action, i32 mods)
{
    TRY { script->_OnMouse(object, button, action, mods); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling OnMouse function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallOnMouseMove(const ScriptCodeLink* script, PrototypeObject* object, f64 x, f64 y)
{
    TRY { script->_OnMouseMove(object, x, y); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling OnMouseMove function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallOnMouseDrag(const ScriptCodeLink* script, PrototypeObject* object, i32 button, f64 x, f64 y)
{
    TRY { script->_OnMouseDrag(object, button, x, y); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling OnMouseDrag function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallOnMouseScroll(const ScriptCodeLink* script, PrototypeObject* object, f64 x, f64 y)
{
    TRY { script->_OnMouseScroll(object, x, y); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling OnMouseScroll function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallOnKeyboard(const ScriptCodeLink* script,
                                            PrototypeObject*      object,
                                            i32                   key,
                                            i32                   scancode,
                                            i32                   action,
                                            i32                   mods)
{
    TRY { script->_OnKeyboard(object, key, scancode, action, mods); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling OnKeyboard function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallOnWindowResize(const ScriptCodeLink* script, PrototypeObject* object, i32 width, i32 height)
{
    TRY { script->_OnWindowResize(object, width, height); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling OnWindowResize function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallOnWindowDragDrop(const ScriptCodeLink* script,
                                                  PrototypeObject*      object,
                                                  i32                   numFiles,
                                                  const char**          names)
{
    TRY { script->_OnWindowDragDrop(object, numFiles, names); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling OnWindowDragDrop function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallOnWindowIconify(const ScriptCodeLink* script, PrototypeObject* object)
{
    TRY { script->_OnWindowIconify(object); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling OnWindowIconify function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallOnWindowIconifyRestore(const ScriptCodeLink* script, PrototypeObject* object)
{
    TRY { script->_OnWindowIconifyRestore(object); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling OnWindowIconifyRestore function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallOnWindowMaximize(const ScriptCodeLink* script, PrototypeObject* object)
{
    TRY { script->_OnWindowMaximize(object); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling OnWindowMaximize function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}

void
PrototypePluginInstance::safeCallOnWindowMaximizeRestore(const ScriptCodeLink* script, PrototypeObject* object)
{
    TRY { script->_OnWindowMaximizeRestore(object); }
    CATCH
    {
        char buf[512];
        snprintf(buf,
                 sizeof(buf),
                 "%s:%s:%i\nException raised while calling OnWindowMaximizeRestore function",
                 ((PrototypeSceneNode*)object->parentNode())->name().c_str(),
                 script->filepath.c_str(),
                 1);
        PrototypeLogger::log(script->filepath.c_str(), 1, buf);
    }
}
