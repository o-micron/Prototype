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

#include "PrototypeEngine.h"
#include "../bullet/PrototypeBulletPhysics.h"
#include "../opengl/PrototypeOpenglWindow.h"
#include "../physx/PrototypePhysxPhysics.h"
#include "../vulkan/PrototypeVulkanWindow.h"
#include "PrototypeDatabase.h"
#include "PrototypePhysics.h"
#include "PrototypePipelines.h"
#include "PrototypePluginInstance.h"
#include "PrototypeProfiler.h"
#include "PrototypeRenderer.h"
#include "PrototypeScene.h"
#include "PrototypeSceneLoader.h"
#include "PrototypeShaderBuffer.h"
#include "PrototypeShortcuts.h"
#include "PrototypeStaticInitializer.h"
#include "PrototypeTextureBuffer.h"
#include "PrototypeUI.h"

#include "PrototypeSceneNode.h"

#include <PrototypeCommon/IO.h>
#include <PrototypeCommon/Logger.h>
#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include <chrono>
#include <filesystem>
#include <fstream>

PrototypeEngineApplication    PrototypeEngineInternalApplication::application;
PrototypeEngineERenderingApi_ PrototypeEngineInternalApplication::renderingApi;
PrototypeEngineEPhysicsApi_   PrototypeEngineInternalApplication::physicsApi;
bool                          PrototypeEngineInternalApplication::shouldQuit;
PrototypeDatabase*            PrototypeEngineInternalApplication::database;
PrototypeWindow*              PrototypeEngineInternalApplication::window;
PrototypeRenderer*            PrototypeEngineInternalApplication::renderer;
PrototypePhysics*             PrototypeEngineInternalApplication::physics;
PrototypeScene*               PrototypeEngineInternalApplication::scene;
#if defined(PROTOTYPE_ENABLE_PROFILER)
PrototypeProfiler* PrototypeEngineInternalApplication::profiler;
#endif
void** PrototypeEngineInternalApplication::traitSystemData;

