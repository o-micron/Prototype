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
#include <PrototypeCommon/Maths.h>
#include <PrototypeCommon/Types.h>

#include <array>

struct PrototypeWindow;

struct PrototypeRenderer;

struct PROTOTYPE_PURE_ABSTRACT PrototypeWindow
{
    // deconstructor
    virtual ~PrototypeWindow() = default;

    // initializes a window
    virtual bool init(i32 width, i32 height) = 0;

    // de-initializes a window
    virtual void deInit() = 0;

    // force set window size
    virtual void setSize(i32 width, i32 height) = 0;

    // updates the window, poll events and swap buffers and so on ...
    virtual bool update() = 0;

    // get window handle
    virtual void* handle() = 0;

    // get refresh rate
    virtual f32 refreshRate() = 0;

    // get time since start
    virtual f64 time() = 0;

    // get delta time
    virtual f64 deltaTime() = 0;

    // get window resolution
    virtual glm::vec2 resolution() = 0;

    // get whether window needs to reload (chaing apis)
    virtual bool needsReload() = 0;

    // get whether window needs to be inspected
    virtual bool needsInspector() = 0;

    // get whether window is iconified or not
    virtual bool isIconified() = 0;

    // get whether window is maximized or not
    virtual bool isMaximized() = 0;

    // get whether control btn is being pressed
    virtual bool isCtrlDown() = 0;

    // get whether shift btn is being pressed
    virtual bool isShiftDown() = 0;

    // get whether alt btn is being pressed
    virtual bool isAltDown() = 0;

    // get whether any of the 3 mouse buttons are being pressed
    virtual glm::bvec3 isMouseDown() = 0;

    // get mouse cursor location
    virtual glm::vec2 mouseLocation() = 0;

    // reset delta time counter
    virtual void resetDeltaTime() = 0;

    // set window resolution
    virtual void setResolution(i32 width, i32 height) = 0;

    // ask to open inspector for debugging purposes
    virtual void setNeedsInspector() = 0;

    // close inspector if it's already open
    virtual void consumeNeedsInspector() = 0;

    // called when mouse clicks events triggers
    virtual void onMouseFn(i32 button, i32 action, i32 mods) = 0;

    // called when mouse cursor movement event triggers
    virtual void onMouseMoveFn(f64 x, f64 y) = 0;

    // called when mouse scroll event triggers
    virtual void onMouseScrollFn(f64 x, f64 y) = 0;

    // called when keyboard keys events trigger
    virtual void onKeyboardFn(i32 key, i32 scancode, i32 action, i32 mods) = 0;

    // called when window resize event triggers
    virtual void onWindowResizeFn(i32 width, i32 height) = 0;

    // called when drag and dropping files event triggers
    virtual void onWindowDragDropFn(i32 numFiles, const char** names) = 0;

    // called when window iconify event triggers
    virtual void onWindowIconifyFn() = 0;

    // called when window is restored from iconify event triggers
    virtual void onWindowIconifyRestoreFn() = 0;

    // called when window maximize event triggers
    virtual void onWindowMaximizeFn() = 0;

    // called when window is restored from maximize event triggers
    virtual void onWindowMaximizeRestoreFn() = 0;
};