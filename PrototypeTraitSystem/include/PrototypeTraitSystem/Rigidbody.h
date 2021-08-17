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

#include <PrototypeCommon/Definitions.h>
#include <PrototypeCommon/Maths.h>

#include <nlohmann/json.hpp>

struct PrototypeObject;

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onEditDispatchHandlerFn)(PrototypeObject* o);

struct Attachable(Trait) Rigidbody
{
    void setRigidbodyRef(void* rigidbodyRef);
    void setLinearVelocity(glm::vec3 linearVelocity);
    void setLinearDamping(f32 linearDamping);
    void setAngularVelocity(glm::vec3 angularVelocity);
    void setAngularDamping(f32 angularDamping);
    void setMass(f32 mass);
    void setLockLinearX(bool lock);
    void setLockLinearY(bool lock);
    void setLockLinearZ(bool lock);
    void setLockAngularX(bool lock);
    void setLockAngularY(bool lock);
    void setLockAngularZ(bool lock);
    void setStatic(bool value);
    void setTrigger(bool value);

    void*            rigidbodyRef();
    const glm::vec3& linearVelocity() const;
    glm::vec3&       linearVelocityMut();
    const f32&       linearDamping() const;
    f32&             linearDampingMut();
    const glm::vec3& angularVelocity() const;
    glm::vec3&       angularVelocityMut();
    const f32&       angularDamping() const;
    f32&             angularDampingMut();
    const f32&       mass() const;
    f32&             massMut();
    const bool&      lockLinearX() const;
    bool&            lockLinearXMut();
    const bool&      lockLinearY() const;
    bool&            lockLinearYMut();
    const bool&      lockLinearZ() const;
    bool&            lockLinearZMut();
    const bool&      lockAngularX() const;
    bool&            lockAngularXMut();
    const bool&      lockAngularY() const;
    bool&            lockAngularYMut();
    const bool&      lockAngularZ() const;
    bool&            lockAngularZMut();
    const bool&      isStatic() const;
    const bool&      isTrigger() const;

    PrototypeObject* object();
    static void      setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler);
    static void      onEditDispatch(PrototypeObject * o);
    static void      to_json(nlohmann::json & j, const Rigidbody& rb);
    static void      from_json(const nlohmann::json& j, Rigidbody& rb, PrototypeObject* o);

  private:
    friend struct PrototypeObject;
    PrototypeObject*               _object;
    static onEditDispatchHandlerFn _onEditDispatchHandler;
    void*                          _rigidbodyRef;
    glm::vec3                      _linearVelocity;
    f32                            _linearDamping;
    glm::vec3                      _angularVelocity;
    f32                            _angularDamping;
    f32                            _mass;
    bool                           _lockLinearX;
    bool                           _lockLinearY;
    bool                           _lockLinearZ;
    bool                           _lockAngularX;
    bool                           _lockAngularY;
    bool                           _lockAngularZ;
    bool                           _static;
    bool                           _trigger;
};
