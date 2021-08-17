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

#include "PrototypeSceneNode.h"
#include "PrototypeDatabase.h"
#include "PrototypeEngine.h"
#include "PrototypeScene.h"
#include "PrototypeSceneLayer.h"
#include "PrototypeStaticInitializer.h"
#include <PrototypeCommon/Definitions.h>
#include <PrototypeCommon/Logger.h>
#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

PrototypeSceneNode::PrototypeSceneNode(std::string name)
  : _id(++PrototypeStaticInitializer::_sceneNodeUUID)
  , _mask(PrototypeSceneMaskVisible | PrototypeSceneMaskMatching | PrototypeSceneMaskPartialMatching)
  , _name(name)
  , _object(nullptr)
  , _parentLayer(nullptr)
  , _parentNode(nullptr)
{
    allocateObject();
}

PrototypeSceneNode::~PrototypeSceneNode()
{
    for (auto pair : _nodes) { pair.second->notifyRemoved(); }
    _nodes.clear();
    _remap_nodes.clear();
    deallocateObject();
}

void
PrototypeSceneNode::setParentLayer(PrototypeSceneLayer* parentLayer)
{
    _parentLayer = parentLayer;
}

void
PrototypeSceneNode::setParentNode(PrototypeSceneNode* parentNode)
{
    _parentNode = parentNode;
}

void
PrototypeSceneNode::allocateObject()
{
    if (!_object) { _object = PrototypeTraitSystem::createObject(); }
}

void
PrototypeSceneNode::deallocateObject()
{
    if (_object) { _object->destroy(); }
}

void
PrototypeSceneNode::notifyRemoved()
{
    for (auto pair : _nodes) { pair.second->notifyRemoved(); }
    _nodes.clear();
    _remap_nodes.clear();
    deallocateObject();
}

bool
PrototypeSceneNode::hasNode(const std::string& name)
{
    if (name == _name) { return true; }
    for (const auto& pair : _nodes) {
        bool inRecursiveBranch = pair.second->hasNode(name);
        if (inRecursiveBranch) return true;
    }
    return false;
}

bool
PrototypeSceneNode::hasNode(PrototypeSceneNode* node)
{
    if (!node) { return true; }
    if (node == this) { return true; }
    for (const auto& pair : _nodes) {
        bool inRecursiveBranch = pair.second->hasNode(node);
        if (inRecursiveBranch) return true;
    }
    return false;
}

bool
PrototypeSceneNode::addNode(const std::string name)
{
    if (hasNode(name)) { return false; }
    auto node = PrototypeEngineInternalApplication::database->allocateSceneNode(name);
    onAddNode(node);
    _nodes.insert({ node->id(), node });
    _remap_nodes.insert({ name, node->id() });
    return true;
}

bool
PrototypeSceneNode::addNode(PrototypeSceneNode* node)
{
    if (hasNode(node)) { return false; }
    onAddNode(node);
    _nodes.insert({ node->id(), node });
    _remap_nodes.insert({ node->name(), node->id() });
    return true;
}

bool
PrototypeSceneNode::moveNode(PrototypeSceneNode* node, PrototypeSceneNode* newParent)
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
PrototypeSceneNode::moveNode(PrototypeSceneNode* node, PrototypeSceneLayer* newParent)
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
PrototypeSceneNode::removeNodeById(const u32 id)
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
PrototypeSceneNode::removeNodeByName(const std::string name)
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
PrototypeSceneNode::renameNode(const std::string name)
{
    if (onRenameNode(this, name)) { _name = name; }
}

void
PrototypeSceneNode::show()
{
    _mask |= PrototypeSceneMaskVisible;
}

void
PrototypeSceneNode::hide()
{
    _mask &= ~PrototypeSceneMaskVisible;
}

void
PrototypeSceneNode::setHovering()
{
    _mask |= PrototypeSceneMaskHovered;
}

void
PrototypeSceneNode::unsetHovering()
{
    _mask &= ~PrototypeSceneMaskHovered;
}

