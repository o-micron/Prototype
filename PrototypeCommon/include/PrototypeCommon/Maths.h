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

#include "Definitions.h"
#include "Types.h"

#pragma warning(disable : 4996)
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <array>

#include <nlohmann/json.hpp>

struct PrototypeVec2
{
    union
    {
        struct
        {
            std::array<f32, 2> array;
        };
        struct
        {
            f32 _0;
            f32 _1;
        };
        struct
        {
            f32 x;
            f32 y;
        };
    };

    f32  length();
    void normalize();

    static void to_json(nlohmann::json& j, const PrototypeVec2& v);
    static void from_json(const nlohmann::json& j, PrototypeVec2& v);
};

struct PrototypeVec3
{
    union
    {
        struct
        {
            std::array<f32, 3> array;
        };
        struct
        {
            f32 _0;
            f32 _1;
            f32 _2;
        };
        struct
        {
            f32 x;
            f32 y;
            f32 z;
        };
        struct
        {
            PrototypeVec2 xy;
            f32           __z;
        };
        struct
        {
            f32           __x;
            PrototypeVec2 yz;
        };
    };

    f32  length();
    void normalize();

    static void to_json(nlohmann::json& j, const PrototypeVec3& v);
    static void from_json(const nlohmann::json& j, PrototypeVec3& v);
};

struct PrototypeVec4
{
    union
    {
        struct
        {
            std::array<f32, 4> array;
        };
        struct
        {
            f32 _0;
            f32 _1;
            f32 _2;
            f32 _3;
        };
        struct
        {
            f32 x;
            f32 y;
            f32 z;
            f32 w;
        };
        struct
        {
            PrototypeVec2 xy;
            PrototypeVec2 zw;
        };
        struct
        {
            PrototypeVec3 xyz;
            f32           __w;
        };
        struct
        {
            f32           __x;
            PrototypeVec2 yz;
            f32           __w2;
        };
        struct
        {
            f32           __x2;
            PrototypeVec3 yzw;
        };
    };

    f32  length();
    void normalize();

    static void to_json(nlohmann::json& j, const PrototypeVec4& v);
    static void from_json(const nlohmann::json& j, PrototypeVec4& v);
};

struct PrototypeMat4
{
    union
    {
        struct
        {
            std::array<f32, 16> array;
        };
        struct
        {
            f32 _00;
            f32 _01;
            f32 _02;
            f32 _03;

            f32 _10;
            f32 _11;
            f32 _12;
            f32 _13;

            f32 _20;
            f32 _21;
            f32 _22;
            f32 _23;

            f32 _30;
            f32 _31;
            f32 _32;
            f32 _33;
        };
        struct
        {
            PrototypeVec4 row0;
            PrototypeVec4 row1;
            PrototypeVec4 row2;
            PrototypeVec4 row3;
        };
    };

    f32  determinant();
    void normalize();

    static void to_json(nlohmann::json& j, const PrototypeMat4& v);
    static void from_json(const nlohmann::json& j, PrototypeMat4& v);
};

namespace glm {
void
to_json(nlohmann::json& j, const glm::vec2& v);
void
from_json(const nlohmann::json& j, glm::vec2& v);

void
to_json(nlohmann::json& j, const glm::vec3& v);
void
from_json(const nlohmann::json& j, glm::vec3& v);

void
to_json(nlohmann::json& j, const glm::vec4& v);
void
from_json(const nlohmann::json& j, glm::vec4& v);

void
to_json(nlohmann::json& j, const glm::mat4& v);
void
from_json(const nlohmann::json& j, glm::mat4& v);
}

namespace PrototypeMaths {

extern void
decomposeModelMatrix(const glm::mat4& model, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);

extern void
buildModelMatrix(glm::mat4& model, const glm::vec3& position, const glm::vec3& rotation);

extern void
buildModelMatrixWithScale(glm::mat4& model, const glm::vec3& scale);

extern void
buildTargetViewMatrix(glm::mat4& view, const glm::vec3& position, const glm::vec3& point);

extern void
buildOrbitalViewMatrix(glm::mat4& view, const glm::vec3& position, const glm::quat& rotation);

extern void
buildFreeViewMatrix(glm::mat4& view, const glm::vec3& position, const glm::quat& rotation);

extern void
buildPerspectiveProjection(glm::mat4& projection, const f32 fov, const f32 aspectRatio, const f32 near, const f32 far);

extern void
buildOrthographicProjection(glm::mat4& projection,
                            const f32  left,
                            const f32  right,
                            const f32  bottom,
                            const f32  top,
                            const f32  near,
                            const f32  far);

extern void
projectRayFromClipSpacePoint(glm::vec3&       ray,
                             const glm::mat4& viewMatrix,
                             const glm::mat4& projectionMatrix,
                             f32              u,
                             f32              v,
                             f32              width,
                             f32              height);

extern void
projectRayFromClipSpaceCenterPoint(glm::vec3& ray, const glm::mat4& viewMatrix);

} // namespace PrototypeMaths