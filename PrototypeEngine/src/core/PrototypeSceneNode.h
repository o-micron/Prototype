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

#include "PrototypeSceneMasks.h"

#include "../../include/PrototypeEngine/PrototypeEngineApi.h"

#include <PrototypeTraitSystem/PrototypeTraitSystemTypes.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <nlohmann/json.hpp>

struct PrototypeObject;
struct PrototypeScene;
struct PrototypeSceneLayer;

struct PrototypeSceneNode
{
    PrototypeSceneNode(const std::string name);
    ~PrototypeSceneNode();

    void setParentLayer(PrototypeSceneLayer* parentLayer);
    void setParentNode(PrototypeSceneNode* parentNode);

    bool hasNode(const std::string& name);
    bool hasNode(PrototypeSceneNode* node);
    bool addNode(const std::string name);
    bool addNode(PrototypeSceneNode* node);
    bool moveNode(PrototypeSceneNode* node, PrototypeSceneNode* newParent);
    bool moveNode(PrototypeSceneNode* node, PrototypeSceneLayer* newParent);
    void removeNodeById(const u32 id);
    void removeNodeByName(const std::string name);
    void renameNode(const std::string name);
    void show();
    void hide();
    void setHovering();
    void unsetHovering();
    void select();
    void deselect();
    void setMatched();
    void unsetMatched();
    void setPartialMatched();
    void unsetPartialMatched();

    const u32&                            id() const;
    const bool                            isVisible() const;
    const bool                            isHovered() const;
    const bool                            isSelected() const;
    const bool                            isMatching() const;
    const bool                            isPartialMatching() const;
    const std::string&                    name() const;
    const std::optional<PrototypeObject*> object() const;

    const std::optional<PrototypeSceneNode*>           nodesById(const u32 id) const;
    const std::optional<PrototypeSceneNode*>           nodesByName(const std::string name) const;
    const std::unordered_map<u32, PrototypeSceneNode*> nodes() const;
    PrototypeSceneLayer*                               parentLayer();
    PrototypeSceneNode*                                parentNode();
    PrototypeSceneLayer*                               absoluteLayer();

    static void                               to_json(nlohmann::json& j, const PrototypeSceneNode& node);
    static std::optional<PrototypeSceneNode*> from_json(const nlohmann::json& j, PrototypeScene* scene);

  private:
    void onAddNode(PrototypeSceneNode* node);
    bool onMoveNode(PrototypeSceneNode* node, PrototypeSceneNode* newParent, PrototypeSceneNode* oldParent);
    bool onMoveNode(PrototypeSceneNode* node, PrototypeSceneLayer* newParent, PrototypeSceneNode* oldParent);
    void onRemoveNode(PrototypeSceneNode* node);
    bool onRenameNode(PrototypeSceneNode* node, const std::string& newName);
    void onSelectNode(PrototypeSceneNode* node);
    void onDeselectNode(PrototypeSceneNode* node);

    friend struct PrototypeSceneLayer;
    friend PrototypeObject* shotcutCreateCloneObjectToLayer(const std::string&   nodeName,
                                                            MASK_TYPE            traitMask,
                                                            PrototypeSceneLayer* parentLayer);
    friend PrototypeObject* shotcutCreateCloneObjectToNode(const std::string&  nodeName,
                                                           MASK_TYPE           traitMask,
                                                           PrototypeSceneNode* parentNode);

    void allocateObject();
    void deallocateObject();
    void notifyRemoved();

    const u32                                    _id;
    PrototypeSceneMaskType                       _mask;
    std::string                                  _name;
    PrototypeObject*                             _object;
    std::unordered_map<u32, PrototypeSceneNode*> _nodes;
    std::unordered_map<std::string, u32>         _remap_nodes;
    PrototypeSceneLayer*                         _parentLayer;
    PrototypeSceneNode*                          _parentNode;
};
