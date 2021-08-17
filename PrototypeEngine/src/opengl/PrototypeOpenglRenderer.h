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

#include "PrototypeOpenGL.h"

#include "../core/PrototypeRenderer.h"
#include "PrototypeOpenglUI.h"

#include "../core/PrototypeVideoRecorder.h"

#include <PrototypeCommon/MemoryPool.h>

#include <deque>
#include <memory>

struct PrototypeOpenglWindow;
struct PrototypeObject;
struct PrototypeSceneNode;
struct PrototypePluginInstance;

struct PrototypeOpenglRenderer final : PrototypeRenderer
{
    explicit PrototypeOpenglRenderer(PrototypeOpenglWindow* window);

    ~PrototypeOpenglRenderer() final = default;

    // initializes a window
    bool init() final;

    // de-initializes a window
    void deInit() final;

    // updates the renderer, update ubos
    bool update() final;

    // render 3D objects
    bool render3D() final;

    // render ui
    bool render2D() final;

    // switches scenes
    void switchScenes(PrototypeScene* scene) final;

    // schedule a rendering pass
    void scheduleRecordPass() final;

    // start recording instructions
    void beginRecordPass() final;

    // stop recording instructions
    void endRecordPass() final;

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    // get ui
    PrototypeUI* ui() final;
#endif

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    // Camera* editorGameCamera() final;
    Camera* editorSceneCamera() final;
    // Camera* editorPaintCamera() final;
#else
    Camera* mainCamera() final;
#endif

    // call it to map buffers to gpu data
    void mapPrototypeMeshBuffer(PrototypeMeshBuffer* meshBuffer) final;

    // call it to map buffers to gpu data
    void mapPrototypeShaderBuffer(PrototypeShaderBuffer* shaderBuffer) final;

    // call it to map buffers to gpu data
    void mapPrototypeTextureBuffer(PrototypeTextureBuffer* textureBuffer) final;

    // call it to map buffers to gpu data
    void mapPrototypeMaterial(PrototypeMaterial* material) final;

    // called when a mesh buffer data needs to get uploaded to gpu memory
    void onMeshBufferGpuUpload(PrototypeMeshBuffer* meshBuffer) final;

    // called when a shader buffer data needs to get uploaded to gpu memory
    void onShaderBufferGpuUpload(PrototypeShaderBuffer* shaderBuffer) final;

    // called when a texture buffer data needs to get uploaded to gpu memory
    void onTextureBufferGpuUpload(PrototypeTextureBuffer* textureBuffer) final;

    // fetch a camera
    void fetchCamera(const std::string& name, void** data) final;

    // fetch the default mesh
    void fetchDefaultMesh(void** data) final;

    // fetch a mesh by name
    void fetchMesh(const std::string& name, void** data) final;

    // fetch the default shader
    void fetchDefaultShader(void** data) final;

    // fetch a shader by name
    void fetchShader(const std::string& name, void** data) final;

    // fetch the default texture
    void fetchDefaultTexture(void** data) final;

    // fetch a texture by name
    void fetchTexture(const std::string& name, void** data) final;

    // fetch the default material
    void fetchDefaultMaterial(void** data) final;

    // fetch a material by name
    void fetchMaterial(const std::string& name, void** data) final;

    // fetch the default framebuffer
    void fetchDefaultFramebuffer(void** data) final;

    // fetch a framebuffer by name
    void fetchFramebuffer(const std::string& name, void** data) final;

    // called when click triggers on the rendering viewport
    // [[nodiscard]] PrototypeSceneNode* onSceneViewClick(const glm::vec2& coordinates, const glm::vec2& Size) final;

    // called when mouse clicks events triggers
    void onMouse(i32 button, i32 action, i32 mods) final;

    // called when mouse cursor movement event triggers
    void onMouseMove(f64 x, f64 y) final;

    // called when mouse drag event triggers
    void onMouseDrag(i32 button, f64 x, f64 y) final;

    // called when mouse scroll event triggers
    void onMouseScroll(f64 x, f64 y) final;

    // called when keyboard keys events trigger
    void onKeyboard(i32 key, i32 scancode, i32 action, i32 mods) final;

    // called when window resize event triggers
    void onWindowResize(i32 width, i32 height) final;

    // called when drag and dropping files event triggers
    void onWindowDragDrop(i32 numFiles, const char** names) final;

    // get the list of available geometries
    [[nodiscard]] const std::unordered_map<std::string, PglGeometry*>& geometries() const;

    // get the list of available shaders
    [[nodiscard]] const std::unordered_map<std::string, PglShader*>& shaders() const;

    // get the list of available textures
    [[nodiscard]] const std::unordered_map<std::string, PglTexture*>& textures() const;

    // get the list of available materials
    [[nodiscard]] const std::unordered_map<std::string, PglMaterial*>& materials() const;

    // get the list of available framebuffers
    [[nodiscard]] const std::unordered_map<std::string, PglFramebuffer*>& framebuffers() const;

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    // get the editor game view camera
    PglCamera& pglEditorGameCamera();

    // get the editor scene view camera
    PglCamera& pglEditorSceneCamera();

    // get the editor paint view camera
    PglCamera& pglEditorPaintCamera();
#else
    // get the main camera
    PglCamera&      pglMainCamera();
#endif

    // sync material variables from shader in case it gets reloaded
    void onMaterialShaderUpdate(PglMaterial* material, PglShader* shader);

  private:
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    PglCamera _editorGameCamera;  // => 64 bytes <=
    PglCamera _editorSceneCamera; // => 64 bytes <=
    PglCamera _editorPaintCamera; // => 64 bytes <=
#else
    PglCamera       _mainCamera; // => 64 bytes <=
#endif
    std::unordered_map<std::string, PglShader*>      _shaders;               // => 56 bytes <=
    PrototypeOpenglWindow*                           _window;                // 8 bytes
    std::unordered_map<std::string, PglMaterial*>    _materials;             // => 56 bytes <=
    std::shared_ptr<PglSkybox>                       _skybox;                // 8 bytes
    std::unordered_map<std::string, PglGeometry*>    _geometries;            // => 56 bytes <=
    PglFramebuffer*                                  _fullscreenFramebuffer; // 8 bytes
    std::unordered_map<std::string, PglTexture*>     _textures;              // => 56 bytes <=
    std::unordered_map<std::string, PglFramebuffer*> _framebuffers;          // => 56 bytes <=
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    std::unique_ptr<PrototypeOpenglUI> _ui; // 8 bytes
#else
    PrototypeInput* _input;
#endif
    std::unordered_map<std::string, PglUniformBufferObject*> _uniformBufferObjects;     // => 56 bytes <=
    PrototypeUIState_                                        _uiState;                  // 4 bytes
    MemoryPool<PrototypeOpenglCommand, 100>                  _commandsPool;             // => 32 bytes< =
    MemoryPool<PglGeometry, 10>                              _geometriesPool;           // 32 bytes
    MemoryPool<PglShader, 10>                                _shadersPool;              // => 32 bytes <=
    MemoryPool<PglTexture, 10>                               _texturesPool;             // 32 bytes
    MemoryPool<PglMaterial, 10>                              _materialsPool;            // => 32 bytes <=
    MemoryPool<PglFramebuffer, 10>                           _framebuffersPool;         // 32 bytes
    MemoryPool<PglUniformBufferObject, 10>                   _uniformBufferObjectsPool; // => 32 bytes <=
    std::vector<PrototypeOpenglCommand*>                     _commands;                 // 24 bytes
    bool                                                     _needsRecord;              // 1 byte
    //  PrototypeVideoRecorder                                   _videoRecorder;        //
};