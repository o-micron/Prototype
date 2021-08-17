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

#include "../include/PrototypeTraitSystem/Rigidbody.h"

onEditDispatchHandlerFn Rigidbody::_onEditDispatchHandler = nullptr;

void
Rigidbody::setRigidbodyRef(void* rigidbodyRef)
{
    _rigidbodyRef = rigidbodyRef;
}

void
Rigidbody::setLinearVelocity(glm::vec3 linearVelocity)
{
    _linearVelocity = linearVelocity;
}

void
Rigidbody::setLinearDamping(f32 linearDamping)
{
    _linearDamping = linearDamping;
}

void
Rigidbody::setAngularVelocity(glm::vec3 angularVelocity)
{
    _angularVelocity = angularVelocity;
}

void
Rigidbody::setAngularDamping(f32 angularDamping)
{
    _angularDamping = angularDamping;
}

void
Rigidbody::setMass(f32 mass)
{
    _mass = mass;
}

void
Rigidbody::setLockLinearX(bool lock)
{
    _lockLinearX = lock;
}

void
Rigidbody::setLockLinearY(bool lock)
{
    _lockLinearY = lock;
}

void
Rigidbody::setLockLinearZ(bool lock)
{
    _lockLinearZ = lock;
}

void
Rigidbody::setLockAngularX(bool lock)
{
    _lockAngularX = lock;
}

void
Rigidbody::setLockAngularY(bool lock)
{
    _lockAngularY = lock;
}

void
Rigidbody::setLockAngularZ(bool lock)
{
    _lockAngularZ = lock;
}

void
Rigidbody::setStatic(bool value)
{
    _static = value;
}

void
Rigidbody::setTrigger(bool value)
{
    _trigger = value;
}

void*
Rigidbody::rigidbodyRef()
{
    return _rigidbodyRef;
}

const glm::vec3&
Rigidbody::linearVelocity() const
{
    return _linearVelocity;
}

glm::vec3&
Rigidbody::linearVelocityMut()
{
    return _linearVelocity;
}

const f32&
Rigidbody::linearDamping() const
{
    return _linearDamping;
}

f32&
Rigidbody::linearDampingMut()
{
    return _linearDamping;
}

const glm::vec3&
Rigidbody::angularVelocity() const
{
    return _angularVelocity;
}

glm::vec3&
Rigidbody::angularVelocityMut()
{
    return _angularVelocity;
}

const f32&
Rigidbody::angularDamping() const
{
    return _angularDamping;
}

f32&
Rigidbody::angularDampingMut()
{
    return _angularDamping;
}

const f32&
Rigidbody::mass() const
{
    return _mass;
}

f32&
Rigidbody::massMut()
{
    return _mass;
}

const bool&
Rigidbody::lockLinearX() const
{
    return _lockLinearX;
}

bool&
Rigidbody::lockLinearXMut()
{
    return _lockLinearX;
}

const bool&
Rigidbody::lockLinearY() const
{
    return _lockLinearY;
}

bool&
Rigidbody::lockLinearYMut()
{
    return _lockLinearY;
}

const bool&
Rigidbody::lockLinearZ() const
{
    return _lockLinearZ;
}

bool&
Rigidbody::lockLinearZMut()
{
    return _lockLinearZ;
}

const bool&
Rigidbody::lockAngularX() const
{
    return _lockAngularX;
}

bool&
Rigidbody::lockAngularXMut()
{
    return _lockAngularX;
}

const bool&
Rigidbody::lockAngularY() const
{
    return _lockAngularY;
}

bool&
Rigidbody::lockAngularYMut()
{
    return _lockAngularY;
}

const bool&
Rigidbody::lockAngularZ() const
{
    return _lockAngularZ;
}

bool&
Rigidbody::lockAngularZMut()
{
    return _lockAngularZ;
}

const bool&
Rigidbody::isStatic() const
{
    return _static;
}

const bool&
Rigidbody::isTrigger() const
{
    return _trigger;
}

PrototypeObject*
Rigidbody::object()
{
    return _object;
}

void
Rigidbody::setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler)
{
    _onEditDispatchHandler = onEditDispatchHandler;
}

void
Rigidbody::onEditDispatch(PrototypeObject* o)
{
    if (_onEditDispatchHandler) { _onEditDispatchHandler(o); }
}