#define PROTOTYOE_TRAIT_SYSTEM_SET_CALLBACKS()                                                                                   \
    PrototypeTraitSystem::setCameraTraitAddCbFnPtr(shortcutDefaultCameraTraitAddInitializer);                                    \
    PrototypeTraitSystem::setTransformTraitAddCbFnPtr(shortcutDefaultTransformTraitAddInitializer);                              \
    PrototypeTraitSystem::setMeshRendererTraitAddCbFnPtr(shortcutDefaultMeshRendererTraitAddInitializer);                        \
    PrototypeTraitSystem::setRigidbodyTraitAddCbFnPtr(shortcutDefaultRigidbodyTraitAddInitializer);                              \
    PrototypeTraitSystem::setScriptTraitAddCbFnPtr(shortcutDefaultScriptTraitAddInitializer);                                    \
    PrototypeTraitSystem::setColliderTraitAddCbFnPtr(shortcutDefaultColliderTraitAddInitializer);                                \
    PrototypeTraitSystem::setVehicleChasisTraitAddCbFnPtr(shortcutDefaultVehicleChasisTraitAddInitializer);                      \
                                                                                                                                 \
    PrototypeTraitSystem::setCameraTraitReuseCbFnPtr(shortcutDefaultCameraTraitReuseInitializer);                                \
    PrototypeTraitSystem::setTransformTraitReuseCbFnPtr(shortcutDefaultTransformTraitReuseInitializer);                          \
    PrototypeTraitSystem::setMeshRendererTraitReuseCbFnPtr(shortcutDefaultMeshRendererTraitReuseInitializer);                    \
    PrototypeTraitSystem::setRigidbodyTraitReuseCbFnPtr(shortcutDefaultRigidbodyTraitReuseInitializer);                          \
    PrototypeTraitSystem::setScriptTraitReuseCbFnPtr(shortcutDefaultScriptTraitReuseInitializer);                                \
    PrototypeTraitSystem::setColliderTraitReuseCbFnPtr(shortcutDefaultColliderTraitReuseInitializer);                            \
    PrototypeTraitSystem::setVehicleChasisTraitReuseCbFnPtr(shortcutDefaultVehicleChasisTraitReuseInitializer);                  \
                                                                                                                                 \
    PrototypeTraitSystem::setCameraTraitRemoveCbFnPtr(shortcutDefaultCameraTraitRemoveInitializer);                              \
    PrototypeTraitSystem::setTransformTraitRemoveCbFnPtr(shortcutDefaultTransformTraitRemoveInitializer);                        \
    PrototypeTraitSystem::setMeshRendererTraitRemoveCbFnPtr(shortcutDefaultMeshRendererTraitRemoveInitializer);                  \
    PrototypeTraitSystem::setRigidbodyTraitRemoveCbFnPtr(shortcutDefaultRigidbodyTraitRemoveInitializer);                        \
    PrototypeTraitSystem::setScriptTraitRemoveCbFnPtr(shortcutDefaultScriptTraitRemoveInitializer);                              \
    PrototypeTraitSystem::setColliderTraitRemoveCbFnPtr(shortcutDefaultColliderTraitRemoveInitializer);                          \
    PrototypeTraitSystem::setVehicleChasisTraitRemoveCbFnPtr(shortcutDefaultVehicleChasisTraitRemoveInitializer);                \
                                                                                                                                 \
    PrototypeTraitSystem::setCameraTraitLogCbFnPtr(shortcutDefaultCameraTraitLogInitializer);                                    \
    PrototypeTraitSystem::setTransformTraitLogCbFnPtr(shortcutDefaultTransformTraitLogInitializer);                              \
    PrototypeTraitSystem::setMeshRendererTraitLogCbFnPtr(shortcutDefaultMeshRendererTraitLogInitializer);                        \
    PrototypeTraitSystem::setRigidbodyTraitLogCbFnPtr(shortcutDefaultRigidbodyTraitLogInitializer);                              \
    PrototypeTraitSystem::setScriptTraitLogCbFnPtr(shortcutDefaultScriptTraitLogInitializer);                                    \
    PrototypeTraitSystem::setColliderTraitLogCbFnPtr(shortcutDefaultColliderTraitLogInitializer);                                \
    PrototypeTraitSystem::setVehicleChasisTraitLogCbFnPtr(shortcutDefaultVehicleChasisTraitLogInitializer);

