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

#include <nlohmann/json.hpp>

#include <unordered_set>

struct PrototypeObject;

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onEditDispatchHandlerFn)(PrototypeObject* o);

struct PrototypeEngineContext;
struct PrototypeLoggerData;
struct PrototypeObject;

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

struct ScriptCodeLink
{
    bool operator==(const ScriptCodeLink& other) const;
    bool operator<(const ScriptCodeLink& other) const;

    std::string name;
    std::string filepath;

  private:
    friend struct PrototypePluginInstance;
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
};

struct Attachable(Trait) Script
{
    PrototypeObject* object();
    static void      setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler);
    static void      onEditDispatch(PrototypeObject * o);
    static void      to_json(nlohmann::json & j, const Script& s);
    static void      from_json(const nlohmann::json& j, Script& s, PrototypeObject* o);

    std::unordered_map<std::string, ScriptCodeLink> codeLinks;

  private:
    friend struct PrototypeObject;
    PrototypeObject*               _object;
    static onEditDispatchHandlerFn _onEditDispatchHandler;
};
