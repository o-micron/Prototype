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

#include "../include/PrototypeCommon/Maths.h"

#include <immintrin.h>

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
// --------------------------------------------------------------------------------------------------
// Operators
// --------------------------------------------------------------------------------------------------
// PrototypeVec2
// operator+(const PrototypeVec2& lhs, const PrototypeVec2& rhs)
// {
//     return { lhs._0 + rhs._0, lhs._1 + rhs._1 };
// }

// PrototypeVec2
// operator-(const PrototypeVec2& lhs, const PrototypeVec2& rhs)
// {
//     return { lhs._0 - rhs._0, lhs._1 - rhs._1 };
// }

// PrototypeVec2
// operator*(const PrototypeVec2& lhs, const PrototypeVec2& rhs)
// {
//     return { lhs._0 * rhs._0, lhs._1 * rhs._1 };
// }

// PrototypeVec2
// operator/(const PrototypeVec2& lhs, const PrototypeVec2& rhs)
// {
//     return { lhs._0 / rhs._0, lhs._1 / rhs._1 };
// }

// PrototypeVec3
// operator+(const PrototypeVec3& lhs, const PrototypeVec3& rhs)
// {
//     return { lhs._0 + rhs._0, lhs._1 + rhs._1, lhs._2 + rhs._2 };
// }

// PrototypeVec3
// operator-(const PrototypeVec3& lhs, const PrototypeVec3& rhs)
// {
//     return { lhs._0 - rhs._0, lhs._1 - rhs._1, lhs._2 - rhs._2 };
// }

// PrototypeVec3
// operator*(const PrototypeVec3& lhs, const PrototypeVec3& rhs)
// {
//     return { lhs._0 * rhs._0, lhs._1 * rhs._1, lhs._2 * rhs._2 };
// }

// PrototypeVec3
// operator/(const PrototypeVec3& lhs, const PrototypeVec3& rhs)
// {
//     return { lhs._0 / rhs._0, lhs._1 / rhs._1, lhs._2 / rhs._2 };
// }

// PrototypeVec4
// operator+(const PrototypeVec4& lhs, const PrototypeVec4& rhs)
// {
//     return { lhs._0 + rhs._0, lhs._1 + rhs._1, lhs._2 + rhs._2, lhs._3 + rhs._3 };
// }

// PrototypeVec4
// operator-(const PrototypeVec4& lhs, const PrototypeVec4& rhs)
// {
//     return { lhs._0 - rhs._0, lhs._1 - rhs._1, lhs._2 - rhs._2, lhs._3 - rhs._3 };
// }

// PrototypeVec4
// operator*(const PrototypeVec4& lhs, const PrototypeVec4& rhs)
// {
//     return { lhs._0 * rhs._0, lhs._1 * rhs._1, lhs._2 * rhs._2, lhs._3 * rhs._3 };
// }

// PrototypeVec4
// operator/(const PrototypeVec4& lhs, const PrototypeVec4& rhs)
// {
//     return { lhs._0 / rhs._0, lhs._1 / rhs._1, lhs._2 / rhs._2, lhs._3 / rhs._3 };
// }
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
// Member Functions
// --------------------------------------------------------------------------------------------------
// f32
// PrototypeVec2::length()
// {
//     return sqrt(x * x + y * y);
// }

// void
// PrototypeVec2::normalize()
// {
//     f32 m = length();
//     x /= m;
//     y /= m;
// }

// f32
// PrototypeVec3::length()
// {
//     return sqrt(x * x + y * y + z * z);
// }

// void
// PrototypeVec3::normalize()
// {
//     f32 m = length();
//     x /= m;
//     y /= m;
//     z /= m;
// }

// f32
// PrototypeVec4::length()
// {
//     return sqrt(x * x + y * y + z * z + w * w);
// }

// void
// PrototypeVec4::normalize()
// {
//     f32 m = length();
//     x /= m;
//     y /= m;
//     z /= m;
//     w /= m;
// }

// f32
// PrototypeMat4::determinant()
// {
//     __m256 a0   = _mm256_set_ps(_00, _00, _00, _01, _01, _02, 0, 0);
//     __m256 b0   = _mm256_set_ps(_11, _12, _13, _12, _13, _13, 0, 0);
//     __m256 a0b0 = _mm256_mul_ps(a0, b0);