void
PrototypeSceneNode::select()
{
    _mask |= PrototypeSceneMaskSelected;
    onSelectNode(this);
}

void
PrototypeSceneNode::deselect()
{
    _mask &= ~PrototypeSceneMaskSelected;
    onDeselectNode(this);
}

void
PrototypeSceneNode::setMatched()
{
    _mask |= PrototypeSceneMaskMatching;
}

void
PrototypeSceneNode::unsetMatched()
{
    _mask &= ~PrototypeSceneMaskMatching;
}

void
PrototypeSceneNode::setPartialMatched()
{
    _mask |= PrototypeSceneMaskPartialMatching;
}

void
PrototypeSceneNode::unsetPartialMatched()
{
    _mask &= ~PrototypeSceneMaskPartialMatching;
}

void
PrototypeSceneNode::onAddNode(PrototypeSceneNode* node)
{
    if (_parentLayer) {
        _parentLayer->onAddNode(node);
    } else if (_parentNode) {
        _parentNode->onAddNode(node);
    }
}

bool
PrototypeSceneNode::onMoveNode(PrototypeSceneNode* node, PrototypeSceneNode* newParent, PrototypeSceneNode* oldParent)
{
    if (newParent->nodesByName(node->name()).has_value()) return false;
    node->setParentLayer(nullptr);
    node->setParentNode(newParent);
    newParent->addNode(node);
    return true;
}

bool
PrototypeSceneNode::onMoveNode(PrototypeSceneNode* node, PrototypeSceneLayer* newParent, PrototypeSceneNode* oldParent)
{
    if (newParent->nodesByName(node->name()).has_value()) return false;
    node->setParentLayer(newParent);
    node->setParentNode(nullptr);
    newParent->addNode(node);
    return true;
}

void
PrototypeSceneNode::onRemoveNode(PrototypeSceneNode* node)
{
    if (_parentLayer) {
        _parentLayer->onRemoveNode(node);
    } else if (_parentNode) {
        _parentNode->onRemoveNode(node);
    }
}

bool
PrototypeSceneNode::onRenameNode(PrototypeSceneNode* node, const std::string& newName)
{
    if (_parentLayer) {
        return _parentLayer->onRenameNode(node, newName);
    } else if (_parentNode) {
        if (_parentNode->_remap_nodes.find(newName) != _parentNode->_remap_nodes.end()) return false;
        auto name                          = node->name();
        auto id                            = node->id();
        _parentNode->_remap_nodes[newName] = id;
        _parentNode->_remap_nodes.erase(name);
        return true;
    }
    return false;
}

void
PrototypeSceneNode::onSelectNode(PrototypeSceneNode* node)
{
    if (_parentLayer) {
        _parentLayer->onSelectNode(node);
    } else if (_parentNode) {
        _parentNode->onSelectNode(node);
    }
}

void
PrototypeSceneNode::onDeselectNode(PrototypeSceneNode* node)
{
    if (_parentLayer) {
        _parentLayer->onDeselectNode(node);
    } else if (_parentNode) {
        _parentNode->onDeselectNode(node);
    }
}

const u32&
PrototypeSceneNode::id() const
{
    return _id;
}

const bool
PrototypeSceneNode::isVisible() const
{
    return (_mask & PrototypeSceneMaskVisible) == PrototypeSceneMaskVisible;
}

const bool
PrototypeSceneNode::isHovered() const
{
    return (_mask & PrototypeSceneMaskHovered) == PrototypeSceneMaskHovered;
}

const bool
PrototypeSceneNode::isSelected() const
{
    return (_mask & PrototypeSceneMaskSelected) == PrototypeSceneMaskSelected;
}

const bool
PrototypeSceneNode::isMatching() const
{
    return (_mask & PrototypeSceneMaskMatching) == PrototypeSceneMaskMatching;
}

const bool
PrototypeSceneNode::isPartialMatching() const
{
    return (_mask & PrototypeSceneMaskPartialMatching) == PrototypeSceneMaskPartialMatching;
}

