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

#include "PrototypeScene.h"
#include "PrototypeDatabase.h"
#include "PrototypeEngine.h"
#include "PrototypeFrameBuffer.h"
#include "PrototypeMaterial.h"
#include "PrototypeMeshBuffer.h"
#include "PrototypeSceneLayer.h"
#include "PrototypeSceneLoader.h"
#include "PrototypeSceneNode.h"
#include "PrototypeShaderBuffer.h"
#include "PrototypeStaticInitializer.h"
#include "PrototypeTextureBuffer.h"
#include "PrototypeUI.h"

#include "PrototypeEngine.h"
#include "PrototypePreloadedAssets.h"
#include "PrototypeRenderer.h"

#include <PrototypeCommon/Definitions.h>
#include <PrototypeCommon/IO.h>
#include <PrototypeCommon/Logger.h>

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include <algorithm>
#include <filesystem>
#include <thread>

PrototypeScene::PrototypeScene(const std::string name)
  : _id(++PrototypeStaticInitializer::_sceneUUID)
  , _name(name)
{}

PrototypeScene::~PrototypeScene()
{
    for (auto pair : _layers) { pair.second->notifyRemoved(); }
    _layers.clear();
    _remap_layers.clear();
}

const std::unordered_set<PrototypeObject*>&
PrototypeScene::fetchObjectsByTraits(MASK_TYPE traitMask) const
{
    auto it = _nodeFilters.find(traitMask);
    if (it != _nodeFilters.end()) { return it->second->data(); }
    // TODO:
    // I can also handle the case when we request out of subscription certificates
    // I need to dispatch subscription request on the fly in that case ..
    PrototypeLogger::fatal("Unreachable, you have requested an undocumented access to a set of traits");
    static const std::unordered_set<PrototypeObject*> dummy;
    return dummy;
    // std::map<NodeObjectPair, std::unordered_set<std::string>> objectsMap;
    // for (auto traitName : traitNames) {
    //     auto it = _filteredObjects.find(traitName);
    //     if (it != _filteredObjects.end()) {
    //         for (auto object : it->second) {
    //             if (objectsMap.count(object) == 0) { objectsMap.insert({ object, {} }); }
    //             objectsMap[object].insert(traitName);
    //         }
    //     }
    // }
    // std::vector<NodeObjectPair> objects;
    // for (auto pair : objectsMap) {
    //     if (pair.second == traitNames) { objects.push_back(pair.first); }
    // }
    // return objects;
}

void
PrototypeScene::subscribeNodeFilters(MASK_TYPE traitMask)
{
    if (_nodeFilters.find(traitMask) == _nodeFilters.end()) {
        _nodeFilters.insert({ traitMask, PrototypeEngineInternalApplication::database->allocateSceneFilter(traitMask) });
    }
}

void
PrototypeScene::unsubscribeNodeFilters(MASK_TYPE traitMask)
{
    if (_nodeFilters.find(traitMask) != _nodeFilters.end()) { _nodeFilters.erase(traitMask); }
}

bool
PrototypeScene::addLayer(const std::string name)
{
    auto it = _remap_layers.find(name);
    if (it == _remap_layers.end()) {
        auto layer = PrototypeEngineInternalApplication::database->allocateSceneLayer(name);
        layer->setParentScene(this);
        onAddLayer(layer);
        _layers.insert({ layer->id(), layer });
        _remap_layers.insert({ name, layer->id() });
        return true;
    }
    return false;
}

bool
PrototypeScene::addLayer(PrototypeSceneLayer* layer)
{
    auto it = _remap_layers.find(layer->name());
    if (it == _remap_layers.end()) {
        layer->setParentScene(this);
        onAddLayer(layer);
        _layers.insert({ layer->id(), layer });
        _remap_layers.insert({ layer->name(), layer->id() });
        return true;
    }
    return false;
}

bool
PrototypeScene::removeLayerById(const u32 id)
{
    auto it = _layers.find(id);
    if (it != _layers.end()) {
        std::string name = it->second->name();
        onRemoveLayer(it->second);
        it->second->notifyRemoved();
        PrototypeEngineInternalApplication::database->deallocateSceneLayer(it->second);
        _remap_layers.erase(name);
        _layers.erase(id);
        return true;
    }
    return false;
}