#define PROTOTYOE_TRAIT_SYSTEM_UNSET_CALLBACKS()                                                                                 \
    PrototypeTraitSystem::setCameraTraitAddCbFnPtr(nullptr);                                                                     \
    PrototypeTraitSystem::setTransformTraitAddCbFnPtr(nullptr);                                                                  \
    PrototypeTraitSystem::setMeshRendererTraitAddCbFnPtr(nullptr);                                                               \
    PrototypeTraitSystem::setRigidbodyTraitAddCbFnPtr(nullptr);                                                                  \
    PrototypeTraitSystem::setScriptTraitAddCbFnPtr(nullptr);                                                                     \
    PrototypeTraitSystem::setColliderTraitAddCbFnPtr(nullptr);                                                                   \
    PrototypeTraitSystem::setVehicleChasisTraitAddCbFnPtr(nullptr);                                                              \
                                                                                                                                 \
    PrototypeTraitSystem::setCameraTraitReuseCbFnPtr(nullptr);                                                                   \
    PrototypeTraitSystem::setTransformTraitReuseCbFnPtr(nullptr);                                                                \
    PrototypeTraitSystem::setMeshRendererTraitReuseCbFnPtr(nullptr);                                                             \
    PrototypeTraitSystem::setRigidbodyTraitReuseCbFnPtr(nullptr);                                                                \
    PrototypeTraitSystem::setScriptTraitReuseCbFnPtr(nullptr);                                                                   \
    PrototypeTraitSystem::setColliderTraitReuseCbFnPtr(nullptr);                                                                 \
    PrototypeTraitSystem::setVehicleChasisTraitReuseCbFnPtr(nullptr);                                                            \
                                                                                                                                 \
    PrototypeTraitSystem::setCameraTraitRemoveCbFnPtr(nullptr);                                                                  \
    PrototypeTraitSystem::setTransformTraitRemoveCbFnPtr(nullptr);                                                               \
    PrototypeTraitSystem::setMeshRendererTraitRemoveCbFnPtr(nullptr);                                                            \
    PrototypeTraitSystem::setRigidbodyTraitRemoveCbFnPtr(nullptr);                                                               \
    PrototypeTraitSystem::setScriptTraitRemoveCbFnPtr(nullptr);                                                                  \
    PrototypeTraitSystem::setColliderTraitRemoveCbFnPtr(nullptr);                                                                \
    PrototypeTraitSystem::setVehicleChasisTraitRemoveCbFnPtr(nullptr);                                                           \
                                                                                                                                 \
    PrototypeTraitSystem::setCameraTraitLogCbFnPtr(nullptr);                                                                     \
    PrototypeTraitSystem::setTransformTraitLogCbFnPtr(nullptr);                                                                  \
    PrototypeTraitSystem::setMeshRendererTraitLogCbFnPtr(nullptr);                                                               \
    PrototypeTraitSystem::setRigidbodyTraitLogCbFnPtr(nullptr);                                                                  \
    PrototypeTraitSystem::setScriptTraitLogCbFnPtr(nullptr);                                                                     \
    PrototypeTraitSystem::setColliderTraitLogCbFnPtr(nullptr);                                                                   \
    PrototypeTraitSystem::setVehicleChasisTraitLogCbFnPtr(nullptr);

