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

#include "PrototypeDatabase.h"

#include "Prototype3DLineBuffer.h"
#include "PrototypeEngine.h"
#include "PrototypeFrameBuffer.h"
#include "PrototypeMaterial.h"
#include "PrototypeMeshBuffer.h"
#include "PrototypePhysics.h"
#include "PrototypePluginInstance.h"
#include "PrototypeShaderBuffer.h"
#include "PrototypeTextureBuffer.h"

#include "PrototypeScene.h"
#include "PrototypeSceneLayer.h"
#include "PrototypeSceneNode.h"

#include <PrototypeCommon/IO.h>
#include <PrototypeCommon/Logger.h>

#include <filesystem>
#include <string>
#include <thread>
#include <unordered_map>

#ifdef PROTOTYPE_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <tchar.h>
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>

MemoryPool<Prototype3DLineBuffer, 10>   PrototypeDatabase::_3DLineBuffersPool;
MemoryPool<PrototypeMeshBuffer, 10>     PrototypeDatabase::_MeshBuffersPool;
MemoryPool<PrototypeShaderBuffer, 10>   PrototypeDatabase::_ShaderBuffersPool;
MemoryPool<PrototypeTextureBuffer, 10>  PrototypeDatabase::_TextureBuffersPool;
MemoryPool<PrototypeMaterial, 10>       PrototypeDatabase::_MaterialsPool;
MemoryPool<PrototypeFrameBuffer, 10>    PrototypeDatabase::_FramebuffersPool;
MemoryPool<PrototypeScene, 10>          PrototypeDatabase::_ScenesPool;
MemoryPool<PrototypeSceneFilter, 10>    PrototypeDatabase::_SceneFilersPool;
MemoryPool<PrototypeSceneLayer, 10>     PrototypeDatabase::_SceneLayersPool;
MemoryPool<PrototypeSceneNode, 10>      PrototypeDatabase::_SceneNodesPool;
MemoryPool<PrototypePluginInstance, 10> PrototypeDatabase::_PluginInstancesPool;

#if defined(PROTOTYPE_PLATFORM_WINDOWS)
DWORD PrototypeDirectoryWatchDataFlags = FILE_NOTIFY_CHANGE_LAST_WRITE;
#define MAX_DIRS   1024
#define MAX_FILES  10240
#define MAX_BUFFER 4096
typedef struct _DIRECTORY_INFO
{
    HANDLE     hDir;
    TCHAR      lpszDirName[MAX_PATH];
    CHAR       lpBuffer[MAX_BUFFER];
    DWORD      dwBufLength;
    OVERLAPPED Overlapped;
} DIRECTORY_INFO, *PDIRECTORY_INFO, *LPDIRECTORY_INFO;

DIRECTORY_INFO DirInfo[MAX_DIRS];
TCHAR          FileList[MAX_FILES * MAX_PATH];
DWORD          numDirs;
#endif

PrototypeWatchData::PrototypeWatchData()
  : _handle(nullptr)
{}

PrototypeWatchData::PrototypeWatchData(std::string path)
  : _path(std::move(path))
  , _handle(nullptr)
{}

PrototypeDatabase::~PrototypeDatabase()
{
    _3DLineBuffersPool.clear();
    _FramebuffersPool.clear();
    _MeshBuffersPool.clear();
    _ShaderBuffersPool.clear();
    _TextureBuffersPool.clear();
    _MaterialsPool.clear();
    _SceneNodesPool.clear();
    _SceneLayersPool.clear();
    _SceneFilersPool.clear();
    _ScenesPool.clear();
    _PluginInstancesPool.clear();
}

void
PrototypeDatabase::allocate()
{}