const std::string&
PrototypeSceneNode::name() const
{
    return _name;
}

const std::optional<PrototypeObject*>
PrototypeSceneNode::object() const
{
    if (_object) { return { _object }; }
    return {};
}

const std::optional<PrototypeSceneNode*>
PrototypeSceneNode::nodesById(const u32 id) const
{
    auto it = _nodes.find(id);
    if (it != _nodes.end()) { return { it->second }; }
    return {};
}

const std::optional<PrototypeSceneNode*>
PrototypeSceneNode::nodesByName(const std::string name) const
{
    auto it = _remap_nodes.find(name);
    if (it != _remap_nodes.end()) {
        auto it2 = _nodes.find(it->second);
        if (it2 != _nodes.end()) { return { it2->second }; }
    }
    return {};
}

const std::unordered_map<u32, PrototypeSceneNode*>
PrototypeSceneNode::nodes() const
{
    return _nodes;
}

PrototypeSceneLayer*
PrototypeSceneNode::parentLayer()
{
    return _parentLayer;
}

PrototypeSceneNode*
PrototypeSceneNode::parentNode()
{
    return _parentNode;
}

PrototypeSceneLayer*
PrototypeSceneNode::absoluteLayer()
{
    if (_parentLayer) return _parentLayer;
    if (_parentNode) return _parentNode->absoluteLayer();
    return nullptr;
}

void
PrototypeSceneNode::to_json(nlohmann::json& j, const PrototypeSceneNode& node)
{
    const char* field_id         = "id";
    const char* field_name       = "name";
    const char* field_components = "components";
    const char* field_nodes      = "nodes";

    j[field_id]   = node.id();
    j[field_name] = node.name();
    std::vector<nlohmann::json> jnodes;
    for (auto pair : node._nodes) {
        nlohmann::json jnode;
        PrototypeSceneNode::to_json(jnode, *pair.second);
        jnodes.push_back(jnode);
    }
    j[field_nodes]                        = jnodes;
    auto                        optObject = node.object();
    std::vector<nlohmann::json> jcomponents;
    j[field_components] = jcomponents;
    if (optObject.has_value()) {
        PrototypeObject* object = optObject.value();
        PrototypeObject::to_json(j[field_components], *object);
    }
}

std::optional<PrototypeSceneNode*>
PrototypeSceneNode::from_json(const nlohmann::json& j, PrototypeScene* scene)
{
    if (j.is_null()) { return {}; }

    const char* field_name       = "name";
    const char* field_components = "components";
    const char* field_nodes      = "nodes";

    if (!j.contains(field_name)) {
        PrototypeLogger::warn("Scene node doesn't have name");
        return {};
    }

    const std::string nodeName = j.at(field_name).get<std::string>();
    auto              node     = PrototypeEngineInternalApplication::database->allocateSceneNode(nodeName);

    // load components
    if (j.contains(field_components)) {
        const auto optObject = node->object();
        if (optObject.has_value()) {
            PrototypeObject* object = optObject.value();
            object->setParentNode(static_cast<void*>(node));
            for (auto jcomponent : j.at(field_components)) {
                if (jcomponent.is_null()) continue;
                if (jcomponent.contains(field_name)) { PrototypeObject::from_json(jcomponent, *object); }
            }
        }
    }
    // load child nodes
    if (j.contains(field_nodes)) {
        for (auto jnode : j.at(field_nodes)) {
            if (jnode.is_null()) continue;
            auto optChildNode = PrototypeSceneNode::from_json(jnode, scene);
            if (optChildNode.has_value()) {
                auto childNode = optChildNode.value();
                childNode->setParentLayer(nullptr);
                childNode->setParentNode(node);
                if (node->addNode(childNode)) {
                    PrototypeEngineInternalApplication::database->sceneNodes[scene].insert({ childNode->name(), childNode });
                } else {
                    PrototypeEngineInternalApplication::database->deallocateSceneNode(childNode);
                }
            }
        }
    }
    return { node };
}