PROTOTYPE_EXTERN PROTOTYPE_ENGINE_API bool
PrototypeEngineInit(const PrototypeEngineApplication& application)
{
    PrototypeStaticInitializer::reset();

    PrototypeLogger::setData(PROTOTYPE_NEW PrototypeLoggerData);

    PrototypeTraitSystemInit();
    PrototypeEngineInternalApplication::traitSystemData = PrototypeTraitSystemGetData();

    PrototypeEngineInternalApplication::database = PROTOTYPE_NEW PrototypeDatabase();
    PrototypeEngineInternalApplication::database->allocate();

    PROTOTYOE_TRAIT_SYSTEM_SET_CALLBACKS()

#if defined(PROTOTYPE_ENABLE_PROFILER)
    PrototypeEngineInternalApplication::profiler = PROTOTYPE_NEW PrototypeProfiler();
#endif

    PrototypeEngineInternalApplication::application = application;
    PrototypeEngineInternalApplication::shouldQuit  = false;

    std::string SettingsSceneName    = "";
    std::string SettingsRenderingApi = "";
    std::string SettingsPhysicsApi   = "";

    // load settings
    {
        std::ifstream settingsFstream(PROTOTYPE_SCENE_PATH("Settings.json"));
        if (!settingsFstream.is_open()) {
            PrototypeLogger::warn("Couldn't load scene settings from <%s>", PROTOTYPE_SCENE_PATH("Settings.json"));
            return false;
        }
        nlohmann::json j;
        settingsFstream >> j;
        settingsFstream.close();

        if (j.is_null()) { return false; }

        const char* field_default_scene         = "DefaultScene";
        const char* field_default_rendering_api = "DefaultRenderingApi";
        const char* field_default_physics_api   = "DefaultPhysicsApi";
        const char* field_resources             = "Resources";
        const char* field_scenes                = "Scenes";

        if (!j.contains(field_default_scene)) {
            PrototypeLogger::warn("Settings doesn't have a default scene field \"%s\"", field_default_scene);
            return false;
        }

        if (!j.contains(field_default_rendering_api)) {
            PrototypeLogger::warn("Settings doesn't have a default rendering api field \"%s\"", field_default_rendering_api);
            return false;
        }

        if (!j.contains(field_default_physics_api)) {
            PrototypeLogger::warn("Settings doesn't have a default physics api field \"%s\"", field_default_physics_api);
            return false;
        }

        if (!j.contains(field_resources)) {
            PrototypeLogger::warn("Settings doesn't have a resources field \"%s\"", field_resources);
            return false;
        }

        if (!j.contains(field_scenes)) {
            PrototypeLogger::warn("Settings doesn't have scenes field \"%s\"", field_scenes);
            return false;
        }

        std::string defaultSceneName    = j.at(field_default_scene).get<std::string>();
        std::string defaultRenderingApi = j.at(field_default_rendering_api).get<std::string>();
        std::string defaultPhysicsApi   = j.at(field_default_physics_api).get<std::string>();
        std::string resourcesFilename   = "";

        // Pick a rendering api
        {
            if (PROTOTYPE_STRINGIFY(PrototypeEngineERenderingApi_) + defaultRenderingApi ==
                PrototypeEngineERenderingApi_OPENGL4_1_Str) {
                PrototypeEngineInternalApplication::renderingApi = PrototypeEngineERenderingApi_OPENGL4_1;
                resourcesFilename                                = "OPENGL4_1";
            } else if (PROTOTYPE_STRINGIFY(PrototypeEngineERenderingApi_) + defaultRenderingApi ==
                       PrototypeEngineERenderingApi_OPENGLES_3_0_Str) {
                PrototypeEngineInternalApplication::renderingApi = PrototypeEngineERenderingApi_OPENGLES_3_0;
                resourcesFilename                                = "OPENGLES_3_0";
            } else if (PROTOTYPE_STRINGIFY(PrototypeEngineERenderingApi_) + defaultRenderingApi ==
                       PrototypeEngineERenderingApi_VULKAN_1_Str) {
                PrototypeEngineInternalApplication::renderingApi = PrototypeEngineERenderingApi_VULKAN_1;
                resourcesFilename                                = "VULKAN_1";
            }
        }
        // Pick a physics api
        {
            if (PROTOTYPE_STRINGIFY(PrototypeEngineEPhysicsApi_) + defaultPhysicsApi == PrototypeEngineEPhysicsApi_PHYSX_Str) {
                PrototypeEngineInternalApplication::physicsApi = PrototypeEngineEPhysicsApi_PHYSX;
            } else if (PROTOTYPE_STRINGIFY(PrototypeEngineEPhysicsApi_) + defaultPhysicsApi ==
                       PrototypeEngineEPhysicsApi_BULLET_Str) {
                PrototypeEngineInternalApplication::physicsApi = PrototypeEngineEPhysicsApi_BULLET;
            }
        }

        // load all resources first
        const auto&        jresources               = j.at(field_resources);
        const std::string& apiSpecificResourcesFile = jresources.at(resourcesFilename).get<std::string>();
        PrototypeSceneLoader::loadResourcesFromFile((PROTOTYPE_SCENE_PATH("") + apiSpecificResourcesFile).c_str());

        // load scenes
        for (const auto& scene : j.at(field_scenes)) {
            auto name                   = scene.at("name").get<std::string>();
            auto availableRenderingApis = scene.at("RenderingApis").get<std::unordered_set<std::string>>();
            auto availablePhysicsApis   = scene.at("PhysicsApis").get<std::unordered_set<std::string>>();

            if (availableRenderingApis.find(defaultRenderingApi) != availableRenderingApis.end() &&
                availablePhysicsApis.find(defaultPhysicsApi) != availablePhysicsApis.end()) {
                // load scene data from json
                PrototypeSceneLoader::loadPrototypeSceneFromFile((PROTOTYPE_SCENE_PATH("") + name).c_str());
            }
        }

        auto it = PrototypeEngineInternalApplication::database->scenes.find(defaultSceneName);
        if (it != PrototypeEngineInternalApplication::database->scenes.end()) {
            // reference default scene
            PrototypeEngineInternalApplication::scene = it->second;
        }

        // try to load first available scene in case default scene was not found
        if (PrototypeEngineInternalApplication::scene == nullptr) {
            if (PrototypeEngineInternalApplication::database->scenes.empty()) {
                PrototypeLogger::fatal("No available scenes to load");
            } else {
                // set global scene pointer
                PrototypeEngineInternalApplication::scene = PrototypeEngineInternalApplication::database->scenes.begin()->second;
            }
        }
    }

    // dump scene to logs
    // PrototypeEngineInternalApplication::database->dump(PrototypeEngineInternalApplication::scene);

    // initialize physics api
    switch (PrototypeEngineInternalApplication::physicsApi) {
        case PrototypeEngineEPhysicsApi_PHYSX: {
            PrototypeEngineInternalApplication::physics = PROTOTYPE_NEW PrototypePhysxPhysics();
        } break;
        case PrototypeEngineEPhysicsApi_BULLET: {
            PrototypeEngineInternalApplication::physics = PROTOTYPE_NEW PrototypeBulletPhysics();
        } break;
        case PrototypeEngineEPhysicsApi_COUNT: {
            PrototypeLogger::fatal("Unimplemented!(Unreachable)");
        } break;
    }

    // create window according to the used rendering api
    switch (PrototypeEngineInternalApplication::renderingApi) {
        case PrototypeEngineERenderingApi_OPENGL4_1: {
            PrototypeEngineInternalApplication::window = PROTOTYPE_NEW PrototypeOpenglWindow();
        } break;
        case PrototypeEngineERenderingApi_VULKAN_1: {
            PrototypeEngineInternalApplication::window = PROTOTYPE_NEW PrototypeVulkanWindow();
        } break;
        default: {
            PrototypeLogger::fatal("renderingApi: Unimplemented!(Unreachable)");
        } break;
    }

    if (!PrototypeEngineInternalApplication::window->init(1250, 800)) { return false; }
    if (!PrototypeEngineInternalApplication::physics->init()) { return false; }
    if (!PrototypeEngineInternalApplication::renderer->init()) { return false; }

    return true;
}