void
PrototypeDatabase::watchFs()
{
#ifdef PROTOTYPE_PLATFORM_WINDOWS
    startTime         = std::chrono::system_clock::now();
    shouldStopPolling = false;
    fsPollThread      = std::thread([&]() {
        char  buf[2048];
        DWORD nRet;
        BOOL  result = TRUE;
        char  filename[MAX_PATH];
        DirInfo[0].hDir = CreateFile(PROTOTYPE_ASSETS_PATH,
                                     GENERIC_READ | FILE_LIST_DIRECTORY,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                     NULL,
                                     OPEN_EXISTING,
                                     FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                                     NULL);

        if (DirInfo[0].hDir == INVALID_HANDLE_VALUE) { return; }

        lstrcpy(DirInfo[0].lpszDirName, PROTOTYPE_ASSETS_PATH);
        OVERLAPPED PollingOverlap;

        FILE_NOTIFY_INFORMATION* pNotify;
        int                      offset;
        PollingOverlap.OffsetHigh = 0;
        PollingOverlap.hEvent     = CreateEvent(NULL, TRUE, FALSE, NULL);
        while (result) {
            {
                std::lock_guard<std::mutex> lock(fsPollMutex);
                if (shouldStopPolling) break;
            }
            result = ReadDirectoryChangesW(DirInfo[0].hDir,
                                           &buf,
                                           sizeof(buf),
                                           TRUE,
                                           FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE,
                                           &nRet,
                                           &PollingOverlap,
                                           NULL);

            WaitForSingleObject(PollingOverlap.hEvent, INFINITE);
            offset = 0;
            std::unordered_set<std::string> directories;
            do {
                pNotify = (FILE_NOTIFY_INFORMATION*)((char*)buf + offset);
                strcpy(filename, "");
                int filenamelen = WideCharToMultiByte(
                  CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength / 2, filename, sizeof(filename), NULL, NULL);
                filename[pNotify->FileNameLength / 2] = '\0';
                std::string formattedFilename(filename);
                std::replace(formattedFilename.begin(), formattedFilename.end(), '\\', '/');
                formattedFilename = std::string(PROTOTYPE_ASSETS_PATH).append(formattedFilename);
                switch (pNotify->Action) {
                    case FILE_ACTION_ADDED: {
                        directories.insert(formattedFilename);
                    } break;
                    case FILE_ACTION_REMOVED: {
                    } break;
                    case FILE_ACTION_MODIFIED: {
                        directories.insert(formattedFilename);
                    } break;
                    case FILE_ACTION_RENAMED_OLD_NAME: {
                    } break;
                    case FILE_ACTION_RENAMED_NEW_NAME: {
                        directories.insert(formattedFilename);
                    } break;
                    default: {
                    } break;
                }
                offset += pNotify->NextEntryOffset;
            } while (pNotify->NextEntryOffset);
            std::vector<std::string> triggeredDirectories(directories.begin(), directories.end());
            refreshDirectories(triggeredDirectories);
        }
        CloseHandle(DirInfo[0].hDir);
    });
#endif
}

void
PrototypeDatabase::deallocate()
{
#ifdef PROTOTYPE_PLATFORM_WINDOWS
    {
        std::lock_guard<std::mutex> lock(fsPollMutex);
        shouldStopPolling = true;
    }
    auto                          end      = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed  = end - startTime;
    std::time_t                   end_time = std::chrono::system_clock::to_time_t(end);

    std::stringstream ss;
    ss << "finished computation at " << std::ctime(&end_time) << "elapsed time: " << elapsed.count() << "s\n";
    PrototypeIo::writeFileBlock(PROTOTYPE_ASSETS_PATH "usage.txt", ss.str());
    fsPollThread.join();
#endif
    PrototypeDatabase::lineBuffers.clear();
    PrototypeDatabase::pluginInstances.clear();
    PrototypeDatabase::scenes.clear();
    PrototypeDatabase::framebuffers.clear();
    PrototypeDatabase::materials.clear();
    PrototypeDatabase::meshBuffers.clear();
    PrototypeDatabase::shaderBuffers.clear();
    PrototypeDatabase::textureBuffers.clear();
}

