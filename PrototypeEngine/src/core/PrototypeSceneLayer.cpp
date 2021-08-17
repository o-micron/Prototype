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

#include "PrototypeSceneLayer.h"
#include "PrototypeDatabase.h"
#include "PrototypeEngine.h"
#include "PrototypeRenderer.h"
#include "PrototypeScene.h"
#include "PrototypeSceneLayer.h"
#include "PrototypeSceneNode.h"
#include "PrototypeStaticInitializer.h"
#include "PrototypeUI.h"

#include <PrototypeCommon/Definitions.h>
#include <PrototypeCommon/Logger.h>

PrototypeSceneLayer::PrototypeSceneLayer(const std::string name)
  : _id(++PrototypeStaticInitializer::_sceneLayerUUID)
  , _mask(PrototypeSceneMaskVisible | PrototypeSceneMaskMatching | PrototypeSceneMaskPartialMatching)
  , _name(name)
  , _parentScene(nullptr)
{}

PrototypeSceneLayer::~PrototypeSceneLayer()
{
    for (auto pair : _nodes) { pair.second->notifyRemoved(); }
    _nodes.clear();
    _remap_nodes.clear();
}

void
PrototypeSceneLayer::setParentScene(PrototypeScene* parentScene)
{
    _parentScene = parentScene;
}

void
PrototypeSceneLayer::notifyRemoved()
{
    for (auto pair : _nodes) { pair.second->notifyRemoved(); }
    _nodes.clear();
    _remap_nodes.clear();
}

bool
PrototypeSceneLayer::addNode(const std::string name)
{
    auto it = _remap_nodes.find(name);
    if (it == _remap_nodes.end()) {
        auto node = PrototypeEngineInternalApplication::database->allocateSceneNode(name);
        onAddNode(node);
        _nodes.insert({ node->id(), node });
        _remap_nodes.insert({ name, node->id() });
        return true;
    }
    return false;
}

bool
PrototypeSceneLayer::addNode(PrototypeSceneNode* node)
{
    auto it = _remap_nodes.find(node->name());
    if (it == _remap_nodes.end()) {
        onAddNode(node);
        _nodes.insert({ node->id(), node });
        _remap_nodes.insert({ node->name(), node->id() });
        return true;
    }
    return false;
}

bool
PrototypeSceneLayer::moveNode(PrototypeSceneNode* node, PrototypeSceneNode* newParent)
{
    if (newParent->hasNode(node) || node->hasNode(newParent)) { return false; }
    std::string name = node->name();
    const u32   id   = node->id();
    auto        it   = _nodes.find(node->id());
    if (it != _nodes.end()) {
        if (onMoveNode(node, newParent, this)) {
            _remap_nodes.erase(name);
            _nodes.erase(id);
            return true;
        }
    }
    return false;
}

bool
PrototypeSceneLayer::moveNode(PrototypeSceneNode* node, PrototypeSceneLayer* newParent)
{
    std::string name = node->name();
    const u32   id   = node->id();
    auto        it   = _nodes.find(node->id());
    if (it != _nodes.end()) {
        if (onMoveNode(node, newParent, this)) {
            _remap_nodes.erase(name);
            _nodes.erase(id);
            return true;
        }
    }
    return false;
}

void
PrototypeSceneLayer::removeNodeById(const u32 id)
{
    auto it = _nodes.find(id);
    if (it != _nodes.end()) {
        std::string name = it->second->name();
        onRemoveNode(it->second);
        it->second->notifyRemoved();
        PrototypeEngineInternalApplication::database->deallocateSceneNode(it->second);
        _remap_nodes.erase(name);
        _nodes.erase(id);
    }
}

void
PrototypeSceneLayer::removeNodeByName(const std::string name)
{
    auto it = _remap_nodes.find(name);
    if (it != _remap_nodes.end()) {
        auto it2 = _nodes.find(it->second);
        onRemoveNode(it2->second);
        it2->second->notifyRemoved();
        PrototypeEngineInternalApplication::database->deallocateSceneNode(it2->second);
        _nodes.erase(it->second);
        _remap_nodes.erase(it->first);
    }
}

void
PrototypeSceneLayer::show()
{
    _mask |= PrototypeSceneMaskVisible;
}

void
PrototypeSceneLayer::hide()
{
    _mask &= ~PrototypeSceneMaskVisible;
}

void
PrototypeSceneLayer::setHovering()
{
    _mask |= PrototypeSceneMaskHovered;
}

void
PrototypeSceneLayer::unsetHovering()
{
    _mask &= ~PrototypeSceneMaskHovered;
}

void
PrototypeSceneLayer::select()
{
    _mask |= PrototypeSceneMaskSelected;
}

void
PrototypeSceneLayer::deselect()
{
    _mask &= ~PrototypeSceneMaskSelected;
}

void
PrototypeSceneLayer::setMatched()
{
    _mask |= PrototypeSceneMaskMatching;
}

void
PrototypeSceneLayer::unsetMatched()
{
    _mask &= ~PrototypeSceneMaskMatching;
}

void
PrototypeSceneLayer::setPartialMatched()
{
    _mask |= PrototypeSceneMaskPartialMatching;
}

void
PrototypeSceneLayer::unsetPartialMatched()
{
    _mask &= ~PrototypeSceneMaskPartialMatching;
}

void
PrototypeSceneLayer::onAddNode(PrototypeSceneNode* node)
{
    if (_parentScene) { _parentScene->onAddNode(node); }
}

void
PrototypeSceneLayer::onRemoveNode(PrototypeSceneNode* node)
{
    if (_parentScene) { _parentScene->onRemoveNode(node); }
}

