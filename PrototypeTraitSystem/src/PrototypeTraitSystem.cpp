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

#include "../include/PrototypeTraitSystem/PrototypeTraitSystem.h"

#include <algorithm>

PrototypeTraitSystemData* PrototypeTraitSystem::_data = nullptr;

PrototypeTraitSystemData::PrototypeTraitSystemData()
  : _gid(0)
  , _addCameraCbFn(nullptr)
  , _reuseCameraCbFn(nullptr)
  , _removeCameraCbFn(nullptr)
  , _logCameraCbFn(nullptr)
  , _addColliderCbFn(nullptr)
  , _reuseColliderCbFn(nullptr)
  , _removeColliderCbFn(nullptr)
  , _logColliderCbFn(nullptr)
  , _addMeshRendererCbFn(nullptr)
  , _reuseMeshRendererCbFn(nullptr)
  , _removeMeshRendererCbFn(nullptr)
  , _logMeshRendererCbFn(nullptr)
  , _addRigidbodyCbFn(nullptr)
  , _reuseRigidbodyCbFn(nullptr)
  , _removeRigidbodyCbFn(nullptr)
  , _logRigidbodyCbFn(nullptr)
  , _addScriptCbFn(nullptr)
  , _reuseScriptCbFn(nullptr)
  , _removeScriptCbFn(nullptr)
  , _logScriptCbFn(nullptr)
  , _addTransformCbFn(nullptr)
  , _reuseTransformCbFn(nullptr)
  , _removeTransformCbFn(nullptr)
  , _logTransformCbFn(nullptr)
  , _addVehicleChasisCbFn(nullptr)
  , _reuseVehicleChasisCbFn(nullptr)
  , _removeVehicleChasisCbFn(nullptr)
  , _logVehicleChasisCbFn(nullptr)

{}

PrototypeTraitSystemData::~PrototypeTraitSystemData() {}

PrototypeObject::PrototypeObject()
  : _id(++PrototypeTraitSystem::_data->_gid)
  , _parentNode(nullptr)
  , _traits({ 0 })
{}

PrototypeObject::~PrototypeObject()
{
    removeCameraTrait();
    removeColliderTrait();
    removeMeshRendererTrait();
    removeRigidbodyTrait();
    removeScriptTrait();
    removeTransformTrait();
    removeVehicleChasisTrait();

    garbageCollectCameraTraitMemory();
    garbageCollectColliderTraitMemory();
    garbageCollectMeshRendererTraitMemory();
    garbageCollectRigidbodyTraitMemory();
    garbageCollectScriptTraitMemory();
    garbageCollectTransformTraitMemory();
    garbageCollectVehicleChasisTraitMemory();

    _id = 0;
}

void
PrototypeObject::destroy()
{
    auto it = std::find(PrototypeTraitSystem::_data->_objects.begin(), PrototypeTraitSystem::_data->_objects.end(), this);
    if (it != PrototypeTraitSystem::_data->_objects.end()) {
        PrototypeObject* et = *it;
        PrototypeTraitSystem::_data->_objects.erase(it);
        PrototypeTraitSystem::_data->_pool.deleteElement(et);
    }
}

u32
PrototypeObject::id() const
{
    return _id;
}

MASK_TYPE
PrototypeObject::traits() const { return _traits.features; }

void
PrototypeObject::log()
{
    if (hasCameraTrait()) {
        if (PrototypeTraitSystem::_data->_logCameraCbFn) {
            if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskCamera)) {
                Camera* cmp = PrototypeTraitSystem::_data->_cameraVector[PrototypeTraitSystem::_data->_cameraMap[_id]];
                PrototypeTraitSystem::_data->_logCameraCbFn(this, cmp);
            }
        }
    }
    if (hasColliderTrait()) {
        if (PrototypeTraitSystem::_data->_logColliderCbFn) {
            if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskCollider)) {
                Collider* cmp = PrototypeTraitSystem::_data->_colliderVector[PrototypeTraitSystem::_data->_colliderMap[_id]];
                PrototypeTraitSystem::_data->_logColliderCbFn(this, cmp);
            }
        }
    }
    if (hasMeshRendererTrait()) {
        if (PrototypeTraitSystem::_data->_logMeshRendererCbFn) {
            if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskMeshRenderer)) {
                MeshRenderer* cmp =
                  PrototypeTraitSystem::_data->_meshRendererVector[PrototypeTraitSystem::_data->_meshRendererMap[_id]];
                PrototypeTraitSystem::_data->_logMeshRendererCbFn(this, cmp);
            }
        }
    }
    if (hasRigidbodyTrait()) {
        if (PrototypeTraitSystem::_data->_logRigidbodyCbFn) {
            if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskRigidbody)) {
                Rigidbody* cmp = PrototypeTraitSystem::_data->_rigidbodyVector[PrototypeTraitSystem::_data->_rigidbodyMap[_id]];
                PrototypeTraitSystem::_data->_logRigidbodyCbFn(this, cmp);
            }
        }
    }
    if (hasScriptTrait()) {
        if (PrototypeTraitSystem::_data->_logScriptCbFn) {
            if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskScript)) {
                Script* cmp = PrototypeTraitSystem::_data->_scriptVector[PrototypeTraitSystem::_data->_scriptMap[_id]];
                PrototypeTraitSystem::_data->_logScriptCbFn(this, cmp);
            }
        }
    }
    if (hasTransformTrait()) {
        if (PrototypeTraitSystem::_data->_logTransformCbFn) {
            if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskTransform)) {
                Transform* cmp = PrototypeTraitSystem::_data->_transformVector[PrototypeTraitSystem::_data->_transformMap[_id]];
                PrototypeTraitSystem::_data->_logTransformCbFn(this, cmp);
            }
        }
    }
    if (hasVehicleChasisTrait()) {
        if (PrototypeTraitSystem::_data->_logVehicleChasisCbFn) {
            if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskVehicleChasis)) {
                VehicleChasis* cmp =
                  PrototypeTraitSystem::_data->_vehicleChasisVector[PrototypeTraitSystem::_data->_vehicleChasisMap[_id]];
                PrototypeTraitSystem::_data->_logVehicleChasisCbFn(this, cmp);
            }
        }
    }
}

