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

#include <PrototypeCommon/Maths.h>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

struct PrototypeMeshBuffer;

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onMeshBufferReloadFn)(PrototypeMeshBuffer* meshBuffer);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onMeshBufferGpuUploadFn)(PrototypeMeshBuffer* meshBuffer);

struct PrototypeMeshVertex
{
    glm::vec4 positionU;
    glm::vec4 normalV;
    glm::vec4 color;

    bool operator==(const PrototypeMeshVertex& other) const
    {
        glm::vec3 position(positionU.x, positionU.y, positionU.z);
        glm::vec3 normal(normalV.x, normalV.y, normalV.z);
        glm::vec2 texcoords(positionU.w, normalV.w);

        glm::vec3 otherPosition(other.positionU.x, other.positionU.y, other.positionU.z);
        glm::vec3 otherNormal(other.normalV.x, other.normalV.y, other.normalV.z);
        glm::vec2 otherTexcoords(other.positionU.w, other.normalV.w);

        return position == otherPosition;
    }

    static void to_json(nlohmann::json& j, const std::vector<PrototypeMeshVertex>& vertices);
};

enum PrototypeMeshBufferType_
{
    PrototypeMeshBufferType_Triangles = 0,
    PrototypeMeshBufferType_Quads,

    PrototypeMeshBufferType_Constant_Triangles,
    PrototypeMeshBufferType_Constant_Triangles_2D,
    PrototypeMeshBufferType_Constant_Triangles_3D,
    PrototypeMeshBufferType_Constant_Colored_Triangles_2D,
    PrototypeMeshBufferType_Constant_Colored_Triangles_3D,
    PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_2D,
    PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_3D,

    PrototypeMeshBufferType_Constant_Quads_2D,
    PrototypeMeshBufferType_Constant_Quads_3D,
    PrototypeMeshBufferType_Constant_Colored_Quads_2D,
    PrototypeMeshBufferType_Constant_Colored_Quads_3D,

    PrototypeMeshBufferType_Count
};

PROTOTYPE_FOR_EACH_X(PROTOTYPE_STRINGIFY_ENUM_EXTENDED,
                     PROTOTYPE_STRINGIFY_ENUM,
                     PrototypeMeshBufferType_Triangles,
                     PrototypeMeshBufferType_Quads,
                     PrototypeMeshBufferType_Constant_Triangles,
                     PrototypeMeshBufferType_Constant_Triangles_2D,
                     PrototypeMeshBufferType_Constant_Triangles_3D,
                     PrototypeMeshBufferType_Constant_Colored_Triangles_2D,
                     PrototypeMeshBufferType_Constant_Colored_Triangles_3D,
                     PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_2D,
                     PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_3D,
                     PrototypeMeshBufferType_Constant_Quads_2D,
                     PrototypeMeshBufferType_Constant_Quads_3D,
                     PrototypeMeshBufferType_Constant_Colored_Quads_2D,
                     PrototypeMeshBufferType_Constant_Colored_Quads_3D);

struct PrototypeMeshBufferSource
{
    PrototypeMeshBufferSource()
      : type(PrototypeMeshBufferType_Count)
    {}

    PrototypeMeshBufferSource(PrototypeMeshBufferType_ type, std::vector<PrototypeMeshVertex> vertices, std::vector<u32> indices)
      : type(type)
      , vertices(vertices)
      , indices(indices)
    {}
    PrototypeMeshBufferType_         type;
    std::vector<PrototypeMeshVertex> vertices;
    std::vector<u32>                 indices;
};

struct PrototypeMeshBuffer
{
    PrototypeMeshBuffer(const std::string name);
    ~PrototypeMeshBuffer();

    const u32&                       id() const;
    const std::string&               name() const;
    const PrototypeMeshBufferSource& source() const;
    const std::string&               fullpath() const;
    const time_t&                    timestamp() const;
    const bool&                      needsUpload() const;

    void setFullpath(std::string fullpath);
    void setTimestamp(time_t timestamp);

    void setSource(std::unique_ptr<PrototypeMeshBufferSource> source);
    void stageChange();
    void commitChange();
    void unsetData();

    static void to_json(nlohmann::json& j, const PrototypeMeshBuffer& meshBuffer);
    static void from_json(const nlohmann::json& j);

    void* userData;

  private:
    const u32                                  _id;
    const std::string                          _name;
    std::string                                _fullpath;
    time_t                                     _timestamp;
    bool                                       _needsUpload;
    std::unique_ptr<PrototypeMeshBufferSource> _source;
};