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

#include "PrototypeSceneFilter.h"
#include "PrototypeSceneNode.h"

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include <PrototypeCommon/Maths.h>

#include <algorithm>

PrototypeSceneFilter::PrototypeSceneFilter(MASK_TYPE traitMask) { _filter = PrototypeBitflagFrom(traitMask); }

PrototypeSceneFilter::~PrototypeSceneFilter() { _data.clear(); }

void
PrototypeSceneFilter::onAddSceneNode(PrototypeSceneNode* node)
{
    auto optObject = node->object();
    if (optObject.has_value()) {
        auto             obj = optObject.value();
        PrototypeBitflag bf  = PrototypeBitflagFrom(obj->traits());
        if (_data.find(obj) != _data.end()) return;
        if (PrototypeBitflagHas(bf, _filter.features)) { _data.insert(obj); }
    }
}

void
PrototypeSceneFilter::onRemoveSceneNode(PrototypeSceneNode* node)
{
    auto optNodeObject = node->object();
    if (optNodeObject.has_value()) {
        auto nodeObject = optNodeObject.value();
        auto it         = _data.find(nodeObject);
        if (it != _data.end()) { _data.erase(it); }
    }
}

void
PrototypeSceneFilter::onAddSceneNodeTraits(PrototypeSceneNode* node, u64 features)
{
    auto optObject = node->object();
    if (optObject.has_value()) {
        auto             obj = optObject.value();
        PrototypeBitflag bf  = PrototypeBitflagFrom(obj->traits());
        if (_data.find(obj) != _data.end()) return;
        if (PrototypeBitflagHas(bf, _filter.features)) { _data.insert(obj); }
    }
}

void
PrototypeSceneFilter::onRemoveSceneNodeTraits(PrototypeSceneNode* node, u64 traitMask)
{
    auto optObject = node->object();
    if (optObject.has_value()) {
        auto             obj = optObject.value();
        PrototypeBitflag bf  = PrototypeBitflagFrom(obj->traits());
        PrototypeBitflagRemove(bf, traitMask);
        auto it = _data.find(obj);
        if (it == _data.end()) return;
        if (!PrototypeBitflagHas(bf, _filter.features)) { _data.erase(it); }
    }
}

const std::unordered_set<PrototypeObject*>&
PrototypeSceneFilter::data() const
{
    return _data;
}

MASK_TYPE
PrototypeSceneFilter::traitMask() const
{
    return _filter.features;
}