void
PrototypeDatabase::dump(PrototypeScene* scene)
{
    PrototypeIo::createDirectory(PROTOTYPE_LOG_PATH(""));
    std::vector<std::thread> scenesThreads;
    scenesThreads.reserve(scenes.size());
    for (const auto& pair : scenes) {
        scenesThreads.emplace_back([=]() { dumpScene(pair.second); });
    }
    {
        {
            std::vector<nlohmann::json> jnodes;
            for (const auto& pair : meshBuffers) {
                nlohmann::json j;
                PrototypeMeshBuffer::to_json(j, *pair.second);
                jnodes.push_back(j);
            }
            std::stringstream ss;
            nlohmann::json    j = nlohmann::json(jnodes);
            ss << j;
            PrototypeLogger::dump(PROTOTYPE_LOG_PATH("database.meshbuffers.json"), ss.str().c_str());
        }
        {
            std::vector<nlohmann::json> jnodes;
            for (const auto& pair : shaderBuffers) {
                nlohmann::json j;
                PrototypeShaderBuffer::to_json(j, *pair.second);
                jnodes.push_back(j);
            }
            std::stringstream ss;
            nlohmann::json    j = nlohmann::json(jnodes);
            ss << j;
            PrototypeLogger::dump(PROTOTYPE_LOG_PATH("database.shaderbuffers.json"), ss.str().c_str());
        }
        {
            std::vector<nlohmann::json> jnodes;
            for (const auto& pair : textureBuffers) {
                nlohmann::json j;
                PrototypeTextureBuffer::to_json(j, *pair.second);
                jnodes.push_back(j);
            }
            std::stringstream ss;
            nlohmann::json    j = nlohmann::json(jnodes);
            ss << j;
            PrototypeLogger::dump(PROTOTYPE_LOG_PATH("database.texturebuffers.json"), ss.str().c_str());
        }
        {
            std::vector<nlohmann::json> jnodes;
            for (const auto& pair : materials) {
                nlohmann::json j;
                PrototypeMaterial::to_json(j, *pair.second);
                jnodes.push_back(j);
            }
            std::stringstream ss;
            nlohmann::json    j = nlohmann::json(jnodes);
            ss << j;
            PrototypeLogger::dump(PROTOTYPE_LOG_PATH("database.materials.json"), ss.str().c_str());
        }
        {
            std::vector<nlohmann::json> jnodes;
            for (const auto& pair : framebuffers) {
                nlohmann::json j;
                PrototypeFrameBuffer::to_json(j, *pair.second);
                jnodes.push_back(j);
            }
            std::stringstream ss;
            nlohmann::json    j = nlohmann::json(jnodes);
            ss << j;
            PrototypeLogger::dump(PROTOTYPE_LOG_PATH("database.framebuffers.json"), ss.str().c_str());
        }
    }
    for (auto& sceneThread : scenesThreads) { sceneThread.join(); }
}

void
PrototypeDatabase::dumpScene(const PrototypeScene* scene)
{
    std::stringstream ss;
    nlohmann::json    j;
    PrototypeScene::to_json(j, *scene);
    ss << j;
    PrototypeLogger::dump((PROTOTYPE_LOG_PATH("scenes.") + scene->name() + ".json").c_str(), ss.str().c_str());
}

