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

#include <PrototypeCommon/Types.h>

#include "../../include/PrototypeEngine/PrototypeEngineApi.h"

#include <PrototypeCommon/Definitions.h>
#include <PrototypeCommon/Maths.h>

#include <functional>
#include <limits>
#include <stack>
#include <string>

#define PROTOTYPE_BLACK        0.f, 0.f, 0.f
#define PROTOTYPE_DARKESTGRAY  0.1f, 0.1f, 0.1f
#define PROTOTYPE_DARKERGRAY   0.175f, 0.175f, 0.175f
#define PROTOTYPE_DARKGRAY     0.3f, 0.3f, 0.3f
#define PROTOTYPE_GRAY         0.5f, 0.5f, 0.5f
#define PROTOTYPE_LIGHTGRAY    0.6f, 0.6f, 0.6f
#define PROTOTYPE_LIGHTERGRAY  0.7f, 0.7f, 0.7f
#define PROTOTYPE_LIGHTESTGRAY 0.9f, 0.9f, 0.9f
#define PROTOTYPE_WHITE        1.0f, 1.0f, 1.0f
#define PROTOTYPE_YELLOW       0.9882f, 0.6392f, 0.0666f
#define PROTOTYPE_PLATINUM     0.898f, 0.898f, 0.898f
#define PROTOTYPE_RED          0.949f, 0.372f, 0.360f // #F25F5C, (242, 95, 92)
#define PROTOTYPE_GREEN        0.427f, 0.609f, 0.486f // #6D9B7C, (109, 155, 124)
#define PROTOTYPE_BLUE         0.003f, 0.729f, 0.937f // #01BAEF, (1, 186, 239)

// instructions recordings
typedef u32 PrototypeUiViewMaskType;

static const PrototypeUiViewMaskType PrototypeUiViewMask3DViewBit        = 0;
static const PrototypeUiViewMaskType PrototypeUiViewMask3DPaintBit       = 1;
static const PrototypeUiViewMaskType PrototypeUiViewMaskGameBit          = 2;
static const PrototypeUiViewMaskType PrototypeUiViewMaskTextureEditorBit = 3;
static const PrototypeUiViewMaskType PrototypeUiViewMaskTextEditorBit    = 4;
static const PrototypeUiViewMaskType PrototypeUiViewMaskInputSettingsBit = 5;
static const PrototypeUiViewMaskType PrototypeUiViewMaskHierarchyBit     = 6;
static const PrototypeUiViewMaskType PrototypeUiViewMaskTraitsBit        = 7;
static const PrototypeUiViewMaskType PrototypeUiViewMaskPropertiesBit    = 8;
static const PrototypeUiViewMaskType PrototypeUiViewMaskSettingsBit      = 9;
static const PrototypeUiViewMaskType PrototypeUiViewMaskRenderingInfoBit = 10;
static const PrototypeUiViewMaskType PrototypeUiViewMaskProfilerBit      = 11;
static const PrototypeUiViewMaskType PrototypeUiViewMaskCollidersBit     = 12;
static const PrototypeUiViewMaskType PrototypeUiViewMaskMaterialsBit     = 13;
static const PrototypeUiViewMaskType PrototypeUiViewMaskTexturesBit      = 14;
static const PrototypeUiViewMaskType PrototypeUiViewMaskShadersBit       = 15;
static const PrototypeUiViewMaskType PrototypeUiViewMaskMeshesBit        = 16;
static const PrototypeUiViewMaskType PrototypeUiViewMaskScenesBit        = 17;
static const PrototypeUiViewMaskType PrototypeUiViewMaskFramebuffersBit  = 18;
static const PrototypeUiViewMaskType PrototypeUiViewMaskConsoleBit       = 19;
static const PrototypeUiViewMaskType PrototypeUiViewMaskScriptsBit       = 20;
static const PrototypeUiViewMaskType PrototypeUiViewMaskBlueprintsBit    = 21;

static const PrototypeUiViewMaskType PrototypeUiViewMaskAll           = UINT32_MAX;
static const PrototypeUiViewMaskType PrototypeUiViewMask3DView        = 1 << PrototypeUiViewMask3DViewBit;
static const PrototypeUiViewMaskType PrototypeUiViewMask3DPaint       = 1 << PrototypeUiViewMask3DPaintBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskGame          = 1 << PrototypeUiViewMaskGameBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskTextureEditor = 1 << PrototypeUiViewMaskTextureEditorBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskTextEditor    = 1 << PrototypeUiViewMaskTextEditorBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskInputSettings = 1 << PrototypeUiViewMaskInputSettingsBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskHierarchy     = 1 << PrototypeUiViewMaskHierarchyBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskTraits        = 1 << PrototypeUiViewMaskTraitsBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskProperties    = 1 << PrototypeUiViewMaskPropertiesBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskSettings      = 1 << PrototypeUiViewMaskSettingsBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskRenderingInfo = 1 << PrototypeUiViewMaskRenderingInfoBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskProfiler      = 1 << PrototypeUiViewMaskProfilerBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskColliders     = 1 << PrototypeUiViewMaskCollidersBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskMaterials     = 1 << PrototypeUiViewMaskMaterialsBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskTextures      = 1 << PrototypeUiViewMaskTexturesBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskShaders       = 1 << PrototypeUiViewMaskShadersBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskMeshes        = 1 << PrototypeUiViewMaskMeshesBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskScenes        = 1 << PrototypeUiViewMaskScenesBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskFramebuffers  = 1 << PrototypeUiViewMaskFramebuffersBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskConsole       = 1 << PrototypeUiViewMaskConsoleBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskScripts       = 1 << PrototypeUiViewMaskScriptsBit;
static const PrototypeUiViewMaskType PrototypeUiViewMaskBlueprints    = 1 << PrototypeUiViewMaskBlueprintsBit;

