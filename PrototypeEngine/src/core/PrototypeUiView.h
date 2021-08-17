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

#include "PrototypeUI.h"

struct PROTOTYPE_PURE_ABSTRACT PrototypeUiView
{   
    // init
    virtual void onInit() = 0;

    // update
    virtual void onUpdate() = 0;

    // when 2d editor ui is being drawn
    virtual PrototypeUIState_ onDrawFrame(PrototypeUIState_ state, void* awesomeFontTitle) = 0;

    // set the camera
    virtual void setCamera(void* camera) = 0;

    // get the camera
    virtual void* camera() = 0;

    // get whether the cursor is hovering over scene view
    virtual bool isHovered() = 0;

    // get the scene view size
    virtual glm::vec2 dimensions() = 0;

    // get the scene cursor location
    virtual glm::vec2 cursorCoordinates() = 0;

    // get view matrix of the view camera
    virtual const float* viewMatrix() = 0;

    // get projection matrix of the view camera
    virtual const float* projectionMatrix() = 0;

    // on mouse clicks, returns true if needs to discard event
    virtual bool onMouse(i32 button, i32 action, i32 mods) = 0;

    // on cursor move, returns true if needs to discard event
    virtual bool onMouseMove(f64 xpos, f64 ypos) = 0;

    // on mouse dragged, returns true if needs to discard event
    virtual bool onMouseDrag(i32 button, f64 xoffset, f64 yoffset) = 0;

    // on mouse scrolled, returns true if needs to discard event
    virtual bool onMouseScroll(f64 xoffset, f64 yoffset) = 0;

    // on keyboard taps, returns true if needs to discard event
    virtual bool onKeyboard(i32 key, i32 scancode, i32 action, i32 mods) = 0;

    // on window resize, returns true if needs to discard event
    virtual bool onWindowResize(i32 width, i32 height) = 0;

    // on drag dropped, returns true if needs to discard event
    virtual bool onWindowDragDrop(i32 numFiles, const char** names) = 0;
};