//     __m256 a1   = _mm256_set_ps(_01, _02, _03, _02, _03, _03, 0, 0);
//     __m256 b1   = _mm256_set_ps(_10, _10, _10, _11, _11, _12, 0, 0);
//     __m256 a1b1 = _mm256_mul_ps(a1, b1);

//     __m256 a2   = _mm256_set_ps(_22, _21, _21, _20, _20, _20, 0, 0);
//     __m256 b2   = _mm256_set_ps(_33, _33, _32, _33, _32, _31, 0, 0);
//     __m256 a2b2 = _mm256_mul_ps(a2, b2);

//     __m256 a3   = _mm256_set_ps(_23, _23, _22, _23, _22, _21, 0, 0);
//     __m256 b3   = _mm256_set_ps(_32, _31, _31, _30, _30, _30, 0, 0);
//     __m256 a3b3 = _mm256_mul_ps(a3, b3);

//     __m256 a0b0_a1b1 = _mm256_sub_ps(a0b0, a1b1);
//     __m256 a2b2_a3b3 = _mm256_sub_ps(a2b2, a3b3);

//     __m256 a0b0a1b1_a2b2a3b3 = _mm256_mul_ps(a0b0_a1b1, a2b2_a3b3);

//     float parts[6];
//     memcpy(parts, &a0b0a1b1_a2b2a3b3, sizeof(float) * 6);

//     return parts[0] - parts[1] + parts[2] + parts[3] - parts[4] + parts[5];

//     // return (array[0] * array[5] - array[1] * array[4]) * (array[10] * array[15] - array[11] * array[14]) -
//     //        (array[0] * array[6] - array[2] * array[4]) * (array[9] * array[15] - array[11] * array[13]) +
//     //        (array[0] * array[7] - array[3] * array[4]) * (array[9] * array[14] - array[10] * array[13]) +
//     //        (array[1] * array[6] - array[2] * array[5]) * (array[8] * array[15] - array[11] * array[12]) -
//     //        (array[1] * array[7] - array[3] * array[5]) * (array[8] * array[14] - array[10] * array[12]) +
//     //        (array[2] * array[7] - array[3] * array[6]) * (array[8] * array[13] - array[9] * array[12]);
// }

// void
// PrototypeMat4::normalize()
// {
//     f32    det         = determinant();
//     __m256 denominator = _mm256_set1_ps(det);
//     __m256 first       = _mm256_div_ps(*((__m256*)&_00), denominator);
//     __m256 second      = _mm256_div_ps(*((__m256*)&_20), denominator);
//     memcpy(&_00, &first, sizeof(__m256));
//     memcpy(&_20, &second, sizeof(__m256));
// }
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
// Serialization / Deserialization
// --------------------------------------------------------------------------------------------------
// void
// PrototypeVec2::to_json(nlohmann::json& j, const PrototypeVec2& v)
// {
//     std::vector<nlohmann::json> jnodes;
//     jnodes.push_back(v.x);
//     jnodes.push_back(v.y);
//     j = jnodes;
// }

// void
// PrototypeVec2::from_json(const nlohmann::json& j, PrototypeVec2& v)
// {
//     v.x = j.at(0);
//     v.y = j.at(1);
// }

// void
// PrototypeVec3::to_json(nlohmann::json& j, const PrototypeVec3& v)
// {
//     std::vector<nlohmann::json> jnodes;
//     jnodes.push_back(v.x);
//     jnodes.push_back(v.y);
//     jnodes.push_back(v.z);
//     j = jnodes;
// }

// void
// PrototypeVec3::from_json(const nlohmann::json& j, PrototypeVec3& v)
// {
//     v.x = j.at(0);
//     v.y = j.at(1);
//     v.z = j.at(2);
// }

// void
// PrototypeVec4::to_json(nlohmann::json& j, const PrototypeVec4& v)
// {
//     std::vector<nlohmann::json> jnodes;
//     jnodes.push_back(v.x);
//     jnodes.push_back(v.y);
//     jnodes.push_back(v.z);
//     jnodes.push_back(v.w);
//     j = jnodes;
// }