void
PrototypeObject::add(MASK_TYPE traitMask)
{
    PrototypeBitflag bf = PrototypeBitflagFrom(traitMask);
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskCamera) && !has(PrototypeTraitTypeMaskCamera)) {
        PrototypeObject::addCameraTrait();
    }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskCollider) && !has(PrototypeTraitTypeMaskCollider)) {
        PrototypeObject::addColliderTrait();
    }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskMeshRenderer) && !has(PrototypeTraitTypeMaskMeshRenderer)) {
        PrototypeObject::addMeshRendererTrait();
    }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskRigidbody) && !has(PrototypeTraitTypeMaskRigidbody)) {
        PrototypeObject::addRigidbodyTrait();
    }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskScript) && !has(PrototypeTraitTypeMaskScript)) {
        PrototypeObject::addScriptTrait();
    }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskTransform) && !has(PrototypeTraitTypeMaskTransform)) {
        PrototypeObject::addTransformTrait();
    }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskVehicleChasis) && !has(PrototypeTraitTypeMaskVehicleChasis)) {
        PrototypeObject::addVehicleChasisTrait();
    }
}

bool
PrototypeObject::has(MASK_TYPE traitMask)
{
    return PrototypeBitflagHas(_traits, traitMask);
}

void
PrototypeObject::remove(MASK_TYPE traitMask)
{
    auto bf = PrototypeBitflagFrom(traitMask);
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskCamera)) { PrototypeObject::removeCameraTrait(); }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskCollider)) { PrototypeObject::removeColliderTrait(); }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskMeshRenderer)) { PrototypeObject::removeMeshRendererTrait(); }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskRigidbody)) { PrototypeObject::removeRigidbodyTrait(); }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskScript)) { PrototypeObject::removeScriptTrait(); }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskTransform)) { PrototypeObject::removeTransformTrait(); }
    if (PrototypeBitflagHas(bf, PrototypeTraitTypeMaskVehicleChasis)) { PrototypeObject::removeVehicleChasisTrait(); }
}

void
PrototypeObject::setParentNode(void* parentNode)
{
    _parentNode = parentNode;
}

void*
PrototypeObject::parentNode() const
{
    if (_parentNode) { return static_cast<void*>(_parentNode); }
    return nullptr;
}

void
PrototypeObject::onEdit()
{
    if (PrototypeObject::hasCameraTrait()) { Camera::onEditDispatch(this); }
    if (PrototypeObject::hasColliderTrait()) { Collider::onEditDispatch(this); }
    if (PrototypeObject::hasMeshRendererTrait()) { MeshRenderer::onEditDispatch(this); }
    if (PrototypeObject::hasRigidbodyTrait()) { Rigidbody::onEditDispatch(this); }
    if (PrototypeObject::hasScriptTrait()) { Script::onEditDispatch(this); }
    if (PrototypeObject::hasTransformTrait()) { Transform::onEditDispatch(this); }
    if (PrototypeObject::hasVehicleChasisTrait()) { VehicleChasis::onEditDispatch(this); }
}

void
PrototypeObject::addCameraTrait()
{
    if (!PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskCamera)) {
        PrototypeBitflagAdd(_traits, PrototypeTraitTypeMaskCamera);
        if (PrototypeTraitSystem::_data->_cameraGarbageMap.find(_id) == PrototypeTraitSystem::_data->_cameraGarbageMap.end()) {
            PrototypeTraitSystem::_data->_cameraMap[_id] = PrototypeTraitSystem::_data->_cameraVector.size();
            auto newcmp                                  = PrototypeTraitSystem::_data->_cameraPool.newElement();
            newcmp->_object                              = this;
            PrototypeTraitSystem::_data->_cameraVector.emplace_back(newcmp);
            if (PrototypeTraitSystem::_data->_addCameraCbFn) { PrototypeTraitSystem::_data->_addCameraCbFn(this, newcmp); }
        } else {
            PrototypeTraitSystem::_data->_cameraMap[_id] = PrototypeTraitSystem::_data->_cameraGarbageMap[_id];
            PrototypeTraitSystem::_data->_cameraGarbageMap.erase(_id);
            if (PrototypeTraitSystem::_data->_reuseCameraCbFn) {
                PrototypeTraitSystem::_data->_reuseCameraCbFn(
                  this, PrototypeTraitSystem::_data->_cameraVector[PrototypeTraitSystem::_data->_cameraMap[_id]]);
            }
        }
    }
}

void
PrototypeObject::removeCameraTrait()
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskCamera)) {
        if (PrototypeTraitSystem::_data->_removeCameraCbFn) {
            PrototypeTraitSystem::_data->_removeCameraCbFn(this, this->getCameraTrait());
        }
        PrototypeBitflagRemove(_traits, PrototypeTraitTypeMaskCamera);
        PrototypeTraitSystem::_data->_cameraGarbageMap[_id] = PrototypeTraitSystem::_data->_cameraMap[_id];
        PrototypeTraitSystem::_data->_cameraMap.erase(_id);
    }
}

