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
struct PrototypeCameraSystem;

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onEditDispatchHandlerFn)(PrototypeObject* o);

struct Attachable(Trait) Camera
{
    // free
    const f32&       lookSensitivity() const;
    f32&             lookSensitivityMut();
    const f32&       moveSensitivity() const;
    f32&             moveSensitivityMut();
    const glm::vec2& rotation() const;
    const glm::quat& rotationQuat() const;
    const glm::vec3& position() const;
    const bool&      orbital() const;
    bool&            orbitalMut();
    const glm::mat4& viewMatrix() const;

    // perspective
    const f32&       fov() const;
    f32&             fovMut();
    const f32&       aspectRatio() const;
    const f32&       znear() const;
    f32&             nearMut();
    const f32&       zfar() const;
    f32&             farMut();
    const glm::vec2& resolution() const;
    const glm::mat4& projectionMatrix() const;

    PrototypeObject* object();
    static void      setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler);
    static void      onEditDispatch(PrototypeObject * o);
    static void      to_json(nlohmann::json & j, const Camera& c);
    static void      from_json(const nlohmann::json& j, Camera& c, PrototypeObject* o);

  private:
    friend struct PrototypeObject;
    friend struct PrototypeInterface;
    PrototypeObject*               _object;
    static onEditDispatchHandlerFn _onEditDispatchHandler;

  public:
    // free
    glm::mat4x4 _viewMatrix;
    glm::mat4x4 _projectionMatrix;
    glm::quat   _rotationQuat;
    glm::vec3   _position;
    glm::vec3   _linearVelocity;
    glm::vec2   _linearRotation;
    f32         _interpolationTime;
    glm::vec2   _rotation;
    f32         _lookSensitivity;
    f32         _moveSensitivity;
    glm::vec2   _resolution;
    f32         _fov;
    f32         _aspectRatio;
    f32         _near;
    f32         _far;
    bool        _orbital;
};