static void
mainLoopProcedureBlock()
{
    static auto t1       = std::chrono::high_resolution_clock::now();
    static auto t2       = std::chrono::high_resolution_clock::now();
    static auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    if (PrototypeEngineInternalApplication::window->isIconified()) {
        if (PrototypeEngineInternalApplication::physics) { PrototypeEngineInternalApplication::physics->pause(); }
        while (PrototypeEngineInternalApplication::window->isIconified()) { glfwPollEvents(); }
        if (PrototypeEngineInternalApplication::physics) { PrototypeEngineInternalApplication::physics->play(); }
    }

#if defined(PROTOTYPE_ENABLE_PROFILER)
    PrototypeEngineInternalApplication::profiler->advanceTimeline();
#endif
    for (auto& command : PrototypePipelines::shortcutsQueue) { command.dispatch(); }
    PrototypePipelines::shortcutsQueue.clear();
#if defined(PROTOTYPE_ENGINE_DEVELOPMENT_MODE)
    PrototypeEngineInternalApplication::renderer->ui()->beginRecordPass();
    PrototypeEngineInternalApplication::renderer->ui()->endRecordPass();
#endif
    PrototypeEngineInternalApplication::renderer->beginRecordPass();
    PrototypeEngineInternalApplication::renderer->endRecordPass();
    PrototypeEngineInternalApplication::physics->beginRecordPass();
    PrototypeEngineInternalApplication::physics->endRecordPass();
    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (PrototypeObject* scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        for (const auto& codeLinkPair : script->codeLinks) {
            PrototypePluginInstance::safeCallUpdateProtocol(&codeLinkPair.second, scriptableObject);
        }
    }

    PrototypeEngineInternalApplication::physics->update();
    PrototypeEngineInternalApplication::renderer->update();
    PrototypeEngineInternalApplication::renderer->render3D();
    PrototypeEngineInternalApplication::renderer->render2D();

    if (PrototypeEngineInternalApplication::window->needsReload()) {}
    if (PrototypeEngineInternalApplication::window->needsInspector()) {
        PrototypeEngineInternalApplication::database->dump(PrototypeEngineInternalApplication::scene);
        PrototypeEngineInternalApplication::window->consumeNeedsInspector();
    }
    PrototypeEngineInternalApplication::shouldQuit = PrototypeEngineInternalApplication::window->update();
}

