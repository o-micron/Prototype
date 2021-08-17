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

#include "../include/PrototypeTraitSystem/Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

onEditDispatchHandlerFn Transform::_onEditDispatchHandler = nullptr;

void
Transform::setModel(const glm::mat4& model)
{
    _model       = model;
    _modelScaled = _model;
    PrototypeMaths::buildModelMatrixWithScale(_modelScaled, _scale);
    // updateComponentsFromMatrix();
}

void
Transform::setModel(const float* model)
{
    _model       = glm::make_mat4(model);
    _modelScaled = _model;
    PrototypeMaths::buildModelMatrixWithScale(_modelScaled, _scale);
    // updateComponentsFromMatrix();
}

void
Transform::setModelScaled(const float* modelScaled)
{
    _modelScaled = glm::make_mat4(modelScaled);
    _model       = _modelScaled;
    PrototypeMaths::buildModelMatrixWithScale(_model, { 1.0f / _scale.x, 1.0f / _scale.y, 1.0f / _scale.z });
}

void
Transform::updateComponentsFromMatrix()
{
    _needsComponentsSync = true;
}

const glm::vec3&
Transform::position()
{
    if (_needsComponentsSync) {
        _needsComponentsSync = false;
        glm::quat orientation;
        glm::vec3 skew, position, scale;
        glm::vec4 perspective;
        glm::decompose(_modelScaled, scale, orientation, position, skew, perspective);
        glm::vec3 rotation     = glm::degrees(glm::eulerAngles(orientation));
        _position              = { position.x, position.y, position.z };
        _rotation              = { rotation.x, rotation.y, rotation.z };
        _scale                 = { scale.x, scale.y, scale.z };
        constexpr f32 clampMin = 0.001f;
        if (_scale.x < clampMin || _scale.y < clampMin || _scale.z < clampMin) {
            _scale.x     = _scale.x < clampMin ? clampMin : _scale.x;
            _scale.y     = _scale.y < clampMin ? clampMin : _scale.y;
            _scale.z     = _scale.z < clampMin ? clampMin : _scale.z;
            _modelScaled = _model;
            PrototypeMaths::buildModelMatrixWithScale(_modelScaled, _scale);
        }
    }
    return _position;
}

glm::vec3&
Transform::positionMut()
{
    if (_needsComponentsSync) {
        _needsComponentsSync = false;
        glm::quat orientation;
        glm::vec3 skew, position, scale;
        glm::vec4 perspective;
        glm::decompose(_modelScaled, scale, orientation, position, skew, perspective);
        glm::vec3 rotation     = glm::degrees(glm::eulerAngles(orientation));
        _position              = { position.x, position.y, position.z };
        _rotation              = { rotation.x, rotation.y, rotation.z };
        _scale                 = { scale.x, scale.y, scale.z };
        constexpr f32 clampMin = 0.001f;
        if (_scale.x < clampMin || _scale.y < clampMin || _scale.z < clampMin) {
            _scale.x     = _scale.x < clampMin ? clampMin : _scale.x;
            _scale.y     = _scale.y < clampMin ? clampMin : _scale.y;
            _scale.z     = _scale.z < clampMin ? clampMin : _scale.z;
            _modelScaled = _model;
            PrototypeMaths::buildModelMatrixWithScale(_modelScaled, _scale);
        }
    }
    return _position;
}

const glm::vec3&
Transform::rotation()
{
    if (_needsComponentsSync) {
        _needsComponentsSync = false;
        glm::quat orientation;
        glm::vec3 skew, position, scale;
        glm::vec4 perspective;
        glm::decompose(_modelScaled, scale, orientation, position, skew, perspective);
        glm::vec3 rotation     = glm::degrees(glm::eulerAngles(orientation));
        _position              = { position.x, position.y, position.z };
        _rotation              = { rotation.x, rotation.y, rotation.z };
        _scale                 = { scale.x, scale.y, scale.z };
        constexpr f32 clampMin = 0.001f;
        if (_scale.x < clampMin || _scale.y < clampMin || _scale.z < clampMin) {
            _scale.x     = _scale.x < clampMin ? clampMin : _scale.x;
            _scale.y     = _scale.y < clampMin ? clampMin : _scale.y;
            _scale.z     = _scale.z < clampMin ? clampMin : _scale.z;
            _modelScaled = _model;
            PrototypeMaths::buildModelMatrixWithScale(_modelScaled, _scale);
        }
    }
    return _rotation;
}