enum
{
    PrototypeUIState_None             = 0,
    PrototypeUIState_SceneViewResized = 1 << 0,
    PrototypeUIState_HierarchyChanged = 1 << 1,
    PrototypeUIState_GuizmoUsed       = 1 << 2,
    PrototypeUIState_Iconified        = 1 << 3
};

typedef u32 PrototypeUIState_;

struct PrototypeErrorDialog
{
    void configure(std::string title, std::string text, bool canClose, bool canCancel)
    {
        _title     = title;
        _text      = text;
        _canClose  = canClose;
        _canCancel = canCancel;
    }

    const std::string& title() const { return _title; }
    const std::string& text() const { return _text; }
    const bool&        canClose() const { return _canClose; }
    const bool&        canCancel() const { return _canCancel; }

  private:
    std::string _title;
    std::string _text;
    bool        _canClose;
    bool        _canCancel;
};

struct PrototypeUiPrompt
{
    void configure(std::string                      title,
                   std::string                      text,
                   std::string                      buttonText,
                   bool                             canClose,
                   std::function<bool(std::string)> onSuccess,
                   std::function<bool()>            onCancel)
    {
        _onSuccess  = onSuccess;
        _onCancel   = onCancel;
        _title      = title;
        _text       = text;
        _buttonText = buttonText;
        _canClose   = canClose;
    }

    const std::function<bool(std::string)>& onSuccess() const { return _onSuccess; }
    const std::function<bool()>&            onCancel() const { return _onCancel; }
    const std::string&                      title() const { return _title; }
    const std::string&                      text() const { return _text; }
    const std::string&                      buttonText() const { return _buttonText; }
    const bool&                             canClose() const { return _canClose; }

  private:
    std::function<bool(std::string)> _onSuccess;
    std::function<bool()>            _onCancel;
    std::string                      _title;
    std::string                      _text;
    std::string                      _buttonText;
    bool                             _canClose;
};

struct PrototypeUiMaterialForm
{
    void configure(std::function<bool(PrototypeUiMaterialForm& form)> content)
    {
        memset(_name, 0, sizeof(_name));
        _baseColor = { 1.0f, 1.0f, 1.0f };
        _shader    = 0;
        _metallic  = 0.0f;
        _roughness = 1.0f;
        _content   = content;
    }
    char*                                                name() { return _name; }
    size_t                                               sizeofName() { return sizeof(_name); }
    PrototypeVec3&                                       baseColor() { return _baseColor; }
    i32&                                                 shader() { return _shader; }
    f32&                                                 metallic() { return _metallic; }
    f32&                                                 roughness() { return _roughness; }
    std::vector<i32>&                                    textures() { return _textures; }
    const std::function<bool(PrototypeUiMaterialForm&)>& content() { return _content; }

  private:
    char                                               _name[128];
    PrototypeVec3                                      _baseColor;
    i32                                                _shader;
    f32                                                _metallic;
    f32                                                _roughness;
    std::vector<i32>                                   _textures;
    std::function<bool(PrototypeUiMaterialForm& form)> _content;
};

struct PrototypeUiView;

struct PROTOTYPE_PURE_ABSTRACT PrototypeUI
{
    // initializes a window
    virtual bool init() = 0;

    // de-initializes a window
    virtual void deInit() = 0;

    // schedule a ui record pass
    virtual void scheduleRecordPass(PrototypeUiViewMaskType mask) = 0;

    // start recording instructions
    virtual void beginRecordPass() = 0;

    // stop recording instructions
    virtual void endRecordPass() = 0;

    // start describing the content of the current ui frame
    virtual void beginFrame(bool changed) = 0;

    // render the frame content
    virtual PrototypeUIState_ drawFrame(u32 fbid, i32 width, i32 height) = 0;

    // stop rendering on the current frame
    virtual void endFrame() = 0;

    // update function called each cycle ..
    virtual void update() = 0;

    // render the frame to the viewport
    virtual void render(i32 x, i32 y, i32 width, i32 height) = 0;

    // push an error message
    virtual void pushErrorDialog(PrototypeErrorDialog errDialog) = 0;

    // pop the last error message
    virtual void popErrorDialog() = 0;

    // signal buffers reload change
    virtual void signalBuffersChanged(bool status) = 0;

    // get whether buffers have reloaded or not
    virtual bool isBuffersChanged() = 0;

    // get whether mouse is being used in ui or not
    virtual bool needsMouse() = 0;

    // get whether keyboard is being used in ui or not
    virtual bool needsKeyboard() = 0;

    // get opened views
    virtual PrototypeUiViewMaskType openedViewsMask() = 0;

    // get scene view
    virtual PrototypeUiView* sceneView() = 0;
};