PROTOTYPE_EXTERN PROTOTYPE_ENGINE_API void
PrototypeEngineLoop()
{
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
#if defined(PROTOTYPE_ENGINE_DEVELOPMENT_MODE)
    PrototypeEngineInternalApplication::renderer->ui()->scheduleRecordPass(PrototypeUiViewMaskAll);
#endif

    PrototypeEngineInternalApplication::shouldQuit = PrototypeEngineInternalApplication::window->update();

    std::vector<std::string> pluginFiles = PrototypeIo::listFiles(PROTOTYPE_PLUGIN_PATH(""), 5);
    for (const auto& pluginFile : pluginFiles) {
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
        const char* stem = ".dll";
#else
        const char* stem = ".so";
#endif
        std::filesystem::path p(pluginFile);
        if (p.extension().string() != stem) { continue; }
        PrototypePluginInstance* plugin = PrototypeEngineInternalApplication::database->allocatePluginInstance(pluginFile);
        auto                     valid  = plugin->load();
        if (valid) {
            PrototypeEngineInternalApplication::database->pluginInstances.insert({ plugin->filepath(), plugin });
        } else {
            PrototypeEngineInternalApplication::database->deallocatePluginInstance(plugin);
        }
    }

    PrototypeEngineInternalApplication::database->watchFs();

    for (auto& pair : PrototypeEngineInternalApplication::database->textureBuffers) { pair.second->unsetData(); }
    for (auto& pair : PrototypeEngineInternalApplication::database->shaderBuffers) { pair.second->unsetData(); }

    while (!PrototypeEngineInternalApplication::shouldQuit) { mainLoopProcedureBlock(); }

    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (const auto& scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        for (const auto& codeLinkPair : script->codeLinks) {
            PrototypePluginInstance::safeCallEndProtocol(&codeLinkPair.second, scriptableObject);
        }
    }

    for (const auto& pair : PrototypeEngineInternalApplication::database->pluginInstances) {
        PrototypePluginInstance* pluginInstance = pair.second;
        pluginInstance->unload();
    }

    for (const auto& pair : PrototypeEngineInternalApplication::database->pluginInstances) {
        PrototypePluginInstance* pluginInstance = pair.second;
        std::filesystem::remove(std::filesystem::path(pluginInstance->name()));
    }
}

PROTOTYPE_EXTERN PROTOTYPE_ENGINE_API void
PrototypeEngineDeInit()
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    delete PrototypeEngineInternalApplication::profiler;
#endif

    PrototypeEngineInternalApplication::renderer->deInit();
    PrototypeEngineInternalApplication::physics->deInit();
    delete PrototypeEngineInternalApplication::physics;
    PrototypeEngineInternalApplication::window->deInit();
    delete PrototypeEngineInternalApplication::window;

    PROTOTYOE_TRAIT_SYSTEM_UNSET_CALLBACKS()
    PrototypeTraitSystem::clearObjects();
    PrototypeTraitSystemDeinit();

    PrototypeEngineInternalApplication::database->deallocate();
    delete PrototypeEngineInternalApplication::database;

    delete PrototypeLogger::data();
}