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

#include <PrototypeCommon/Bitflag.h>

#include <memory>
#include <set>
#include <unordered_set>
#include <vector>

struct PrototypeSceneNode;
struct PrototypeSceneLayer;
struct PrototypeObject;

struct PrototypeSceneFilter
{
    PrototypeSceneFilter(MASK_TYPE traitMask);
    ~PrototypeSceneFilter();

    void onAddSceneNode(PrototypeSceneNode* node);
    void onRemoveSceneNode(PrototypeSceneNode* node);
    void onAddSceneNodeTraits(PrototypeSceneNode* node, u64 features);
    void onRemoveSceneNodeTraits(PrototypeSceneNode* node, u64 traitMask);

    const std::unordered_set<PrototypeObject*>& data() const;
    MASK_TYPE traitMask() const;

  private:
    std::unordered_set<PrototypeObject*> _data;
    PrototypeBitflag                     _filter;
};
