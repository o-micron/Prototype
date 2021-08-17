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

#pragma once

#include <PrototypeCommon/Definitions.h>
#include <PrototypeCommon/Types.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>

struct PrototypeEngineContext;
struct PrototypeLoggerData;
struct PrototypeObject;
struct ScriptCodeLink;

struct PrototypePluginInstance
{
    typedef bool(PROTOTYPE_DYNAMIC_FN_CALL* PluginLoadProtocolFn)(PrototypeEngineContext*, PrototypeLoggerData*);
    typedef bool(PROTOTYPE_DYNAMIC_FN_CALL* PluginReloadProtocolFn)(PrototypeEngineContext*, PrototypeLoggerData*);
    typedef bool(PROTOTYPE_DYNAMIC_FN_CALL* PluginStartProtocolFn)(PrototypeObject*);
    typedef bool(PROTOTYPE_DYNAMIC_FN_CALL* PluginUpdateProtocolFn)(PrototypeObject*);
    typedef bool(PROTOTYPE_DYNAMIC_FN_CALL* PluginEndProtocolFn)(PrototypeObject*);
    typedef bool(PROTOTYPE_DYNAMIC_FN_CALL* PluginUnloadProtocolFn)();

    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* PluginOnMouseFn)(PrototypeObject*, i32 button, i32 action, i32 mods);
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* PluginOnMouseMoveFn)(PrototypeObject*, f64 x, f64 y);
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* PluginOnMouseDragFn)(PrototypeObject*, i32 button, f64 x, f64 y);
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* PluginOnMouseScrollFn)(PrototypeObject*, f64 x, f64 y);
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* PluginOnKeyboardFn)(PrototypeObject*, i32 key, i32 scancode, i32 action, i32 mods);
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* PluginOnWindowResizeFn)(PrototypeObject*, i32 width, i32 height);
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* PluginOnWindowDragDropFn)(PrototypeObject*, i32 numFiles, const char** names);
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* PluginOnWindowIconifyFn)(PrototypeObject*);
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* PluginOnWindowIconifyRestoreFn)(PrototypeObject*);
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* PluginOnWindowMaximizeFn)(PrototypeObject*);
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* PluginOnWindowMaximizeRestoreFn)(PrototypeObject*);

    PrototypePluginInstance(const std::string& filepath);
    ~PrototypePluginInstance();

    void stageChange();
    void commitChange();

    bool load();
    bool reload();
    bool update(PrototypeObject* object);
    bool unload();

    void onMouse(PrototypeObject* object, i32 button, i32 action, i32 mods);
    void onMouseMove(PrototypeObject* object, f64 x, f64 y);
    void onMouseDrag(PrototypeObject* object, i32 button, f64 x, f64 y);
    void onMouseScroll(PrototypeObject* object, f64 x, f64 y);
    void onKeyboard(PrototypeObject* object, i32 key, i32 scancode, i32 action, i32 mods);
    void onWindowResize(PrototypeObject* object, i32 width, i32 height);
    void onWindowDragDrop(PrototypeObject* object, i32 numFiles, const char** names);
    void onWindowIconify(PrototypeObject* object);
    void onWindowIconifyRestore(PrototypeObject* object);
    void onWindowMaximize(PrototypeObject* object);
    void onWindowMaximizeRestore(PrototypeObject* object);

    const std::string& name() const;
    const std::string& filepath() const;
    time_t             timestamp() const;
    bool               needsUpload() const;

    void linkScript(ScriptCodeLink* script);

    static void safeCallStartProtocol(const ScriptCodeLink* script, PrototypeObject* object);
    static void safeCallUpdateProtocol(const ScriptCodeLink* script, PrototypeObject* object);
    static void safeCallEndProtocol(const ScriptCodeLink* script, PrototypeObject* object);
    static void safeCallOnMouse(const ScriptCodeLink* script, PrototypeObject* object, i32 button, i32 action, i32 mods);
    static void safeCallOnMouseMove(const ScriptCodeLink* script, PrototypeObject* object, f64 x, f64 y);
    static void safeCallOnMouseDrag(const ScriptCodeLink* script, PrototypeObject* object, i32 button, f64 x, f64 y);
    static void safeCallOnMouseScroll(const ScriptCodeLink* script, PrototypeObject* object, f64 x, f64 y);
    static void safeCallOnKeyboard(const ScriptCodeLink* script,
                                   PrototypeObject*      object,
                                   i32                   key,
                                   i32                   scancode,
                                   i32                   action,
                                   i32                   mods);
    static void safeCallOnWindowResize(const ScriptCodeLink* script, PrototypeObject* object, i32 width, i32 height);
    static void safeCallOnWindowDragDrop(const ScriptCodeLink* script, PrototypeObject* object, i32 numFiles, const char** names);
    static void safeCallOnWindowIconify(const ScriptCodeLink* script, PrototypeObject* object);
    static void safeCallOnWindowIconifyRestore(const ScriptCodeLink* script, PrototypeObject* object);
    static void safeCallOnWindowMaximize(const ScriptCodeLink* script, PrototypeObject* object);
    static void safeCallOnWindowMaximizeRestore(const ScriptCodeLink* script, PrototypeObject* object);

  private:
    PROTOTYPE_DLL_HANDLE_TYPE       _handle;
    PluginLoadProtocolFn            _LoadProtocol;
    PluginReloadProtocolFn          _ReloadProtocol;
    PluginStartProtocolFn           _StartProtocol;
    PluginUpdateProtocolFn          _UpdateProtocol;
    PluginEndProtocolFn             _EndProtocol;
    PluginUnloadProtocolFn          _UnloadProtocol;
    PluginOnMouseFn                 _OnMouse;
    PluginOnMouseMoveFn             _OnMouseMove;
    PluginOnMouseDragFn             _OnMouseDrag;
    PluginOnMouseScrollFn           _OnMouseScroll;
    PluginOnKeyboardFn              _OnKeyboard;
    PluginOnWindowResizeFn          _OnWindowResize;
    PluginOnWindowDragDropFn        _OnWindowDragDrop;
    PluginOnWindowIconifyFn         _OnWindowIconify;
    PluginOnWindowIconifyRestoreFn  _OnWindowIconifyRestore;
    PluginOnWindowMaximizeFn        _OnWindowMaximize;
    PluginOnWindowMaximizeRestoreFn _OnWindowMaximizeRestore;
    std::string                     _name;
    std::string                     _filepath;
    time_t                          _timestamp;
    bool                            _needsUpload;
};