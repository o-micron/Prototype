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

#include "../../include/PrototypeEngine/PrototypeEngineApi.h"

#include <PrototypeTraitSystem/PrototypeTraitSystemTypes.h>

#include <PrototypeCommon/Definitions.h>
#include <PrototypeCommon/MemoryPool.h>

#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>

struct PrototypeMeshBuffer;
struct PrototypeMesh;
struct PrototypeShaderBuffer;
struct PrototypeShader;
struct PrototypeTextureBuffer;
struct PrototypeTexture;
struct Prototype3DLineBuffer;

struct PrototypeMaterial;
struct PrototypeFrameBuffer;

struct PrototypeScene;
struct PrototypeSceneFilter;
struct PrototypeSceneLayer;
struct PrototypeSceneNode;

struct PrototypePluginInstance;

typedef void* HANDLE;

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onMeshBufferReloadFn)(PrototypeMeshBuffer* meshBuffer);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onShaderBufferReloadFn)(PrototypeShaderBuffer* shaderBuffer);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onTextureBufferReloadFn)(PrototypeTextureBuffer* textureBuffer);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onMaterialsReloadFn)(PrototypeMaterial* material);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onFramebufferReloadFn)(PrototypeFrameBuffer* framebuffer);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onScenesReloadFn)(PrototypeScene* scene);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onPluginInstancesReloadFn)(PrototypePluginInstance* pluginInstance);

struct PrototypeWatchData
{
    PrototypeWatchData();
    explicit PrototypeWatchData(std::string path);

    std::string _path;
    HANDLE      _handle;
};

struct PrototypeDatabase
{
    PrototypeDatabase() = default;
    ~PrototypeDatabase();

    void allocate();
    void watchFs();
    void deallocate();
    void dump(PrototypeScene* scene);

    Prototype3DLineBuffer*   allocate3DLineBuffer(const std::string& name);
    PrototypeMeshBuffer*     allocateMeshBuffer(const std::string& name);
    PrototypeShaderBuffer*   allocateShaderBuffer(const std::string& name);
    PrototypeTextureBuffer*  allocateTextureBuffer(const std::string& name);
    PrototypeMaterial*       allocateMaterial(const std::string& name);
    PrototypeFrameBuffer*    allocateFramebuffer(const std::string& name,
                                                 const std::string& shader,
                                                 u32                numColorAttachments,
                                                 bool               withDepthAttachment);
    PrototypeScene*          allocateScene(const std::string& name);
    PrototypeSceneFilter*    allocateSceneFilter(MASK_TYPE traitMask);
    PrototypeSceneLayer*     allocateSceneLayer(const std::string& name);
    PrototypeSceneNode*      allocateSceneNode(const std::string& name);
    PrototypePluginInstance* allocatePluginInstance(const std::string& name);

    void deallocate3DLineBuffer(Prototype3DLineBuffer* lineBuffer);
    void deallocateMeshBuffer(PrototypeMeshBuffer* meshBuffer);
    void deallocateShaderBuffer(PrototypeShaderBuffer* shaderBuffer);
    void deallocateTextureBuffer(PrototypeTextureBuffer* textureBuffer);
    void deallocateMaterial(PrototypeMaterial* material);
    void deallocateFramebuffer(PrototypeFrameBuffer* framebuffer);
    void deallocateScene(PrototypeScene* scene);
    void deallocateSceneFilter(PrototypeSceneFilter* sceneFilter);
    void deallocateSceneLayer(PrototypeSceneLayer* sceneLayer);
    void deallocateSceneNode(PrototypeSceneNode* sceneNode);
    void deallocatePluginInstance(PrototypePluginInstance* pluginInstance);

    std::unordered_map<std::string, Prototype3DLineBuffer*>                          lineBuffers;
    std::unordered_map<std::string, PrototypeMeshBuffer*>                            meshBuffers;
    std::unordered_map<std::string, PrototypeShaderBuffer*>                          shaderBuffers;
    std::unordered_map<std::string, PrototypeTextureBuffer*>                         textureBuffers;
    std::unordered_map<std::string, PrototypeMaterial*>                              materials;
    std::unordered_map<std::string, PrototypeFrameBuffer*>                           framebuffers;
    std::unordered_map<std::string, PrototypeScene*>                                 scenes;
    std::map<PrototypeScene*, std::unordered_map<MASK_TYPE, PrototypeSceneFilter*>>  sceneFilters;
    std::map<PrototypeScene*, std::unordered_map<std::string, PrototypeSceneLayer*>> sceneLayers;
    std::map<PrototypeScene*, std::unordered_map<std::string, PrototypeSceneNode*>>  sceneNodes;
    std::unordered_map<std::string, PrototypePluginInstance*>                        pluginInstances;

  private:
    void dumpScene(const PrototypeScene* scene);
    void refreshDirectories(std::vector<std::string> directories);

    std::vector<onMeshBufferReloadFn>      meshBuffersChangeCallbacks;
    std::vector<onShaderBufferReloadFn>    shaderBuffersChangeCallbacks;
    std::vector<onTextureBufferReloadFn>   textureBuffersChangeCallbacks;
    std::vector<onMaterialsReloadFn>       materialsChangeCallbacks;
    std::vector<onFramebufferReloadFn>     framebuffersChangeCallbacks;
    std::vector<onScenesReloadFn>          scenesChangeCallbacks;
    std::vector<onPluginInstancesReloadFn> pluginInstancesChangeCallbacks;
    std::thread                            fsPollThread;
    std::mutex                             fsPollMutex;
    bool                                   shouldStopPolling;
    std::chrono::system_clock::time_point  startTime;

    static MemoryPool<Prototype3DLineBuffer, 10>   _3DLineBuffersPool;
    static MemoryPool<PrototypeMeshBuffer, 10>     _MeshBuffersPool;
    static MemoryPool<PrototypeShaderBuffer, 10>   _ShaderBuffersPool;
    static MemoryPool<PrototypeTextureBuffer, 10>  _TextureBuffersPool;
    static MemoryPool<PrototypeMaterial, 10>       _MaterialsPool;
    static MemoryPool<PrototypeFrameBuffer, 10>    _FramebuffersPool;
    static MemoryPool<PrototypeScene, 10>          _ScenesPool;
    static MemoryPool<PrototypeSceneFilter, 10>    _SceneFilersPool;
    static MemoryPool<PrototypeSceneLayer, 10>     _SceneLayersPool;
    static MemoryPool<PrototypeSceneNode, 10>      _SceneNodesPool;
    static MemoryPool<PrototypePluginInstance, 10> _PluginInstancesPool;
};