// void
// PrototypeVec4::from_json(const nlohmann::json& j, PrototypeVec4& v)
// {
//     v.x = j.at(0);
//     v.y = j.at(1);
//     v.z = j.at(2);
//     v.w = j.at(3);
// }

// void
// PrototypeMat4::to_json(nlohmann::json& j, const PrototypeMat4& v)
// {
//     std::vector<nlohmann::json> jnodes;
//     jnodes.push_back(v._00);
//     jnodes.push_back(v._01);
//     jnodes.push_back(v._02);
//     jnodes.push_back(v._03);

//     jnodes.push_back(v._10);
//     jnodes.push_back(v._11);
//     jnodes.push_back(v._12);
//     jnodes.push_back(v._13);

//     jnodes.push_back(v._20);
//     jnodes.push_back(v._21);
//     jnodes.push_back(v._22);
//     jnodes.push_back(v._23);

//     jnodes.push_back(v._30);
//     jnodes.push_back(v._31);
//     jnodes.push_back(v._32);
//     jnodes.push_back(v._33);
//     j = jnodes;
// }

// void
// PrototypeMat4::from_json(const nlohmann::json& j, PrototypeMat4& v)
// {
//     v._00 = j.at(0);
//     v._01 = j.at(1);
//     v._02 = j.at(2);
//     v._03 = j.at(3);

//     v._10 = j.at(4);
//     v._11 = j.at(5);
//     v._12 = j.at(6);
//     v._13 = j.at(7);

//     v._20 = j.at(8);
//     v._21 = j.at(9);
//     v._22 = j.at(10);
//     v._23 = j.at(11);

//     v._30 = j.at(12);
//     v._31 = j.at(13);
//     v._32 = j.at(14);
//     v._33 = j.at(15);
// }

