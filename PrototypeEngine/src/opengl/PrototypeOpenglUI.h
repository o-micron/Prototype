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

#include "../core/PrototypeUI.h"

#include "PrototypeOpenGL.h"
#include "PrototypeOpenglUiSceneView.h"

#include "../imgui/helpers.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imguizmo.h"
#include "../imgui/implot.h"

#include <stack>
#include <string>

struct PrototypeSceneNode;
struct PrototypeSceneLayer;

struct PrototypeClipboard
{
    PrototypeClipboard()
    {
        type[0] = "";
        data[0] = "";
    }
    std::string type[1];
    std::string data[1];
};

enum PrototypeConfigurationSelectionAssetType_
{
    PrototypeConfigurationSelectionAssetType_None,

    PrototypeConfigurationSelectionAssetType_Mesh,
    PrototypeConfigurationSelectionAssetType_Shader,
    PrototypeConfigurationSelectionAssetType_Texture,
    PrototypeConfigurationSelectionAssetType_Material,

    PrototypeConfigurationSelectionAssetType_Count
};

struct PrototypeConfigurationSelection
{
    PrototypeConfigurationSelection()
      : type(PrototypeConfigurationSelectionAssetType_None)
      , mesh(nullptr)
    {}

    PrototypeConfigurationSelectionAssetType_ type;
    union
    {
        struct
        {
            PglGeometry* mesh;
        };
        struct
        {
            PglShader* shader;
        };
        struct
        {
            PglTexture* texture;
        };
        struct
        {
            PglMaterial* material;
        };
    };
};

struct PrototypeOpenglUIInstructions
{
    PrototypeOpenglUIInstructions()
    {
        sceneNodes.clear();
        sceneNodesSearchBuffer[0] = '\0';

        meshes.clear();
        meshesSearchBuff[0] = '\0';

        colliders.clear();
        collidersSearchBuff[0] = '\0';

        shaders.clear();
        shadersSearchBuff[0] = '\0';

        textures.clear();
        texturesSearchBuff[0] = '\0';

        materials.clear();
        materialsSearchBuff[0] = '\0';

        framebuffers.clear();
        framebuffersSearchBuff[0] = '\0';
    }

    // nodes
    struct InternalSceneNode
    {
        union
        {
            struct
            {
                PrototypeSceneNode* node;
            };
            struct
            {
                PrototypeSceneLayer* layer;
            };
        };
        bool isLayer;
        int  ChildIdx;
        int  ChildCount;
    };
    std::vector<InternalSceneNode> sceneNodes;
    char                           sceneNodesSearchBuffer[256];

    // meshes
    std::vector<PglGeometry*> meshes;
    char                      meshesSearchBuff[256];

    // colliders
    std::vector<std::pair<std::string, std::string>> colliders;
    char                                             collidersSearchBuff[256];

    // shaders
    std::vector<PglShader*> shaders;
    char                    shadersSearchBuff[256];

    // textures
    std::vector<PglTexture*> textures;
    char                     texturesSearchBuff[256];

    // materials
    std::vector<PglMaterial*> materials;
    char                      materialsSearchBuff[256];

    // framebuffers
    std::vector<PglFramebuffer*> framebuffers;
    char                         framebuffersSearchBuff[256];
};

struct PrototypeOpenglUI final : PrototypeUI
{
    PrototypeOpenglUI();

    ~PrototypeOpenglUI() = default;

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

  private:
    std::stack<PrototypeUiPrompt>       _prompts;             // 80 bytes
    std::stack<PrototypeErrorDialog>    _errorDialogs;        // 80 bytes
    std::stack<PrototypeUiMaterialForm> _materialforms;       // 80 bytes
    PrototypeClipboard                  _clipboard;           // 16 bytes
    GLuint                              _playBtnImg;          // 4 bytes
    GLuint                              _reloadBtnImg;        // 4 bytes
    GLuint                              _pauseBtnImg;         // 4 bytes
    GLuint                              _importBtnImg;        // 4 bytes
    GLuint                              _exportBtnImg;        // 4 bytes
    bool                                _isBuffersChanged;    // 1 byte
    bool                                _freezeResizingViews; // 1 byte
    PrototypeUiViewMaskType             _recordMask;
    PrototypeUiViewMaskType             _openViewsMask;
    PrototypeOpenglUIInstructions       _recordedInstructions;
    PrototypeOpenglUiSceneView          _sceneView;
};