glm::vec3&
Transform::rotationMut()
{
    if (_needsComponentsSync) {
        _needsComponentsSync = false;
        glm::quat orientation;
        glm::vec3 skew, position, scale;
        glm::vec4 perspective;
        glm::decompose(_modelScaled, scale, orientation, position, skew, perspective);
        glm::vec3 rotation     = glm::degrees(glm::eulerAngles(orientation));
        _position              = { position.x, position.y, position.z };
        _rotation              = { rotation.x, rotation.y, rotation.z };
        _scale                 = { scale.x, scale.y, scale.z };
        constexpr f32 clampMin = 0.001f;
        if (_scale.x < clampMin || _scale.y < clampMin || _scale.z < clampMin) {
            _scale.x     = _scale.x < clampMin ? clampMin : _scale.x;
            _scale.y     = _scale.y < clampMin ? clampMin : _scale.y;
            _scale.z     = _scale.z < clampMin ? clampMin : _scale.z;
            _modelScaled = _model;
            PrototypeMaths::buildModelMatrixWithScale(_modelScaled, _scale);
        }
    }
    return _rotation;
}

const glm::vec3&
Transform::scale()
{
    if (_needsComponentsSync) {
        _needsComponentsSync = false;
        glm::quat orientation;
        glm::vec3 skew, position, scale;
        glm::vec4 perspective;
        glm::decompose(_modelScaled, scale, orientation, position, skew, perspective);
        glm::vec3 rotation     = glm::degrees(glm::eulerAngles(orientation));
        _position              = { position.x, position.y, position.z };
        _rotation              = { rotation.x, rotation.y, rotation.z };
        _scale                 = { scale.x, scale.y, scale.z };
        constexpr f32 clampMin = 0.001f;
        if (_scale.x < clampMin || _scale.y < clampMin || _scale.z < clampMin) {
            _scale.x     = _scale.x < clampMin ? clampMin : _scale.x;
            _scale.y     = _scale.y < clampMin ? clampMin : _scale.y;
            _scale.z     = _scale.z < clampMin ? clampMin : _scale.z;
            _modelScaled = _model;
            PrototypeMaths::buildModelMatrixWithScale(_modelScaled, _scale);
        }
    }
    return _scale;
}

glm::vec3&
Transform::scaleMut()
{
    if (_needsComponentsSync) {
        _needsComponentsSync = false;
        glm::quat orientation;
        glm::vec3 skew, position, scale;
        glm::vec4 perspective;
        glm::decompose(_modelScaled, scale, orientation, position, skew, perspective);
        glm::vec3 rotation     = glm::degrees(glm::eulerAngles(orientation));
        _position              = { position.x, position.y, position.z };
        _rotation              = { rotation.x, rotation.y, rotation.z };
        _scale                 = { scale.x, scale.y, scale.z };
        constexpr f32 clampMin = 0.001f;
        if (_scale.x < clampMin || _scale.y < clampMin || _scale.z < clampMin) {
            _scale.x     = _scale.x < clampMin ? clampMin : _scale.x;
            _scale.y     = _scale.y < clampMin ? clampMin : _scale.y;
            _scale.z     = _scale.z < clampMin ? clampMin : _scale.z;
            _modelScaled = _model;
            PrototypeMaths::buildModelMatrixWithScale(_modelScaled, _scale);
        }
    }
    return _scale;
}

const glm::mat4&
Transform::model() const
{
    return _model;
}

glm::mat4&
Transform::modelMut()
{
    return _model;
}

const glm::mat4&
Transform::modelScaled() const
{
    return _modelScaled;
}

const bool&
Transform::needsPhysicsSync() const
{
    return _needsPhysicsSync;
}

void
Transform::setNeedsPhysicsSync(bool needsPhysicsSync)
{
    _needsPhysicsSync = needsPhysicsSync;
}

PrototypeObject*
Transform::object()
{
    return _object;
}

void
Transform::setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler)
{
    _onEditDispatchHandler = onEditDispatchHandler;
}

void
Transform::onEditDispatch(PrototypeObject* o)
{
    if (_onEditDispatchHandler) { _onEditDispatchHandler(o); }
}

void
Transform::to_json(nlohmann::json& j, const Transform& t)
{
    const char* field_name     = "name";
    const char* field_position = "position";
    const char* field_rotation = "rotation";
    const char* field_scale    = "scale";

    j[field_name]     = PROTOTYPE_STRINGIFY(Transform);
    j[field_position] = t._position;
    j[field_rotation] = t._rotation;
    j[field_scale]    = t._scale;
}

void
Transform::from_json(const nlohmann::json& j, Transform& t, PrototypeObject* o)
{
    const char* field_position = "position";
    const char* field_rotation = "rotation";
    const char* field_scale    = "scale";

    t._position = j.at(field_position);
    t._rotation = j.at(field_rotation);
    t._scale    = j.at(field_scale);

    PrototypeMaths::buildModelMatrix(t._model, t._position, t._rotation);
    t._modelScaled = t._model;
    PrototypeMaths::buildModelMatrixWithScale(t._modelScaled, t._scale);

    t._object              = o;
    t._needsPhysicsSync    = true;
    t._needsComponentsSync = false;
}