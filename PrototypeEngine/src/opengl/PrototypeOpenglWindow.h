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

#include "../core/PrototypeWindow.h"

#include "PrototypeOpenGL.h"

typedef struct GLFWwindow GLFWwindow;
struct PrototypeEngineApplication;
struct PrototypeOpenglRenderer;

struct PrototypeOpenglWindow final : PrototypeWindow
{
    PrototypeOpenglWindow();

    ~PrototypeOpenglWindow() final = default;

    // initializes a window
    bool init(i32 width, i32 height) final;

    // de-initializes a window
    void deInit() final;

    // force set window size
    void setSize(i32 width, i32 height) final;

    // updates the window, poll events and swap buffers and so on ...
    bool update() final;

    // get window handle
    void* handle() final;

    // get refresh rate
    f32 refreshRate() final;

    // get time since start
    f64 time() final;

    // get delta time
    f64 deltaTime() final;

    // get window resolution
    glm::vec2 resolution() final;

    // get whether window needs to reload (chaing apis)
    bool needsReload() final;

    // get whether window needs to be inspected
    bool needsInspector() final;

    // get whether window is iconified or not
    bool isIconified() final;

    // get whether window is maximized or not
    bool isMaximized() final;

    // get whether control btn is being pressed
    bool isCtrlDown() final;

    // get whether shift btn is being pressed
    bool isShiftDown() final;

    // get whether alt btn is being pressed
    bool isAltDown() final;

    // get whether any of the 3 mouse buttons are being pressed
    glm::bvec3 isMouseDown() final;

    // get mouse cursor location
    glm::vec2 mouseLocation() final;

    // reset delta time counter
    void resetDeltaTime() final;

    // set window resolution
    void setResolution(i32 width, i32 height) final;

    // ask to open inspector for debugging purposes
    void setNeedsInspector() final;

    // close inspector if it's already open
    void consumeNeedsInspector() final;

    // called when mouse clicks events triggers
    void onMouseFn(i32 button, i32 action, i32 mods) final;

    // called when mouse cursor movement event triggers
    void onMouseMoveFn(f64 x, f64 y) final;

    // called when mouse scroll event triggers
    void onMouseScrollFn(f64 x, f64 y) final;

    // called when keyboard keys events trigger
    void onKeyboardFn(i32 key, i32 scancode, i32 action, i32 mods) final;

    // called when window resize event triggers
    void onWindowResizeFn(i32 width, i32 height) final;

    // called when drag and dropping files event triggers
    void onWindowDragDropFn(i32 numFiles, const char** names) final;

    // called when window iconify event triggers
    void onWindowIconifyFn() final;

    // called when window is restored from iconify event triggers
    void onWindowIconifyRestoreFn() final;

    // called when window maximize event triggers
    void onWindowMaximizeFn() final;

    // called when window is restored from maximize event triggers
    void onWindowMaximizeRestoreFn() final;

  private:
    friend struct PrototypeOpenglRenderer;

    GLFWwindow* _handle;            // 8 bytes
    f64         _time;              // 8 bytes
    f64         _deltaTime;         // 8 bytes
    glm::vec2   _resolution;        // 8 bytes
    glm::vec2   _mouseLocation;     // 8 bytes
    std::string _physicalDeviceStr; // 8 bytes
    glm::bvec3  _mouseDown;         // 3 bytes
    bool        _needsReload;       // 1 byte
    bool        _needsInspector;    // 1 byte
    bool        _isCtrlDown;        // 1 byte
    bool        _isShiftDown;       // 1 byte
    bool        _isAltDown;         // 1 byte
    bool        _isIconified;       // 1 byte
    bool        _isMaximized;       // 1 byte
};