void
PrototypeObject::garbageCollectCameraTraitMemory()
{
    for (auto pair : PrototypeTraitSystem::_data->_cameraGarbageMap) {
        for (auto& pair2 : PrototypeTraitSystem::_data->_cameraMap) {
            if (pair2.second > pair.second) { --pair2.second; }
        }
        PrototypeTraitSystem::_data->_cameraPool.deleteElement(PrototypeTraitSystem::_data->_cameraVector[pair.second]);
        PrototypeTraitSystem::_data->_cameraVector.erase(PrototypeTraitSystem::_data->_cameraVector.begin() + pair.second);
    }
    PrototypeTraitSystem::_data->_cameraGarbageMap.clear();
}

bool
PrototypeObject::hasCameraTrait() const
{
    return PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskCamera);
}

Camera*
PrototypeObject::getCameraTrait() const
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskCamera)) {
        return PrototypeTraitSystem::_data->_cameraVector[PrototypeTraitSystem::_data->_cameraMap[_id]];
    }
    return nullptr;
}
void
PrototypeObject::addColliderTrait()
{
    if (!PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskCollider)) {
        PrototypeBitflagAdd(_traits, PrototypeTraitTypeMaskCollider);
        if (PrototypeTraitSystem::_data->_colliderGarbageMap.find(_id) ==
            PrototypeTraitSystem::_data->_colliderGarbageMap.end()) {
            PrototypeTraitSystem::_data->_colliderMap[_id] = PrototypeTraitSystem::_data->_colliderVector.size();
            auto newcmp                                    = PrototypeTraitSystem::_data->_colliderPool.newElement();
            newcmp->_object                                = this;
            PrototypeTraitSystem::_data->_colliderVector.emplace_back(newcmp);
            if (PrototypeTraitSystem::_data->_addColliderCbFn) { PrototypeTraitSystem::_data->_addColliderCbFn(this, newcmp); }
        } else {
            PrototypeTraitSystem::_data->_colliderMap[_id] = PrototypeTraitSystem::_data->_colliderGarbageMap[_id];
            PrototypeTraitSystem::_data->_colliderGarbageMap.erase(_id);
            if (PrototypeTraitSystem::_data->_reuseColliderCbFn) {
                PrototypeTraitSystem::_data->_reuseColliderCbFn(
                  this, PrototypeTraitSystem::_data->_colliderVector[PrototypeTraitSystem::_data->_colliderMap[_id]]);
            }
        }
    }
}

void
PrototypeObject::removeColliderTrait()
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskCollider)) {
        if (PrototypeTraitSystem::_data->_removeColliderCbFn) {
            PrototypeTraitSystem::_data->_removeColliderCbFn(this, this->getColliderTrait());
        }
        PrototypeBitflagRemove(_traits, PrototypeTraitTypeMaskCollider);
        PrototypeTraitSystem::_data->_colliderGarbageMap[_id] = PrototypeTraitSystem::_data->_colliderMap[_id];
        PrototypeTraitSystem::_data->_colliderMap.erase(_id);
    }
}

void
PrototypeObject::garbageCollectColliderTraitMemory()
{
    for (auto pair : PrototypeTraitSystem::_data->_colliderGarbageMap) {
        for (auto& pair2 : PrototypeTraitSystem::_data->_colliderMap) {
            if (pair2.second > pair.second) { --pair2.second; }
        }
        PrototypeTraitSystem::_data->_colliderPool.deleteElement(PrototypeTraitSystem::_data->_colliderVector[pair.second]);
        PrototypeTraitSystem::_data->_colliderVector.erase(PrototypeTraitSystem::_data->_colliderVector.begin() + pair.second);
    }
    PrototypeTraitSystem::_data->_colliderGarbageMap.clear();
}

bool
PrototypeObject::hasColliderTrait() const
{
    return PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskCollider);
}

Collider*
PrototypeObject::getColliderTrait() const
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskCollider)) {
        return PrototypeTraitSystem::_data->_colliderVector[PrototypeTraitSystem::_data->_colliderMap[_id]];
    }
    return nullptr;
}
void
PrototypeObject::addMeshRendererTrait()
{
    if (!PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskMeshRenderer)) {
        PrototypeBitflagAdd(_traits, PrototypeTraitTypeMaskMeshRenderer);
        if (PrototypeTraitSystem::_data->_meshRendererGarbageMap.find(_id) ==
            PrototypeTraitSystem::_data->_meshRendererGarbageMap.end()) {
            PrototypeTraitSystem::_data->_meshRendererMap[_id] = PrototypeTraitSystem::_data->_meshRendererVector.size();
            auto newcmp                                        = PrototypeTraitSystem::_data->_meshRendererPool.newElement();
            newcmp->_object                                    = this;
            PrototypeTraitSystem::_data->_meshRendererVector.emplace_back(newcmp);
            if (PrototypeTraitSystem::_data->_addMeshRendererCbFn) {
                PrototypeTraitSystem::_data->_addMeshRendererCbFn(this, newcmp);
            }
        } else {
            PrototypeTraitSystem::_data->_meshRendererMap[_id] = PrototypeTraitSystem::_data->_meshRendererGarbageMap[_id];
            PrototypeTraitSystem::_data->_meshRendererGarbageMap.erase(_id);
            if (PrototypeTraitSystem::_data->_reuseMeshRendererCbFn) {
                PrototypeTraitSystem::_data->_reuseMeshRendererCbFn(
                  this, PrototypeTraitSystem::_data->_meshRendererVector[PrototypeTraitSystem::_data->_meshRendererMap[_id]]);
            }
        }
    }
}