bool
PrototypeScene::removeLayerByName(const std::string name)
{
    auto it = _remap_layers.find(name);
    if (it != _remap_layers.end()) {
        auto it2 = _layers.find(it->second);
        onRemoveLayer(it2->second);
        it2->second->notifyRemoved();
        PrototypeEngineInternalApplication::database->deallocateSceneLayer(it2->second);
        _layers.erase(it->second);
        _remap_layers.erase(it->first);
        return true;
    }
    return false;
}

void
PrototypeScene::addSelectedNode(PrototypeSceneNode* node)
{
    if (_selectedNodes.find(node) == _selectedNodes.end()) { _selectedNodes.insert(node); }
}

void
PrototypeScene::removeSelectedNode(PrototypeSceneNode* node)
{
    auto it = _selectedNodes.find(node);
    if (it != _selectedNodes.end()) { _selectedNodes.erase(it); }
}

void
PrototypeScene::clearSelectedNodes()
{
    while (!_selectedNodes.empty()) {
        if (*_selectedNodes.begin()) { (*_selectedNodes.begin())->deselect(); }
    }
    _selectedNodes.clear();
}

void
PrototypeScene::addNodeToTraitFilters(PrototypeSceneNode* node, u64 traitMask)
{
    if (!PrototypeEngineInternalApplication::scene) return;
    for (auto& pair : _nodeFilters) { pair.second->onAddSceneNodeTraits(node, traitMask); }
}

void
PrototypeScene::removeNodeFromTraitFilters(PrototypeSceneNode* node, u64 traitMask)
{
    if (!PrototypeEngineInternalApplication::scene) return;
    for (auto& pair : _nodeFilters) { pair.second->onRemoveSceneNodeTraits(node, traitMask); }
}

void
PrototypeScene::onAddLayer(PrototypeSceneLayer* layer)
{
    for (auto pair2 : layer->nodes()) {
        auto node = pair2.second;
        onAddNode(node);
    }
}

void
PrototypeScene::onRemoveLayer(PrototypeSceneLayer* layer)
{
    for (auto pair2 : layer->nodes()) {
        auto node = pair2.second;
        onRemoveNode(node);
    }
}

void
PrototypeScene::onAddNode(PrototypeSceneNode* node)
{
    auto optObject = node->object();
    if (optObject.has_value()) {
        auto obj = optObject.value();
        for (auto& pair : _nodeFilters) { pair.second->onAddSceneNode(node); }
    }
}

void
PrototypeScene::onRemoveNode(PrototypeSceneNode* node, bool dispatchRecordingPass)
{
    for (const auto& childNodePair : node->nodes()) {
        for (auto& pair : _nodeFilters) { pair.second->onRemoveSceneNode(childNodePair.second); }
        onRemoveNode(childNodePair.second, false);
    }

    auto optObject = node->object();
    if (optObject.has_value()) {
        auto obj = optObject.value();
        for (auto& pair : _nodeFilters) { pair.second->onRemoveSceneNode(node); }
    }
    if (dispatchRecordingPass) {
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
#if defined(PROTOTYPE_ENGINE_DEVELOPMENT_MODE)
        PrototypeEngineInternalApplication::renderer->ui()->scheduleRecordPass(PrototypeUiViewMaskHierarchy);
#endif
    }
}

void
PrototypeScene::onSelectNode(PrototypeSceneNode* node)
{
    addSelectedNode(node);
}

void
PrototypeScene::onDeselectNode(PrototypeSceneNode* node)
{
    removeSelectedNode(node);
}

const u32&
PrototypeScene::id() const
{
    return _id;
}

const std::string&
PrototypeScene::name() const
{
    return _name;
}

const std::optional<PrototypeSceneLayer*>
PrototypeScene::layerById(const u32 id) const
{
    auto it = _layers.find(id);
    if (it != _layers.end()) { return { it->second }; }
    return {};
}

const std::optional<PrototypeSceneLayer*>
PrototypeScene::layerByName(const std::string name) const
{
    auto it = _remap_layers.find(name);
    if (it != _remap_layers.end()) {
        auto it2 = _layers.find(it->second);
        if (it2 != _layers.end()) { return { it2->second }; }
    }
    return {};
}

const std::unordered_map<u32, PrototypeSceneLayer*>&
PrototypeScene::layers() const
{
    return _layers;
}

