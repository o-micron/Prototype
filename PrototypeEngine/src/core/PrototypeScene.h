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

#include "PrototypeSceneFilter.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <nlohmann/json.hpp>

struct PrototypeSceneNode;
struct PrototypeSceneLayer;
struct PrototypeObject;

struct PrototypeScene
{
    PrototypeScene(const std::string name);
    ~PrototypeScene();

    const std::unordered_set<PrototypeObject*>& fetchObjectsByTraits(MASK_TYPE traitMask) const;

    bool addLayer(const std::string name);
    bool addLayer(PrototypeSceneLayer* layer);
    bool removeLayerById(const u32 id);
    bool removeLayerByName(const std::string name);

    void addSelectedNode(PrototypeSceneNode* node);
    void removeSelectedNode(PrototypeSceneNode* node);
    void clearSelectedNodes();

    void addNodeToTraitFilters(PrototypeSceneNode* node, u64 traitMask);
    void removeNodeFromTraitFilters(PrototypeSceneNode* node, u64 traitMask);

    const u32&                                           id() const;
    const std::string&                                   name() const;
    const std::optional<PrototypeSceneLayer*>            layerById(const u32 id) const;
    const std::optional<PrototypeSceneLayer*>            layerByName(const std::string name) const;
    const std::unordered_map<u32, PrototypeSceneLayer*>& layers() const;
    const std::unordered_set<PrototypeSceneNode*>&       selectedNodes() const;

    static void                           to_json(nlohmann::json& j, const PrototypeScene& scene);
    static std::optional<PrototypeScene*> from_json(const nlohmann::json& j);

  private:
    friend struct PrototypeSceneLayer;
    void subscribeNodeFilters(MASK_TYPE traitMask);
    void unsubscribeNodeFilters(MASK_TYPE traitMask);
    void onAddLayer(PrototypeSceneLayer* layer);
    void onRemoveLayer(PrototypeSceneLayer* layer);
    void onAddNode(PrototypeSceneNode* node);
    void onRemoveNode(PrototypeSceneNode* node, bool dispatchRecordingPass = true);
    void onSelectNode(PrototypeSceneNode* node);
    void onDeselectNode(PrototypeSceneNode* node);

    void loadGltfAscii(std::string path);
    void loadGltfBinary(std::string path);

    const u32                                            _id;
    std::string                                          _name;
    std::unordered_map<u32, PrototypeSceneLayer*>        _layers;
    std::unordered_map<std::string, u32>                 _remap_layers;
    std::unordered_map<MASK_TYPE, PrototypeSceneFilter*> _nodeFilters;
    std::unordered_set<PrototypeSceneNode*>              _selectedNodes;
};
