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

#include <PrototypeCommon/Maths.h>

struct PrototypeScene;
struct PrototypeSceneNode;
struct PrototypeUI;
struct PrototypeMeshBuffer;
struct PrototypeShaderBuffer;
struct PrototypeTextureBuffer;
struct PrototypeMaterial;
struct Camera;

struct PROTOTYPE_PURE_ABSTRACT PrototypeRenderer
{
    virtual ~PrototypeRenderer()                     = default;
    virtual bool init()                              = 0;
    virtual void deInit()                            = 0;
    virtual bool update()                            = 0;
    virtual bool render3D()                          = 0;
    virtual bool render2D()                          = 0;
    virtual void switchScenes(PrototypeScene* scene) = 0;

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    virtual PrototypeUI* ui() = 0;
#endif

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    // virtual Camera* editorGameCamera()  = 0;
    virtual Camera* editorSceneCamera() = 0;
    // virtual Camera* editorPaintCamera() = 0;
#else
    virtual Camera* mainCamera() = 0;
#endif

    virtual void scheduleRecordPass() = 0;
    virtual void beginRecordPass()    = 0;
    virtual void endRecordPass()      = 0;

    virtual void mapPrototypeMeshBuffer(PrototypeMeshBuffer* meshBuffer)          = 0;
    virtual void mapPrototypeShaderBuffer(PrototypeShaderBuffer* shaderBuffer)    = 0;
    virtual void mapPrototypeTextureBuffer(PrototypeTextureBuffer* textureBuffer) = 0;
    virtual void mapPrototypeMaterial(PrototypeMaterial* material)                = 0;

    virtual void onMeshBufferGpuUpload(PrototypeMeshBuffer* meshBuffer)          = 0;
    virtual void onShaderBufferGpuUpload(PrototypeShaderBuffer* shaderBuffer)    = 0;
    virtual void onTextureBufferGpuUpload(PrototypeTextureBuffer* textureBuffer) = 0;

    virtual void fetchCamera(const std::string& name, void** data)      = 0;
    virtual void fetchDefaultMesh(void** data)                          = 0;
    virtual void fetchMesh(const std::string& name, void** data)        = 0;
    virtual void fetchDefaultShader(void** data)                        = 0;
    virtual void fetchShader(const std::string& name, void** data)      = 0;
    virtual void fetchDefaultTexture(void** data)                       = 0;
    virtual void fetchTexture(const std::string& name, void** data)     = 0;
    virtual void fetchDefaultMaterial(void** data)                      = 0;
    virtual void fetchMaterial(const std::string& name, void** data)    = 0;
    virtual void fetchDefaultFramebuffer(void** data)                   = 0;
    virtual void fetchFramebuffer(const std::string& name, void** data) = 0;

    // [[nodiscard]] virtual PrototypeSceneNode* onSceneViewClick(const glm::vec2& coordinates, const glm::vec2& Size) = 0;
    virtual void onMouse(i32 button, i32 action, i32 mods)               = 0;
    virtual void onMouseMove(f64 xpos, f64 ypos)                         = 0;
    virtual void onMouseDrag(i32 button, f64 xoffset, f64 yoffset)       = 0;
    virtual void onMouseScroll(f64 xoffset, f64 yoffset)                 = 0;
    virtual void onKeyboard(i32 key, i32 scancode, i32 action, i32 mods) = 0;
    virtual void onWindowResize(i32 width, i32 height)                   = 0;
    virtual void onWindowDragDrop(i32 numFiles, const char** names)      = 0;
};