bool
PrototypeSceneLayer::onMoveNode(PrototypeSceneNode* node, PrototypeSceneNode* newParent, PrototypeSceneLayer* oldParent)
{
    if (newParent->nodesByName(node->name()).has_value()) return false;
    node->setParentLayer(nullptr);
    node->setParentNode(newParent);
    newParent->addNode(node);
    return true;
}

bool
PrototypeSceneLayer::onMoveNode(PrototypeSceneNode* node, PrototypeSceneLayer* newParent, PrototypeSceneNode* oldParent)
{
    if (newParent->nodesByName(node->name()).has_value()) return false;
    node->setParentLayer(newParent);
    node->setParentNode(nullptr);
    newParent->addNode(node);
    if (oldParent->absoluteLayer() != newParent) {
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
#if defined(PROTOTYPE_ENGINE_DEVELOPMENT_MODE)
        PrototypeEngineInternalApplication::renderer->ui()->scheduleRecordPass(PrototypeUiViewMaskHierarchy);
#endif
    }
    return true;
}

bool
PrototypeSceneLayer::onMoveNode(PrototypeSceneNode* node, PrototypeSceneLayer* newParent, PrototypeSceneLayer* oldParent)
{
    if (newParent->nodesByName(node->name()).has_value()) return false;
    node->setParentLayer(newParent);
    node->setParentNode(nullptr);
    newParent->addNode(node);
    if (oldParent != newParent) {
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
#if defined(PROTOTYPE_ENGINE_DEVELOPMENT_MODE)
        PrototypeEngineInternalApplication::renderer->ui()->scheduleRecordPass(PrototypeUiViewMaskHierarchy);
#endif
    }
    return true;
}

bool
PrototypeSceneLayer::onRenameNode(PrototypeSceneNode* node, const std::string& newName)
{
    if (_remap_nodes.find(newName) != _remap_nodes.end()) return false;
    auto name             = node->name();
    auto id               = node->id();
    _remap_nodes[newName] = id;
    _remap_nodes.erase(name);
    return true;
}

void
PrototypeSceneLayer::onSelectNode(PrototypeSceneNode* node)
{
    if (_parentScene) { _parentScene->onSelectNode(node); }
}

void
PrototypeSceneLayer::onDeselectNode(PrototypeSceneNode* node)
{
    if (_parentScene) { _parentScene->onDeselectNode(node); }
}

const u32&
PrototypeSceneLayer::id() const
{
    return _id;
}

const bool
PrototypeSceneLayer::isVisible() const
{
    return (_mask & PrototypeSceneMaskVisible) == PrototypeSceneMaskVisible;
}

const bool
PrototypeSceneLayer::isHovered() const
{
    return (_mask & PrototypeSceneMaskHovered) == PrototypeSceneMaskHovered;
}

const bool
PrototypeSceneLayer::isSelected() const
{
    return (_mask & PrototypeSceneMaskSelected) == PrototypeSceneMaskSelected;
}

const bool
PrototypeSceneLayer::isMatching() const
{
    return (_mask & PrototypeSceneMaskMatching) == PrototypeSceneMaskMatching;
}

const bool
PrototypeSceneLayer::isPartialMatching() const
{
    return (_mask & PrototypeSceneMaskPartialMatching) == PrototypeSceneMaskPartialMatching;
}

const std::string&
PrototypeSceneLayer::name() const
{
    return _name;
}

const std::optional<PrototypeSceneNode*>
PrototypeSceneLayer::nodesById(const u32 id) const
{
    auto it = _nodes.find(id);
    if (it != _nodes.end()) { return { it->second }; }
    return {};
}

const std::optional<PrototypeSceneNode*>
PrototypeSceneLayer::nodesByName(const std::string name) const
{
    auto it = _remap_nodes.find(name);
    if (it != _remap_nodes.end()) {
        auto it2 = _nodes.find(it->second);
        if (it2 != _nodes.end()) { return { it2->second }; }
    }
    return {};
}

const std::unordered_map<u32, PrototypeSceneNode*>
PrototypeSceneLayer::nodes() const
{
    return _nodes;
}

PrototypeScene*
PrototypeSceneLayer::parentScene()
{
    return _parentScene;
}

void
PrototypeSceneLayer::to_json(nlohmann::json& j, const PrototypeSceneLayer& layer)
{
    j["id"]   = layer.id();
    j["name"] = layer.name();
    std::vector<nlohmann::json> jnodes;
    for (auto pair : layer._nodes) {
        nlohmann::json jnode;
        PrototypeSceneNode::to_json(jnode, *pair.second);
        jnodes.push_back(jnode);
    }
    j["nodes"] = jnodes;
}

std::optional<PrototypeSceneLayer*>
PrototypeSceneLayer::from_json(const nlohmann::json& j, PrototypeScene* scene)
{
    if (j.is_null()) { return {}; }

    const char* field_name  = "name";
    const char* field_nodes = "nodes";

    if (!j.contains(field_name)) { return {}; }

    const std::string layerName = j.at(field_name).get<std::string>();

    auto layer = PrototypeEngineInternalApplication::database->allocateSceneLayer(layerName);

    // nodes
    if (j.contains(field_nodes)) {
        for (auto jnode : j.at(field_nodes)) {
            if (jnode.is_null()) continue;
            auto optNode = PrototypeSceneNode::from_json(jnode, scene);
            if (optNode.has_value()) {
                auto node = optNode.value();
                node->setParentLayer(layer);
                node->setParentNode(nullptr);
                if (layer->addNode(node)) {
                    PrototypeEngineInternalApplication::database->sceneNodes[scene].insert({ node->name(), node });
                } else {
                    PrototypeEngineInternalApplication::database->deallocateSceneNode(node);
                }
            }
        }
    }

    return { layer };
}