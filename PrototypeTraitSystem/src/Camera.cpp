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

#include "../include/PrototypeTraitSystem/Camera.h"

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

onEditDispatchHandlerFn Camera::_onEditDispatchHandler = nullptr;

// free
const f32&
Camera::lookSensitivity() const
{
    return _lookSensitivity;
}

f32&
Camera::lookSensitivityMut()
{
    return _lookSensitivity;
}

const f32&
Camera::moveSensitivity() const
{
    return _moveSensitivity;
}

f32&
Camera::moveSensitivityMut()
{
    return _moveSensitivity;
}

const glm::vec2&
Camera::rotation() const
{
    return _rotation;
}

const glm::quat&
Camera::rotationQuat() const
{
    return _rotationQuat;
}

const glm::vec3&
Camera::position() const
{
    return _position;
}

const bool&
Camera::orbital() const
{
    return _orbital;
}

bool&
Camera::orbitalMut()
{
    return _orbital;
}

const glm::mat4&
Camera::viewMatrix() const
{
    return _viewMatrix;
}

// perspective
const f32&
Camera::fov() const
{
    return _fov;
}

f32&
Camera::fovMut()
{
    return _fov;
}

const f32&
Camera::aspectRatio() const
{
    return _aspectRatio;
}

const f32&
Camera::znear() const
{
    return _near;
}

f32&
Camera::nearMut()
{
    return _near;
}

const f32&
Camera::zfar() const
{
    return _far;
}

f32&
Camera::farMut()
{
    return _far;
}

const glm::vec2&
Camera::resolution() const
{
    return _resolution;
}

const glm::mat4&
Camera::projectionMatrix() const
{
    return _projectionMatrix;
}

PrototypeObject*
Camera::object()
{
    return _object;
}

void
Camera::setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler)
{
    _onEditDispatchHandler = onEditDispatchHandler;
}

void
Camera::onEditDispatch(PrototypeObject* o)
{
    if (_onEditDispatchHandler) { _onEditDispatchHandler(o); }
}

void
Camera::to_json(nlohmann::json& j, const Camera& c)
{
    const char* field_name            = "name";
    const char* field_fov             = "fov";
    const char* field_aspectRatio     = "aspectRatio";
    const char* field_near            = "near";
    const char* field_far             = "far";
    const char* field_lookSensitivity = "lookSensitivity";
    const char* field_moveSensitivity = "moveSensitivity";
    const char* field_position        = "position";
    const char* field_rotation        = "rotation";
    const char* field_orbital         = "orbital";

    j[field_name]            = PROTOTYPE_STRINGIFY(Camera);
    j[field_fov]             = c._fov;
    j[field_near]            = c._near;
    j[field_far]             = c._far;
    j[field_lookSensitivity] = c._lookSensitivity;
    j[field_moveSensitivity] = c._moveSensitivity;
    j[field_position]        = c.position();
    j[field_rotation]        = c.rotation();
    j[field_orbital]         = c.orbital();
}

void
Camera::from_json(const nlohmann::json& j, Camera& c, PrototypeObject* o)
{
    const char* field_fov             = "fov";
    const char* field_near            = "near";
    const char* field_far             = "far";
    const char* field_lookSensitivity = "lookSensitivity";
    const char* field_moveSensitivity = "moveSensitivity";
    const char* field_position        = "position";
    const char* field_rotation        = "rotation";
    const char* field_orbital         = "orbital";

    j.at(field_fov).get_to(c._fov);
    j.at(field_near).get_to(c._near);
    j.at(field_far).get_to(c._far);

    j.at(field_lookSensitivity).get_to(c._lookSensitivity);
    j.at(field_moveSensitivity).get_to(c._moveSensitivity);
    j.at(field_position).get_to(c._position);
    j.at(field_rotation).get_to(c._rotation);
    j.at(field_orbital).get_to(c._orbital);

    c._interpolationTime = 0.0f;
    c._linearVelocity    = glm::vec3(0.0f, 0.0f, 0.0f);
    c._linearRotation    = glm::vec2(0.0f, 0.0f);

    c._object = o;
}