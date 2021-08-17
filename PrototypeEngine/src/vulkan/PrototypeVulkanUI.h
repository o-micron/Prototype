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

#include "../core/PrototypeUI.h"

#include "PrototypeVulkan.h"
#include "PrototypeVulkanUiSceneView.h"

#include "../imgui/helpers.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_vulkan.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imguizmo.h"

#include <array>
#include <string>

struct PrototypeVulkanUI final : PrototypeUI
{
    PrototypeVulkanUI();

    ~PrototypeVulkanUI() = default;

    // initializes a window
    bool init() final;

    // de-initializes a window
    void deInit() final;

    // schedule a ui record pass
    void scheduleRecordPass(PrototypeUiViewMaskType mask) final;

    // start recording instructions
    void beginRecordPass() final;

    // stop recording instructions
    void endRecordPass() final;

    // start describing the content of the current ui frame
    void beginFrame(bool changed) final;

    // render the frame content
    PrototypeUIState_ drawFrame(u32 fbid, i32 width, i32 height) final;

    // stop rendering on the current frame
    void endFrame() final;

    // update function called each cycle ..
    void update() final;

    // render the frame to the viewport
    void render(i32 x, i32 y, i32 width, i32 height) final;

    // push an error message
    void pushErrorDialog(PrototypeErrorDialog errDialog) final;

    // pop the last error message
    void popErrorDialog() final;

    // signal buffers reload change
    void signalBuffersChanged(bool status) final;

    // get whether buffers have reloaded or not
    bool isBuffersChanged() final;

    // get whether mouse is being used in ui or not
    bool needsMouse() final;

    // get whether keyboard is being used in ui or not
    bool needsKeyboard() final;

    // get opened views
    PrototypeUiViewMaskType openedViewsMask() final;

    // get scene view
    PrototypeUiView* sceneView() final;

    void createVulkanData();
    void recreateVulkanData();
    void destroyVulkanData();
    void frameRender();
    void framePresent();

  private:
    ImGui_ImplVulkanH_Window         _wd;                  // 137 bytes
    std::stack<PrototypeErrorDialog> _errorDialogs;        // 80 bytes
    PtvDescriptor                    _descriptor;          // 48 bytes
    bool                             _isBuffersChanged;    // 1 byte
    PtvCommand                       _command;             // 32 bytes
    ImFont*                          _defaultFont;         // 8 bytes
    ImFont*                          _awesomeFont;         // 8 bytes
    VkRenderPass                     _renderPass;          // 8 bytes
    bool                             _swapChainRebuild;    // 1 byte
    bool                             _freezeResizingViews; // 1 byte
    PrototypeVulkanUiSceneView       _sceneView;
};