namespace glm {
void
to_json(nlohmann::json& j, const glm::vec2& v)
{
    std::vector<nlohmann::json> jnodes;
    jnodes.emplace_back(v.x);
    jnodes.emplace_back(v.y);
    j = jnodes;
}

void
from_json(const nlohmann::json& j, glm::vec2& v)
{
    v.x = j.at(0);
    v.y = j.at(1);
}

void
to_json(nlohmann::json& j, const glm::vec3& v)
{
    std::vector<nlohmann::json> jnodes;
    jnodes.emplace_back(v.x);
    jnodes.emplace_back(v.y);
    jnodes.emplace_back(v.z);
    j = jnodes;
}

void
from_json(const nlohmann::json& j, glm::vec3& v)
{
    v.x = j.at(0);
    v.y = j.at(1);
    v.z = j.at(2);
}

void
to_json(nlohmann::json& j, const glm::vec4& v)
{
    std::vector<nlohmann::json> jnodes;
    jnodes.emplace_back(v.x);
    jnodes.emplace_back(v.y);
    jnodes.emplace_back(v.z);
    jnodes.emplace_back(v.w);
    j = jnodes;
}

void
from_json(const nlohmann::json& j, glm::vec4& v)
{
    v.x = j.at(0);
    v.y = j.at(1);
    v.z = j.at(2);
    v.w = j.at(3);
}

void
to_json(nlohmann::json& j, const glm::mat4& v)
{
    std::vector<nlohmann::json> jnodes;
    jnodes.emplace_back(v[0][0]);
    jnodes.emplace_back(v[0][1]);
    jnodes.emplace_back(v[0][2]);
    jnodes.emplace_back(v[0][3]);

    jnodes.emplace_back(v[1][0]);
    jnodes.emplace_back(v[1][1]);
    jnodes.emplace_back(v[1][2]);
    jnodes.emplace_back(v[1][3]);

    jnodes.emplace_back(v[2][0]);
    jnodes.emplace_back(v[2][1]);
    jnodes.emplace_back(v[2][2]);
    jnodes.emplace_back(v[2][3]);

    jnodes.emplace_back(v[3][0]);
    jnodes.emplace_back(v[3][1]);
    jnodes.emplace_back(v[3][2]);
    jnodes.emplace_back(v[3][3]);
    j = jnodes;
}

void
from_json(const nlohmann::json& j, glm::mat4& v)
{
    v[0][0] = j.at(0);
    v[0][1] = j.at(1);
    v[0][2] = j.at(2);
    v[0][3] = j.at(3);

    v[1][0] = j.at(4);
    v[1][1] = j.at(5);
    v[1][2] = j.at(6);
    v[1][3] = j.at(7);

    v[2][0] = j.at(8);
    v[2][1] = j.at(9);
    v[2][2] = j.at(10);
    v[2][3] = j.at(11);

    v[3][0] = j.at(12);
    v[3][1] = j.at(13);
    v[3][2] = j.at(14);
    v[3][3] = j.at(15);
}
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
// Free Functions
// --------------------------------------------------------------------------------------------------
namespace PrototypeMaths {

extern void
decomposeModelMatrix(const glm::mat4& model, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{
    glm::quat orientation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(model, scale, orientation, position, skew, perspective);
    rotation = glm::degrees(glm::eulerAngles(orientation));
}

extern void
buildModelMatrix(glm::mat4& model, const glm::vec3& position, const glm::vec3& rotation)
{
    static const glm::vec3 right   = glm::vec3(1.0f, 0.0f, 0.0f);
    static const glm::vec3 up      = glm::vec3(0.0f, 1.0f, 0.0f);
    static const glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
    model                          = glm::translate(glm::mat4(1.0f), position);
    model                          = glm::rotate(model, glm::radians(rotation.x), right);
    model                          = glm::rotate(model, glm::radians(rotation.y), up);
    model                          = glm::rotate(model, glm::radians(rotation.z), forward);
}

extern void
buildModelMatrixWithScale(glm::mat4& model, const glm::vec3& scale)
{
    model = glm::scale(model, scale);
}

extern void
buildTargetViewMatrix(glm::mat4& view, const glm::vec3& position, const glm::vec3& point)
{
    static const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    view                      = glm::lookAt(position, point, up);
}

extern void
buildOrbitalViewMatrix(glm::mat4& view, const glm::vec3& position, const glm::quat& rotation)
{
    glm::mat4 positionMatrix = glm::translate(-position);
    glm::mat4 rotationMatrix = glm::transpose(glm::toMat4(rotation));
    view                     = positionMatrix * rotationMatrix;
}

extern void
buildFreeViewMatrix(glm::mat4& view, const glm::vec3& position, const glm::quat& rotation)
{
    glm::mat4 positionMatrix = glm::translate(-position);
    glm::mat4 rotationMatrix = glm::transpose(glm::toMat4(rotation));
    view                     = rotationMatrix * positionMatrix;
}

extern void
buildPerspectiveProjection(glm::mat4& projection, const float fov, const float aspectRatio, const float near, const float far)
{
    projection = glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

extern void
buildOrthographicProjection(glm::mat4&  projection,
                            const float left,
                            const float right,
                            const float bottom,
                            const float top,
                            const float near,
                            const float far)
{
    projection = glm::ortho(left, right, bottom, top, near, far);
}

extern void
projectRayFromClipSpacePoint(glm::vec3&       ray,
                             const glm::mat4& viewMatrix,
                             const glm::mat4& projectionMatrix,
                             f32              u,
                             f32              v,
                             f32              width,
                             f32              height)
{
    glm::mat4 inverseProjection = glm::inverse(projectionMatrix);
    glm::mat4 inverseView       = glm::inverse(viewMatrix);

    f32       x        = (2.0f * u) / width - 1.0f;
    f32       y        = 1.0f - (2.0f * v) / height;
    f32       z        = 1.0f;
    glm::vec3 rayNds   = glm::vec3(x, y, z);
    glm::vec4 rayClip  = glm::vec4(rayNds.x, rayNds.y, -1.0f, 1.0f);
    glm::vec4 rayEye   = inverseProjection * rayClip;
    rayEye             = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    glm::vec4 worldEye = inverseView * rayEye;
    glm::vec3 rayWorld = glm::vec3(worldEye.x, worldEye.y, worldEye.z);
    rayWorld           = glm::normalize(rayWorld);
    ray                = { rayWorld.x, rayWorld.y, rayWorld.z };
}

extern void
projectRayFromClipSpaceCenterPoint(glm::vec3& ray, const glm::mat4& viewMatrix)
{
    ray = glm::normalize(glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]));
}

} // namespace PrototypeMaths