void
PrototypeObject::removeMeshRendererTrait()
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskMeshRenderer)) {
        if (PrototypeTraitSystem::_data->_removeMeshRendererCbFn) {
            PrototypeTraitSystem::_data->_removeMeshRendererCbFn(this, this->getMeshRendererTrait());
        }
        PrototypeBitflagRemove(_traits, PrototypeTraitTypeMaskMeshRenderer);
        PrototypeTraitSystem::_data->_meshRendererGarbageMap[_id] = PrototypeTraitSystem::_data->_meshRendererMap[_id];
        PrototypeTraitSystem::_data->_meshRendererMap.erase(_id);
    }
}

void
PrototypeObject::garbageCollectMeshRendererTraitMemory()
{
    for (auto pair : PrototypeTraitSystem::_data->_meshRendererGarbageMap) {
        for (auto& pair2 : PrototypeTraitSystem::_data->_meshRendererMap) {
            if (pair2.second > pair.second) { --pair2.second; }
        }
        PrototypeTraitSystem::_data->_meshRendererPool.deleteElement(
          PrototypeTraitSystem::_data->_meshRendererVector[pair.second]);
        PrototypeTraitSystem::_data->_meshRendererVector.erase(PrototypeTraitSystem::_data->_meshRendererVector.begin() +
                                                               pair.second);
    }
    PrototypeTraitSystem::_data->_meshRendererGarbageMap.clear();
}

bool
PrototypeObject::hasMeshRendererTrait() const
{
    return PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskMeshRenderer);
}

MeshRenderer*
PrototypeObject::getMeshRendererTrait() const
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskMeshRenderer)) {
        return PrototypeTraitSystem::_data->_meshRendererVector[PrototypeTraitSystem::_data->_meshRendererMap[_id]];
    }
    return nullptr;
}
void
PrototypeObject::addRigidbodyTrait()
{
    if (!PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskRigidbody)) {
        PrototypeBitflagAdd(_traits, PrototypeTraitTypeMaskRigidbody);
        if (PrototypeTraitSystem::_data->_rigidbodyGarbageMap.find(_id) ==
            PrototypeTraitSystem::_data->_rigidbodyGarbageMap.end()) {
            PrototypeTraitSystem::_data->_rigidbodyMap[_id] = PrototypeTraitSystem::_data->_rigidbodyVector.size();
            auto newcmp                                     = PrototypeTraitSystem::_data->_rigidbodyPool.newElement();
            newcmp->_object                                 = this;
            PrototypeTraitSystem::_data->_rigidbodyVector.emplace_back(newcmp);
            if (PrototypeTraitSystem::_data->_addRigidbodyCbFn) { PrototypeTraitSystem::_data->_addRigidbodyCbFn(this, newcmp); }
        } else {
            PrototypeTraitSystem::_data->_rigidbodyMap[_id] = PrototypeTraitSystem::_data->_rigidbodyGarbageMap[_id];
            PrototypeTraitSystem::_data->_rigidbodyGarbageMap.erase(_id);
            if (PrototypeTraitSystem::_data->_reuseRigidbodyCbFn) {
                PrototypeTraitSystem::_data->_reuseRigidbodyCbFn(
                  this, PrototypeTraitSystem::_data->_rigidbodyVector[PrototypeTraitSystem::_data->_rigidbodyMap[_id]]);
            }
        }
    }
}

void
PrototypeObject::removeRigidbodyTrait()
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskRigidbody)) {
        if (PrototypeTraitSystem::_data->_removeRigidbodyCbFn) {
            PrototypeTraitSystem::_data->_removeRigidbodyCbFn(this, this->getRigidbodyTrait());
        }
        PrototypeBitflagRemove(_traits, PrototypeTraitTypeMaskRigidbody);
        PrototypeTraitSystem::_data->_rigidbodyGarbageMap[_id] = PrototypeTraitSystem::_data->_rigidbodyMap[_id];
        PrototypeTraitSystem::_data->_rigidbodyMap.erase(_id);
    }
}

void
PrototypeObject::garbageCollectRigidbodyTraitMemory()
{
    for (auto pair : PrototypeTraitSystem::_data->_rigidbodyGarbageMap) {
        for (auto& pair2 : PrototypeTraitSystem::_data->_rigidbodyMap) {
            if (pair2.second > pair.second) { --pair2.second; }
        }
        PrototypeTraitSystem::_data->_rigidbodyPool.deleteElement(PrototypeTraitSystem::_data->_rigidbodyVector[pair.second]);
        PrototypeTraitSystem::_data->_rigidbodyVector.erase(PrototypeTraitSystem::_data->_rigidbodyVector.begin() + pair.second);
    }
    PrototypeTraitSystem::_data->_rigidbodyGarbageMap.clear();
}

bool
PrototypeObject::hasRigidbodyTrait() const
{
    return PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskRigidbody);
}