void
PrototypeDatabase::refreshDirectories(std::vector<std::string> directories)
{
    if (directories.empty()) return;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    size_t const_assets_identifier  = sizeof(PROTOTYPE_ASSETS_PATH) - 1;
    size_t const_mesh_identifier    = sizeof(PROTOTYPE_ROOT_MESH_PATH) - 1;
    size_t const_shader_identifier  = sizeof(PROTOTYPE_ROOT_SHADER_PATH) - 1;
    size_t const_texture_identifier = sizeof(PROTOTYPE_ROOT_TEXTURE_PATH) - 1;
    size_t const_plugin_identifier  = sizeof(PROTOTYPE_ROOT_PLUGIN_PATH) - 1;

    size_t const_shader_renderer_specific_identifier           = 0;
    size_t const_shader_renderer_specific_extension_identifier = 0;
    switch (PrototypeEngineInternalApplication::renderingApi) {
        case PrototypeEngineERenderingApi_OPENGL4_1: {
            const_shader_renderer_specific_identifier           = sizeof(PROTOTYPE_OPENGL_SHADER_PATH("")) - 1;
            const_shader_renderer_specific_extension_identifier = sizeof("/vert.glsl") - 1;
        } break;
        case PrototypeEngineERenderingApi_OPENGLES_3_0: {
            const_shader_renderer_specific_identifier           = sizeof(PROTOTYPE_OPENGL_SHADER_PATH("")) - 1;
            const_shader_renderer_specific_extension_identifier = sizeof("/vert.glsl") - 1;
        } break;
        case PrototypeEngineERenderingApi_VULKAN_1: {
            const_shader_renderer_specific_identifier           = sizeof(PROTOTYPE_VULKAN_SHADER_PATH("")) - 1;
            const_shader_renderer_specific_extension_identifier = sizeof("/vert.spv") - 1;
        } break;
        default: {
            PrototypeLogger::fatal("renderingApi: Unimplemented!(Unreachable");
        } break;
    }

    for (auto& directory : directories) {
        std::string filetype = directory;
        if (filetype.substr(const_assets_identifier, const_mesh_identifier) == PROTOTYPE_ROOT_MESH_PATH) {
            std::string meshFilename = filetype.substr(const_assets_identifier + const_mesh_identifier);
            auto        it           = meshBuffers.find(meshFilename);
            if (it != meshBuffers.end()) {
                if (PrototypeIo::filestamp(it->second->fullpath()) != it->second->timestamp()) { it->second->stageChange(); }
            }
        } else if (filetype.substr(const_assets_identifier, const_shader_identifier) == PROTOTYPE_ROOT_SHADER_PATH) {
            std::string shaderFilename = filetype.substr(const_shader_renderer_specific_identifier);
            shaderFilename =
              shaderFilename.substr(0, shaderFilename.size() - const_shader_renderer_specific_extension_identifier);
            auto it = shaderBuffers.find(shaderFilename);
            if (it != shaderBuffers.end()) {
                bool changed = false;
                for (const auto& source : it->second->sources()) {
                    if (PrototypeIo::filestamp(source->fullpath) != source->timestamp) {
                        changed = true;
                        break;
                    }
                }
                if (changed) { it->second->stageChange(); }
            }
        } else if (filetype.substr(const_assets_identifier, const_texture_identifier) == PROTOTYPE_ROOT_TEXTURE_PATH) {
            std::string textureFilename = filetype.substr(const_assets_identifier + const_texture_identifier);
            auto        it              = textureBuffers.find(textureFilename);
            if (it != textureBuffers.end()) {
                if (PrototypeIo::filestamp(it->second->source().fullpath) != it->second->source().timestamp) {
                    it->second->stageChange();
                }
            }
        } else if (filetype.substr(const_assets_identifier, const_plugin_identifier) == PROTOTYPE_ROOT_PLUGIN_PATH) {
            std::string pluginFilename = filetype.substr(const_assets_identifier + const_plugin_identifier);
            auto        it             = pluginInstances.find(std::string(PROTOTYPE_PLUGIN_PATH("")).append(pluginFilename));
            if (it != pluginInstances.end()) {
                if (PrototypeIo::filestamp(it->second->filepath()) != it->second->timestamp()) { it->second->stageChange(); }
            } else {
                PrototypePluginInstance* plugin =
                  allocatePluginInstance(std::string(PROTOTYPE_PLUGIN_PATH("")).append(pluginFilename));
                auto valid = plugin->load();
                if (valid) {
                    pluginInstances.insert({ plugin->filepath(), plugin });
                } else {
                    deallocatePluginInstance(plugin);
                }
            }
        }
    }
}

Prototype3DLineBuffer*
PrototypeDatabase::allocate3DLineBuffer(const std::string& name)
{
    return _3DLineBuffersPool.newElement(name);
}

PrototypeMeshBuffer*
PrototypeDatabase::allocateMeshBuffer(const std::string& name)
{
    return _MeshBuffersPool.newElement(name);
}

PrototypeShaderBuffer*
PrototypeDatabase::allocateShaderBuffer(const std::string& name)
{
    return _ShaderBuffersPool.newElement(name);
}

PrototypeTextureBuffer*
PrototypeDatabase::allocateTextureBuffer(const std::string& name)
{
    return _TextureBuffersPool.newElement(name);
}

PrototypeMaterial*
PrototypeDatabase::allocateMaterial(const std::string& name)
{
    return _MaterialsPool.newElement(name);
}

PrototypeFrameBuffer*
PrototypeDatabase::allocateFramebuffer(const std::string& name,
                                       const std::string& shader,
                                       const u32          numColorAttachments,
                                       const bool         withDepthAttachment)
{
    return _FramebuffersPool.newElement(name, shader, numColorAttachments, withDepthAttachment);
}

PrototypeScene*
PrototypeDatabase::allocateScene(const std::string& name)
{
    return _ScenesPool.newElement(name);
}

PrototypeSceneFilter*
PrototypeDatabase::allocateSceneFilter(MASK_TYPE traitMask)
{
    return _SceneFilersPool.newElement(traitMask);
}

PrototypeSceneLayer*
PrototypeDatabase::allocateSceneLayer(const std::string& name)
{
    return _SceneLayersPool.newElement(name);
}

