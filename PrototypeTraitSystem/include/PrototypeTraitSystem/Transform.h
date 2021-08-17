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
#include <PrototypeCommon/Types.h>

#include <array>

#include <nlohmann/json.hpp>

struct PrototypeObject;

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onEditDispatchHandlerFn)(PrototypeObject* o);

struct Attachable(Trait) Transform
{
    void setModel(const glm::mat4& model);
    void setModel(const float* model);
    void setModelScaled(const float* modelScaled);
    void updateComponentsFromMatrix();

    const glm::vec3& position();
    glm::vec3&       positionMut();
    const glm::vec3& rotation();
    glm::vec3&       rotationMut();
    const glm::vec3& scale();
    glm::vec3&       scaleMut();
    const glm::mat4& model() const;
    glm::mat4&       modelMut();
    const glm::mat4& modelScaled() const;

    const bool& needsPhysicsSync() const;
    void        setNeedsPhysicsSync(bool needsPhysicsSync);

    PrototypeObject* object();
    static void      setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler);
    static void      onEditDispatch(PrototypeObject * o);
    static void      to_json(nlohmann::json & j, const Transform& t);
    static void      from_json(const nlohmann::json& j, Transform& t, PrototypeObject* o);

  private:
    friend struct PrototypeObject;
    glm::mat4x4                    _model;
    glm::mat4x4                    _modelScaled;
    glm::vec3                      _position;
    glm::vec3                      _rotation;
    glm::vec3                      _scale;
    PrototypeObject*               _object;
    static onEditDispatchHandlerFn _onEditDispatchHandler;
    bool                           _needsPhysicsSync;
    bool                           _needsComponentsSync;
};