Rigidbody*
PrototypeObject::getRigidbodyTrait() const
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskRigidbody)) {
        return PrototypeTraitSystem::_data->_rigidbodyVector[PrototypeTraitSystem::_data->_rigidbodyMap[_id]];
    }
    return nullptr;
}
void
PrototypeObject::addScriptTrait()
{
    if (!PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskScript)) {
        PrototypeBitflagAdd(_traits, PrototypeTraitTypeMaskScript);
        if (PrototypeTraitSystem::_data->_scriptGarbageMap.find(_id) == PrototypeTraitSystem::_data->_scriptGarbageMap.end()) {
            PrototypeTraitSystem::_data->_scriptMap[_id] = PrototypeTraitSystem::_data->_scriptVector.size();
            auto newcmp                                  = PrototypeTraitSystem::_data->_scriptPool.newElement();
            newcmp->_object                              = this;
            PrototypeTraitSystem::_data->_scriptVector.emplace_back(newcmp);
            if (PrototypeTraitSystem::_data->_addScriptCbFn) { PrototypeTraitSystem::_data->_addScriptCbFn(this, newcmp); }
        } else {
            PrototypeTraitSystem::_data->_scriptMap[_id] = PrototypeTraitSystem::_data->_scriptGarbageMap[_id];
            PrototypeTraitSystem::_data->_scriptGarbageMap.erase(_id);
            if (PrototypeTraitSystem::_data->_reuseScriptCbFn) {
                PrototypeTraitSystem::_data->_reuseScriptCbFn(
                  this, PrototypeTraitSystem::_data->_scriptVector[PrototypeTraitSystem::_data->_scriptMap[_id]]);
            }
        }
    }
}

void
PrototypeObject::removeScriptTrait()
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskScript)) {
        if (PrototypeTraitSystem::_data->_removeScriptCbFn) {
            PrototypeTraitSystem::_data->_removeScriptCbFn(this, this->getScriptTrait());
        }
        PrototypeBitflagRemove(_traits, PrototypeTraitTypeMaskScript);
        PrototypeTraitSystem::_data->_scriptGarbageMap[_id] = PrototypeTraitSystem::_data->_scriptMap[_id];
        PrototypeTraitSystem::_data->_scriptMap.erase(_id);
    }
}

void
PrototypeObject::garbageCollectScriptTraitMemory()
{
    for (auto pair : PrototypeTraitSystem::_data->_scriptGarbageMap) {
        for (auto& pair2 : PrototypeTraitSystem::_data->_scriptMap) {
            if (pair2.second > pair.second) { --pair2.second; }
        }
        PrototypeTraitSystem::_data->_scriptPool.deleteElement(PrototypeTraitSystem::_data->_scriptVector[pair.second]);
        PrototypeTraitSystem::_data->_scriptVector.erase(PrototypeTraitSystem::_data->_scriptVector.begin() + pair.second);
    }
    PrototypeTraitSystem::_data->_scriptGarbageMap.clear();
}

bool
PrototypeObject::hasScriptTrait() const
{
    return PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskScript);
}

Script*
PrototypeObject::getScriptTrait() const
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskScript)) {
        return PrototypeTraitSystem::_data->_scriptVector[PrototypeTraitSystem::_data->_scriptMap[_id]];
    }
    return nullptr;
}
void
PrototypeObject::addTransformTrait()
{
    if (!PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskTransform)) {
        PrototypeBitflagAdd(_traits, PrototypeTraitTypeMaskTransform);
        if (PrototypeTraitSystem::_data->_transformGarbageMap.find(_id) ==
            PrototypeTraitSystem::_data->_transformGarbageMap.end()) {
            PrototypeTraitSystem::_data->_transformMap[_id] = PrototypeTraitSystem::_data->_transformVector.size();
            auto newcmp                                     = PrototypeTraitSystem::_data->_transformPool.newElement();
            newcmp->_object                                 = this;
            PrototypeTraitSystem::_data->_transformVector.emplace_back(newcmp);
            if (PrototypeTraitSystem::_data->_addTransformCbFn) { PrototypeTraitSystem::_data->_addTransformCbFn(this, newcmp); }
        } else {
            PrototypeTraitSystem::_data->_transformMap[_id] = PrototypeTraitSystem::_data->_transformGarbageMap[_id];
            PrototypeTraitSystem::_data->_transformGarbageMap.erase(_id);
            if (PrototypeTraitSystem::_data->_reuseTransformCbFn) {
                PrototypeTraitSystem::_data->_reuseTransformCbFn(
                  this, PrototypeTraitSystem::_data->_transformVector[PrototypeTraitSystem::_data->_transformMap[_id]]);
            }
        }
    }
}

void
PrototypeObject::removeTransformTrait()
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskTransform)) {
        if (PrototypeTraitSystem::_data->_removeTransformCbFn) {
            PrototypeTraitSystem::_data->_removeTransformCbFn(this, this->getTransformTrait());
        }
        PrototypeBitflagRemove(_traits, PrototypeTraitTypeMaskTransform);
        PrototypeTraitSystem::_data->_transformGarbageMap[_id] = PrototypeTraitSystem::_data->_transformMap[_id];
        PrototypeTraitSystem::_data->_transformMap.erase(_id);
    }
}

void
PrototypeObject::garbageCollectTransformTraitMemory()
{
    for (auto pair : PrototypeTraitSystem::_data->_transformGarbageMap) {
        for (auto& pair2 : PrototypeTraitSystem::_data->_transformMap) {
            if (pair2.second > pair.second) { --pair2.second; }
        }
        PrototypeTraitSystem::_data->_transformPool.deleteElement(PrototypeTraitSystem::_data->_transformVector[pair.second]);
        PrototypeTraitSystem::_data->_transformVector.erase(PrototypeTraitSystem::_data->_transformVector.begin() + pair.second);
    }
    PrototypeTraitSystem::_data->_transformGarbageMap.clear();
}

bool
PrototypeObject::hasTransformTrait() const
{
    return PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskTransform);
}