PrototypeSceneNode*
PrototypeDatabase::allocateSceneNode(const std::string& name)
{
    return _SceneNodesPool.newElement(name);
}

PrototypePluginInstance*
PrototypeDatabase::allocatePluginInstance(const std::string& name)
{
    return _PluginInstancesPool.newElement(name);
}

void
PrototypeDatabase::deallocate3DLineBuffer(Prototype3DLineBuffer* lineBuffer)
{
    auto it = lineBuffers.find(lineBuffer->name);
    if (it != lineBuffers.end()) { lineBuffers.erase(it); }
    _3DLineBuffersPool.deleteElement(lineBuffer);
}

void
PrototypeDatabase::deallocateMeshBuffer(PrototypeMeshBuffer* meshBuffer)
{
    auto it = meshBuffers.find(meshBuffer->name());
    if (it != meshBuffers.end()) { meshBuffers.erase(it); }
    _MeshBuffersPool.deleteElement(meshBuffer);
}

void
PrototypeDatabase::deallocateShaderBuffer(PrototypeShaderBuffer* shaderBuffer)
{
    auto it = shaderBuffers.find(shaderBuffer->name());
    if (it != shaderBuffers.end()) { shaderBuffers.erase(it); }
    _ShaderBuffersPool.deleteElement(shaderBuffer);
}

void
PrototypeDatabase::deallocateTextureBuffer(PrototypeTextureBuffer* textureBuffer)
{
    auto it = textureBuffers.find(textureBuffer->name());
    if (it != textureBuffers.end()) { textureBuffers.erase(it); }
    _TextureBuffersPool.deleteElement(textureBuffer);
}

void
PrototypeDatabase::deallocateMaterial(PrototypeMaterial* material)
{
    auto it = materials.find(material->name());
    if (it != materials.end()) { materials.erase(it); }
    _MaterialsPool.deleteElement(material);
}

void
PrototypeDatabase::deallocateFramebuffer(PrototypeFrameBuffer* framebuffer)
{
    auto it = framebuffers.find(framebuffer->name());
    if (it != framebuffers.end()) { framebuffers.erase(it); }
    _FramebuffersPool.deleteElement(framebuffer);
}

void
PrototypeDatabase::deallocateScene(PrototypeScene* scene)
{
    auto it = scenes.find(scene->name());
    if (it != scenes.end()) { scenes.erase(it); }
    _ScenesPool.deleteElement(scene);
}

void
PrototypeDatabase::deallocateSceneFilter(PrototypeSceneFilter* sceneFilter)
{
    for (auto it = scenes.begin(); it != scenes.end(); ++it) {
        auto sceneIt = sceneFilters.find(it->second);
        if (sceneIt == sceneFilters.end()) continue;
        auto filterIt = sceneIt->second.find(sceneFilter->traitMask());
        if (filterIt != sceneIt->second.end()) { sceneIt->second.erase(filterIt); }
    }
    _SceneFilersPool.deleteElement(sceneFilter);
}

void
PrototypeDatabase::deallocateSceneLayer(PrototypeSceneLayer* sceneLayer)
{
    for (auto it = scenes.begin(); it != scenes.end(); ++it) {
        auto sceneIt = sceneLayers.find(it->second);
        if (sceneIt == sceneLayers.end()) continue;
        auto layerIt = sceneIt->second.find(sceneLayer->name());
        if (layerIt != sceneIt->second.end()) { sceneIt->second.erase(layerIt); }
    }
    _SceneLayersPool.deleteElement(sceneLayer);
}

void
PrototypeDatabase::deallocateSceneNode(PrototypeSceneNode* sceneNode)
{
    for (auto it = scenes.begin(); it != scenes.end(); ++it) {
        auto sceneIt = sceneLayers.find(it->second);
        if (sceneIt == sceneLayers.end()) continue;
        auto nodeIt = sceneIt->second.find(sceneNode->name());
        if (nodeIt != sceneIt->second.end()) { sceneIt->second.erase(nodeIt); }
    }
    _SceneNodesPool.deleteElement(sceneNode);
}

void
PrototypeDatabase::deallocatePluginInstance(PrototypePluginInstance* pluginInstance)
{
    auto it = pluginInstances.find(pluginInstance->name());
    if (it != pluginInstances.end()) { pluginInstances.erase(it); }
    _PluginInstancesPool.deleteElement(pluginInstance);
}