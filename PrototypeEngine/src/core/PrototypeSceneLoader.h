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

#include <nlohmann/json.hpp>

struct PrototypeScene;
struct PrototypeSceneLayer;
struct PrototypeSceneNode;

struct BundleConfig
{
    BundleConfig()
      : filepath("")
      , colliders("")
      , loaded(false)
      , isStatic(true)
    {}
    std::string filepath;
    std::string colliders;
    bool        loaded;
    bool        isStatic;
};

struct PrototypeSceneLoader
{
    PrototypeSceneLoader()  = delete;
    ~PrototypeSceneLoader() = delete;

    static void loadResourcesFromFile(const char* filepath);
    static void loadPrototypeSceneFromFile(const char* filepath);
    static void tinygltfImportScene(BundleConfig config, PrototypeScene* scene);
    static void assimpImportScene(BundleConfig config, PrototypeScene* scene);

  private:
    // static void loadScene(const nlohmann::json& j);
    // static void loadSceneLayer(const nlohmann::json& j, PrototypeScene* scene);
    // static void loadSceneNode(const nlohmann::json& j, PrototypeSceneLayer* layer, PrototypeSceneNode* node);
    // static void loadSceneNodeComponents(const nlohmann::json& j, PrototypeSceneNode* node);
    // static void loadMeshes(const nlohmann::json& j);
    // static void loadShaders(const nlohmann::json& j);
    // static void loadTextures(const nlohmann::json& j);
    // static void loadMaterials(const nlohmann::json& j);
};