Transform*
PrototypeObject::getTransformTrait() const
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskTransform)) {
        return PrototypeTraitSystem::_data->_transformVector[PrototypeTraitSystem::_data->_transformMap[_id]];
    }
    return nullptr;
}
void
PrototypeObject::addVehicleChasisTrait()
{
    if (!PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskVehicleChasis)) {
        PrototypeBitflagAdd(_traits, PrototypeTraitTypeMaskVehicleChasis);
        if (PrototypeTraitSystem::_data->_vehicleChasisGarbageMap.find(_id) ==
            PrototypeTraitSystem::_data->_vehicleChasisGarbageMap.end()) {
            PrototypeTraitSystem::_data->_vehicleChasisMap[_id] = PrototypeTraitSystem::_data->_vehicleChasisVector.size();
            auto newcmp                                         = PrototypeTraitSystem::_data->_vehicleChasisPool.newElement();
            newcmp->_object                                     = this;
            PrototypeTraitSystem::_data->_vehicleChasisVector.emplace_back(newcmp);
            if (PrototypeTraitSystem::_data->_addVehicleChasisCbFn) {
                PrototypeTraitSystem::_data->_addVehicleChasisCbFn(this, newcmp);
            }
        } else {
            PrototypeTraitSystem::_data->_vehicleChasisMap[_id] = PrototypeTraitSystem::_data->_vehicleChasisGarbageMap[_id];
            PrototypeTraitSystem::_data->_vehicleChasisGarbageMap.erase(_id);
            if (PrototypeTraitSystem::_data->_reuseVehicleChasisCbFn) {
                PrototypeTraitSystem::_data->_reuseVehicleChasisCbFn(
                  this, PrototypeTraitSystem::_data->_vehicleChasisVector[PrototypeTraitSystem::_data->_vehicleChasisMap[_id]]);
            }
        }
    }
}

void
PrototypeObject::removeVehicleChasisTrait()
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskVehicleChasis)) {
        if (PrototypeTraitSystem::_data->_removeVehicleChasisCbFn) {
            PrototypeTraitSystem::_data->_removeVehicleChasisCbFn(this, this->getVehicleChasisTrait());
        }
        PrototypeBitflagRemove(_traits, PrototypeTraitTypeMaskVehicleChasis);
        PrototypeTraitSystem::_data->_vehicleChasisGarbageMap[_id] = PrototypeTraitSystem::_data->_vehicleChasisMap[_id];
        PrototypeTraitSystem::_data->_vehicleChasisMap.erase(_id);
    }
}

void
PrototypeObject::garbageCollectVehicleChasisTraitMemory()
{
    for (auto pair : PrototypeTraitSystem::_data->_vehicleChasisGarbageMap) {
        for (auto& pair2 : PrototypeTraitSystem::_data->_vehicleChasisMap) {
            if (pair2.second > pair.second) { --pair2.second; }
        }
        PrototypeTraitSystem::_data->_vehicleChasisPool.deleteElement(
          PrototypeTraitSystem::_data->_vehicleChasisVector[pair.second]);
        PrototypeTraitSystem::_data->_vehicleChasisVector.erase(PrototypeTraitSystem::_data->_vehicleChasisVector.begin() +
                                                                pair.second);
    }
    PrototypeTraitSystem::_data->_vehicleChasisGarbageMap.clear();
}

bool
PrototypeObject::hasVehicleChasisTrait() const
{
    return PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskVehicleChasis);
}

VehicleChasis*
PrototypeObject::getVehicleChasisTrait() const
{
    if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMaskVehicleChasis)) {
        return PrototypeTraitSystem::_data->_vehicleChasisVector[PrototypeTraitSystem::_data->_vehicleChasisMap[_id]];
    }
    return nullptr;
}

void
PrototypeObject::to_json(nlohmann::json& j, const PrototypeObject& o)
{
    {
        if (o.hasCameraTrait()) {
            nlohmann::json jj;
            Camera::to_json(jj, (const Camera) * (o.getCameraTrait()));
            j.push_back(jj);
        }
    }
    {
        if (o.hasColliderTrait()) {
            nlohmann::json jj;
            Collider::to_json(jj, (const Collider) * (o.getColliderTrait()));
            j.push_back(jj);
        }
    }
    {
        if (o.hasMeshRendererTrait()) {
            nlohmann::json jj;
            MeshRenderer::to_json(jj, (const MeshRenderer) * (o.getMeshRendererTrait()));
            j.push_back(jj);
        }
    }
    {
        if (o.hasRigidbodyTrait()) {
            nlohmann::json jj;
            Rigidbody::to_json(jj, (const Rigidbody) * (o.getRigidbodyTrait()));
            j.push_back(jj);
        }
    }
    {
        if (o.hasScriptTrait()) {
            nlohmann::json jj;
            Script::to_json(jj, (const Script) * (o.getScriptTrait()));
            j.push_back(jj);
        }
    }
    {
        if (o.hasTransformTrait()) {
            nlohmann::json jj;
            Transform::to_json(jj, (const Transform) * (o.getTransformTrait()));
            j.push_back(jj);
        }
    }
    {
        if (o.hasVehicleChasisTrait()) {
            nlohmann::json jj;
            VehicleChasis::to_json(jj, (const VehicleChasis) * (o.getVehicleChasisTrait()));
            j.push_back(jj);
        }
    }
}