void
Rigidbody::to_json(nlohmann::json& j, const Rigidbody& rb)
{
    const char* field_name             = "name";
    const char* field_linear_velocity  = "linearVelocity";
    const char* field_linear_damping   = "linearDamping";
    const char* field_angular_velocity = "angularVelocity";
    const char* field_angular_damping  = "angularDamping";
    const char* field_mass             = "mass";
    const char* field_lock_linear_x    = "lockLinearX";
    const char* field_lock_linear_y    = "lockLinearY";
    const char* field_lock_linear_z    = "lockLinearZ";
    const char* field_lock_angular_x   = "lockAngularX";
    const char* field_lock_angular_y   = "lockAngularY";
    const char* field_lock_angular_z   = "lockAngularZ";
    const char* field_static           = "static";
    const char* field_trigger          = "trigger";

    j[field_name]             = PROTOTYPE_STRINGIFY(Rigidbody);
    j[field_linear_velocity]  = rb._linearVelocity;
    j[field_linear_damping]   = rb._linearDamping;
    j[field_angular_velocity] = rb._angularVelocity;
    j[field_angular_damping]  = rb._angularDamping;
    j[field_mass]             = rb._mass;
    j[field_lock_linear_x]    = rb._lockLinearX;
    j[field_lock_linear_y]    = rb._lockLinearY;
    j[field_lock_linear_z]    = rb._lockLinearZ;
    j[field_lock_angular_x]   = rb._lockAngularX;
    j[field_lock_angular_y]   = rb._lockAngularY;
    j[field_lock_angular_z]   = rb._lockAngularZ;
    j[field_static]           = rb._static;
    j[field_trigger]          = rb._trigger;
}

void
Rigidbody::from_json(const nlohmann::json& j, Rigidbody& rb, PrototypeObject* o)
{
    const char* field_linear_velocity  = "linearVelocity";
    const char* field_linear_damping   = "linearDamping";
    const char* field_angular_velocity = "angularVelocity";
    const char* field_angular_damping  = "angularDamping";
    const char* field_mass             = "mass";
    const char* field_lock_linear_x    = "lockLinearX";
    const char* field_lock_linear_y    = "lockLinearY";
    const char* field_lock_linear_z    = "lockLinearZ";
    const char* field_lock_angular_x   = "lockAngularX";
    const char* field_lock_angular_y   = "lockAngularY";
    const char* field_lock_angular_z   = "lockAngularZ";
    const char* field_static           = "static";
    const char* field_trigger          = "trigger";

    if (j.find(field_linear_velocity) != j.end()) {
        rb._linearVelocity = j.at(field_linear_velocity);
    } else {
        rb._linearVelocity = { 0.0f, 0.0f, 0.0f };
    }
    if (j.find(field_linear_damping) != j.end()) {
        j.at(field_linear_damping).get_to(rb._linearDamping);
    } else {
        rb._linearDamping = 1.0f;
    }
    if (j.find(field_angular_velocity) != j.end()) {
        rb._angularVelocity = j.at(field_angular_velocity);
    } else {
        rb._angularVelocity = { 0.0f, 0.0f, 0.0f };
    }
    if (j.find(field_angular_damping) != j.end()) {
        j.at(field_angular_damping).get_to(rb._angularDamping);
    } else {
        rb._angularDamping = 1.0f;
    }
    if (j.find(field_mass) != j.end()) {
        j.at(field_mass).get_to(rb._mass);
    } else {
        rb._mass = 1.0f;
    }
    if (j.find(field_static) != j.end()) {
        rb._static = j.at(field_static).get<bool>();
    } else {
        rb._static = false;
    }
    if (j.find(field_trigger) != j.end()) {
        rb._trigger = j.at(field_trigger).get<bool>();
    } else {
        rb._trigger = false;
    }
    if (j.find(field_lock_linear_x) != j.end()) {
        j.at(field_lock_linear_x).get_to(rb._lockLinearX);
    } else {
        rb._lockLinearX = false;
    }
    if (j.find(field_lock_linear_y) != j.end()) {
        j.at(field_lock_linear_y).get_to(rb._lockLinearY);
    } else {
        rb._lockLinearY = false;
    }
    if (j.find(field_lock_linear_z) != j.end()) {
        j.at(field_lock_linear_z).get_to(rb._lockLinearZ);
    } else {
        rb._lockLinearZ = false;
    }
    if (j.find(field_lock_angular_x) != j.end()) {
        j.at(field_lock_angular_x).get_to(rb._lockAngularX);
    } else {
        rb._lockAngularX = false;
    }
    if (j.find(field_lock_angular_y) != j.end()) {
        j.at(field_lock_angular_y).get_to(rb._lockAngularY);
    } else {
        rb._lockAngularY = false;
    }
    if (j.find(field_lock_angular_z) != j.end()) {
        j.at(field_lock_angular_z).get_to(rb._lockAngularZ);
    } else {
        rb._lockAngularZ = false;
    }

    rb._object = o;
}