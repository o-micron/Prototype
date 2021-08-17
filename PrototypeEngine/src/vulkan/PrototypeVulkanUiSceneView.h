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

#include "../core/PrototypeUiView.h"
#include "PrototypeVulkan.h"

#include "../imgui/helpers.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imguizmo.h"

struct PrototypeInput;
struct PrototypeSceneNode;

struct PrototypeVulkanUiSceneView final : PrototypeUiView
{
    PrototypeVulkanUiSceneView();

    ~PrototypeVulkanUiSceneView();

    // init
    void onInit() final;

    // update
    void onUpdate() final;

    // when 2d editor ui is being drawn
    PrototypeUIState_ onDrawFrame(PrototypeUIState_ state, void* awesomeFontTitle) final;

    // set the camera
    void setCamera(void* camera) final;

    // get the camera
    void* camera() final;

    // get whether the cursor is hovering over scene view
    bool isHovered() final;

    // get the scene view size
    glm::vec2 dimensions() final;

    // get the scene cursor location
    glm::vec2 cursorCoordinates() final;

    // get view matrix of the view camera
    const float* viewMatrix() final;

    // get projection matrix of the view camera
    const float* projectionMatrix() final;

    // on mouse clicks, returns true if needs to discard event
    bool onMouse(i32 button, i32 action, i32 mods) final;

    // on cursor move, returns true if needs to discard event
    bool onMouseMove(f64 xpos, f64 ypos) final;

    // on mouse dragged, returns true if needs to discard event
    bool onMouseDrag(i32 button, f64 xoffset, f64 yoffset) final;

    // on mouse scrolled, returns true if needs to discard event
    bool onMouseScroll(f64 xoffset, f64 yoffset) final;

    // on keyboard taps, returns true if needs to discard event
    bool onKeyboard(i32 key, i32 scancode, i32 action, i32 mods) final;

    // on window resize, returns true if needs to discard event
    bool onWindowResize(i32 width, i32 height) final;

    // on drag dropped, returns true if needs to discard event
    bool onWindowDragDrop(i32 numFiles, const char** names) final;

    // switch transformation operation guizmo
    void setGuizmoOperation(ImGuizmo::OPERATION operation);

    // get the current transformation operation guizmo
    ImGuizmo::OPERATION guizmoOperation();

    // set guizmo world vs local
    void setGuizmoMode(ImGuizmo::MODE mode);

    // toggle guizmo mode world/local
    void toggleGuizmoMode();

    // get the guizmo world vs local mode
    ImGuizmo::MODE guizmoMode();

    // is guizmo being used
    bool isUsingGuizmo();

  private:
    // called when click triggers on the rendering viewport
    [[nodiscard]] PrototypeSceneNode* onSceneViewClick(const glm::vec2& coordinates, const glm::vec2& Size);

    PrototypeInput*     _input;
    PvtCamera*          _camera;
    glm::vec2           _dimensions;
    glm::vec2           _cursorCoordinates;
    ImGuizmo::OPERATION _guizmoOperation;
    ImGuizmo::MODE      _guizmoMode;
    bool                _isHovered;
    bool                _isUsingGuizmo;
};