void
PrototypeObject::from_json(const nlohmann::json& j, PrototypeObject& o)
{
    const char* field_name = "name";
    if (j.at(field_name) == "Camera") {
        o.addCameraTrait();
        Camera::from_json(j, *(o.getCameraTrait()), &o);
        return;
    }
    if (j.at(field_name) == "Collider") {
        o.addColliderTrait();
        Collider::from_json(j, *(o.getColliderTrait()), &o);
        return;
    }
    if (j.at(field_name) == "MeshRenderer") {
        o.addMeshRendererTrait();
        MeshRenderer::from_json(j, *(o.getMeshRendererTrait()), &o);
        return;
    }
    if (j.at(field_name) == "Rigidbody") {
        o.addRigidbodyTrait();
        Rigidbody::from_json(j, *(o.getRigidbodyTrait()), &o);
        return;
    }
    if (j.at(field_name) == "Script") {
        o.addScriptTrait();
        Script::from_json(j, *(o.getScriptTrait()), &o);
        return;
    }
    if (j.at(field_name) == "Transform") {
        o.addTransformTrait();
        Transform::from_json(j, *(o.getTransformTrait()), &o);
        return;
    }
    if (j.at(field_name) == "VehicleChasis") {
        o.addVehicleChasisTrait();
        VehicleChasis::from_json(j, *(o.getVehicleChasisTrait()), &o);
        return;
    }
}

// TraitSystem
PrototypeObject*
PrototypeTraitSystem::createObject()
{
    PrototypeObject* et = PrototypeTraitSystem::_data->_pool.newElement();
    PrototypeTraitSystem::_data->_objects.push_back(et);
    return et;
}

void
PrototypeTraitSystem::clearObjects()
{
    for (auto it = PrototypeTraitSystem::_data->_objects.begin(); it != PrototypeTraitSystem::_data->_objects.end(); ++it) {
        PrototypeTraitSystem::_data->_pool.deleteElement(*it);
    }
    PrototypeTraitSystem::_data->_objects.clear();
}

const std::vector<PrototypeObject*>&
PrototypeTraitSystem::objects()
{
    return PrototypeTraitSystem::_data->_objects;
}