const std::unordered_set<PrototypeSceneNode*>&
PrototypeScene::selectedNodes() const
{
    return _selectedNodes;
}

void
PrototypeScene::to_json(nlohmann::json& j, const PrototypeScene& scene)
{
    j["id"]   = scene.id();
    j["name"] = scene.name();
    std::vector<nlohmann::json> jlayers;
    for (auto pair : scene._layers) {
        nlohmann::json jlayer;
        PrototypeSceneLayer::to_json(jlayer, *pair.second);
        jlayers.push_back(jlayer);
    }
    j["layers"] = jlayers;
}

std::optional<PrototypeScene*>
PrototypeScene::from_json(const nlohmann::json& j)
{
    if (j.is_null()) { return {}; }

    const char* field_name    = "name";
    const char* field_layers  = "layers";
    const char* field_filters = "filters";
    const char* field_bundles = "bundles";

    if (!j.contains(field_name)) {
        PrototypeLogger::warn("Scene doesn't have a \"%s\"", field_name);
        return {};
    }
    if (!j.contains(field_layers)) {
        PrototypeLogger::warn("Scene doesn't have \"%s\"", field_layers);
        return {};
    }
    if (!j.contains(field_filters)) {
        PrototypeLogger::warn("Scene doesn't have \"%s\"", field_filters);
        return {};
    }
    if (!j.contains(field_bundles)) {
        PrototypeLogger::warn("Scene doesn't have \"%s\"", field_bundles);
        return {};
    }

    const std::string sceneName = j.at(field_name).get<std::string>();

    auto scene = PrototypeEngineInternalApplication::database->allocateScene(sceneName);

    for (auto jfilter : j.at(field_filters)) {
        MASK_TYPE traitMask = 0;
        auto      traits    = jfilter.at("traits").get<std::vector<std::string>>();
        for (const auto& traitName : traits) {
            auto it = PrototypeTraitTypeMaskFromAbsoluteString.find(traitName);
            if (it != PrototypeTraitTypeMaskFromAbsoluteString.end()) { traitMask |= it->second; }
        }
        scene->subscribeNodeFilters(traitMask);
    }

    // auto blueprintsLayer = PrototypeEngineInternalApplication::database->allocateSceneLayer("blueprints");
    // blueprintsLayer->setParentScene(scene);
    // if (scene->addLayer(blueprintsLayer)) {
    //     PrototypeEngineInternalApplication::database->sceneLayers[scene].insert({ blueprintsLayer->name(), blueprintsLayer });
    // } else {
    //     PrototypeEngineInternalApplication::database->deallocateSceneLayer(blueprintsLayer);
    // }

    for (auto jlayer : j.at(field_layers)) {
        if (jlayer.is_null()) continue;
        auto optLayer = PrototypeSceneLayer::from_json(jlayer, scene);
        if (optLayer.has_value()) {
            auto layer = optLayer.value();
            layer->setParentScene(scene);
            if (scene->addLayer(layer)) {
                PrototypeEngineInternalApplication::database->sceneLayers[scene].insert({ layer->name(), layer });
            } else {
                PrototypeEngineInternalApplication::database->deallocateSceneLayer(layer);
            }
        }
    }

    for (auto jbundle : j.at(field_bundles)) {
        if (jbundle.is_null()) continue;
        std::string  filename = jbundle["path"].get<std::string>();
        BundleConfig config   = BundleConfig();
        config.filepath       = filename;
        config.colliders      = "";
        config.isStatic       = true;
        config.loaded         = false;
        if (jbundle.find("loaded") != jbundle.end()) {
            if (!jbundle["loaded"].is_null()) {
                config.loaded = true;
                if (jbundle["loaded"].contains("colliders")) {
                    config.colliders = jbundle["loaded"]["colliders"].get<std::string>();
                }
                if (jbundle["loaded"].contains("isStatic")) { config.isStatic = jbundle["loaded"]["isStatic"].get<bool>(); }
            } else {
                config.loaded = false;
            }
        } else {
            config.loaded = false;
        }

        std::filesystem::path p(config.filepath);

        if (p.extension().string() == ".gltf" || p.extension().string() == ".glb") {
            PrototypeSceneLoader::tinygltfImportScene(config, scene);
        } else {
            PrototypeSceneLoader::assimpImportScene(config, scene);
        }
    }

    return { scene };
}