const std::vector<Camera*>&
PrototypeTraitSystem::cameraVector()
{
    return PrototypeTraitSystem::_data->_cameraVector;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::cameraMap()
{
    return PrototypeTraitSystem::_data->_cameraMap;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::cameraGarbageMap()
{
    return PrototypeTraitSystem::_data->_cameraGarbageMap;
}

void
PrototypeTraitSystem::setCameraTraitAddCbFnPtr(addCameraFn cbfn)
{
    PrototypeTraitSystem::_data->_addCameraCbFn = cbfn;
}

void
PrototypeTraitSystem::setCameraTraitReuseCbFnPtr(reuseCameraFn cbfn)
{
    PrototypeTraitSystem::_data->_reuseCameraCbFn = cbfn;
}

void
PrototypeTraitSystem::setCameraTraitRemoveCbFnPtr(removeCameraFn cbfn)
{
    PrototypeTraitSystem::_data->_removeCameraCbFn = cbfn;
}
void
PrototypeTraitSystem::setCameraTraitLogCbFnPtr(logCameraFn cbfn)
{
    PrototypeTraitSystem::_data->_logCameraCbFn = cbfn;
}
const std::vector<Collider*>&
PrototypeTraitSystem::colliderVector()
{
    return PrototypeTraitSystem::_data->_colliderVector;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::colliderMap()
{
    return PrototypeTraitSystem::_data->_colliderMap;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::colliderGarbageMap()
{
    return PrototypeTraitSystem::_data->_colliderGarbageMap;
}

void
PrototypeTraitSystem::setColliderTraitAddCbFnPtr(addColliderFn cbfn)
{
    PrototypeTraitSystem::_data->_addColliderCbFn = cbfn;
}

void
PrototypeTraitSystem::setColliderTraitReuseCbFnPtr(reuseColliderFn cbfn)
{
    PrototypeTraitSystem::_data->_reuseColliderCbFn = cbfn;
}

void
PrototypeTraitSystem::setColliderTraitRemoveCbFnPtr(removeColliderFn cbfn)
{
    PrototypeTraitSystem::_data->_removeColliderCbFn = cbfn;
}
void
PrototypeTraitSystem::setColliderTraitLogCbFnPtr(logColliderFn cbfn)
{
    PrototypeTraitSystem::_data->_logColliderCbFn = cbfn;
}
const std::vector<MeshRenderer*>&
PrototypeTraitSystem::meshRendererVector()
{
    return PrototypeTraitSystem::_data->_meshRendererVector;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::meshRendererMap()
{
    return PrototypeTraitSystem::_data->_meshRendererMap;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::meshRendererGarbageMap()
{
    return PrototypeTraitSystem::_data->_meshRendererGarbageMap;
}

void
PrototypeTraitSystem::setMeshRendererTraitAddCbFnPtr(addMeshRendererFn cbfn)
{
    PrototypeTraitSystem::_data->_addMeshRendererCbFn = cbfn;
}

void
PrototypeTraitSystem::setMeshRendererTraitReuseCbFnPtr(reuseMeshRendererFn cbfn)
{
    PrototypeTraitSystem::_data->_reuseMeshRendererCbFn = cbfn;
}

void
PrototypeTraitSystem::setMeshRendererTraitRemoveCbFnPtr(removeMeshRendererFn cbfn)
{
    PrototypeTraitSystem::_data->_removeMeshRendererCbFn = cbfn;
}
void
PrototypeTraitSystem::setMeshRendererTraitLogCbFnPtr(logMeshRendererFn cbfn)
{
    PrototypeTraitSystem::_data->_logMeshRendererCbFn = cbfn;
}
const std::vector<Rigidbody*>&
PrototypeTraitSystem::rigidbodyVector()
{
    return PrototypeTraitSystem::_data->_rigidbodyVector;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::rigidbodyMap()
{
    return PrototypeTraitSystem::_data->_rigidbodyMap;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::rigidbodyGarbageMap()
{
    return PrototypeTraitSystem::_data->_rigidbodyGarbageMap;
}

void
PrototypeTraitSystem::setRigidbodyTraitAddCbFnPtr(addRigidbodyFn cbfn)
{
    PrototypeTraitSystem::_data->_addRigidbodyCbFn = cbfn;
}

void
PrototypeTraitSystem::setRigidbodyTraitReuseCbFnPtr(reuseRigidbodyFn cbfn)
{
    PrototypeTraitSystem::_data->_reuseRigidbodyCbFn = cbfn;
}

void
PrototypeTraitSystem::setRigidbodyTraitRemoveCbFnPtr(removeRigidbodyFn cbfn)
{
    PrototypeTraitSystem::_data->_removeRigidbodyCbFn = cbfn;
}
void
PrototypeTraitSystem::setRigidbodyTraitLogCbFnPtr(logRigidbodyFn cbfn)
{
    PrototypeTraitSystem::_data->_logRigidbodyCbFn = cbfn;
}
const std::vector<Script*>&
PrototypeTraitSystem::scriptVector()
{
    return PrototypeTraitSystem::_data->_scriptVector;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::scriptMap()
{
    return PrototypeTraitSystem::_data->_scriptMap;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::scriptGarbageMap()
{
    return PrototypeTraitSystem::_data->_scriptGarbageMap;
}

void
PrototypeTraitSystem::setScriptTraitAddCbFnPtr(addScriptFn cbfn)
{
    PrototypeTraitSystem::_data->_addScriptCbFn = cbfn;
}

void
PrototypeTraitSystem::setScriptTraitReuseCbFnPtr(reuseScriptFn cbfn)
{
    PrototypeTraitSystem::_data->_reuseScriptCbFn = cbfn;
}

void
PrototypeTraitSystem::setScriptTraitRemoveCbFnPtr(removeScriptFn cbfn)
{
    PrototypeTraitSystem::_data->_removeScriptCbFn = cbfn;
}
void
PrototypeTraitSystem::setScriptTraitLogCbFnPtr(logScriptFn cbfn)
{
    PrototypeTraitSystem::_data->_logScriptCbFn = cbfn;
}
const std::vector<Transform*>&
PrototypeTraitSystem::transformVector()
{
    return PrototypeTraitSystem::_data->_transformVector;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::transformMap()
{
    return PrototypeTraitSystem::_data->_transformMap;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::transformGarbageMap()
{
    return PrototypeTraitSystem::_data->_transformGarbageMap;
}

void
PrototypeTraitSystem::setTransformTraitAddCbFnPtr(addTransformFn cbfn)
{
    PrototypeTraitSystem::_data->_addTransformCbFn = cbfn;
}

void
PrototypeTraitSystem::setTransformTraitReuseCbFnPtr(reuseTransformFn cbfn)
{
    PrototypeTraitSystem::_data->_reuseTransformCbFn = cbfn;
}

void
PrototypeTraitSystem::setTransformTraitRemoveCbFnPtr(removeTransformFn cbfn)
{
    PrototypeTraitSystem::_data->_removeTransformCbFn = cbfn;
}
void
PrototypeTraitSystem::setTransformTraitLogCbFnPtr(logTransformFn cbfn)
{
    PrototypeTraitSystem::_data->_logTransformCbFn = cbfn;
}
const std::vector<VehicleChasis*>&
PrototypeTraitSystem::vehicleChasisVector()
{
    return PrototypeTraitSystem::_data->_vehicleChasisVector;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::vehicleChasisMap()
{
    return PrototypeTraitSystem::_data->_vehicleChasisMap;
}

const std::unordered_map<u32, size_t>&
PrototypeTraitSystem::vehicleChasisGarbageMap()
{
    return PrototypeTraitSystem::_data->_vehicleChasisGarbageMap;
}

void
PrototypeTraitSystem::setVehicleChasisTraitAddCbFnPtr(addVehicleChasisFn cbfn)
{
    PrototypeTraitSystem::_data->_addVehicleChasisCbFn = cbfn;
}

void
PrototypeTraitSystem::setVehicleChasisTraitReuseCbFnPtr(reuseVehicleChasisFn cbfn)
{
    PrototypeTraitSystem::_data->_reuseVehicleChasisCbFn = cbfn;
}

void
PrototypeTraitSystem::setVehicleChasisTraitRemoveCbFnPtr(removeVehicleChasisFn cbfn)
{
    PrototypeTraitSystem::_data->_removeVehicleChasisCbFn = cbfn;
}
void
PrototypeTraitSystem::setVehicleChasisTraitLogCbFnPtr(logVehicleChasisFn cbfn)
{
    PrototypeTraitSystem::_data->_logVehicleChasisCbFn = cbfn;
}

extern void**
PrototypeTraitSystemGetDataInternal()
{
    return &((void*)PrototypeTraitSystem::_data);
}

extern void
PrototypeTraitSystemSetDataInternal(void** data)
{
    PrototypeTraitSystem::_data = (PrototypeTraitSystemData*)(*data);
}

extern bool
PrototypeTraitSystemInit()
{
    PrototypeTraitSystem::_data = PROTOTYPE_NEW PrototypeTraitSystemData();
    return true;
}

extern "C" void**
PrototypeTraitSystemGetData()
{
    return PrototypeTraitSystemGetDataInternal();
}

extern "C" void
PrototypeTraitSystemSetData(void** data)
{
    PrototypeTraitSystemSetDataInternal(data);
}

extern void
PrototypeTraitSystemDeinit()
{
    delete PrototypeTraitSystem::_data;
}