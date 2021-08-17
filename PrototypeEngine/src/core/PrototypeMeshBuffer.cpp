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

#include "PrototypeMeshBuffer.h"
#include "PrototypeDatabase.h"
#include "PrototypeEngine.h"
#include "PrototypeRenderer.h"
#include "PrototypeStaticInitializer.h"
#include "PrototypeUI.h"

#include <PrototypeCommon/IO.h>
#include <PrototypeCommon/Logger.h>

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/gtx/hash.hpp>

namespace std {
template<>
struct hash<PrototypeMeshVertex>
{
    size_t operator()(PrototypeMeshVertex const& v) const
    {
        glm::vec3 position(v.positionU.x, v.positionU.y, v.positionU.z);
        glm::vec3 normal(v.normalV.x, v.normalV.y, v.normalV.z);
        glm::vec2 texcoords(v.positionU.w, v.normalV.w);
        return ((hash<glm::vec3>()(position) ^ (hash<glm::vec3>()(normal) << 1)) >> 1) ^ (hash<glm::vec2>()(texcoords) << 1);
    }
};
}

void
loadSourceFromFile(PrototypeMeshBufferSource* meshBufferSource, const std::string& meshFullPath)
{
    i32              defaultFlags = aiProcess_Triangulate | aiProcess_FlipUVs;
    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFile(meshFullPath, defaultFlags);
    if (!scene) {
        PrototypeLogger::warn("Mesh file not found %s", meshFullPath.c_str());
        return;
    }
    meshBufferSource->vertices.clear();
    meshBufferSource->indices.clear();
    std::unordered_map<PrototypeMeshVertex, u32> uniqueVertices{};
    for (u32 m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* sceneMesh = scene->mMeshes[m];
        for (u32 v = 0; v < sceneMesh->mNumVertices; ++v) {
            PrototypeMeshVertex vertex = {};
            vertex.positionU.x         = sceneMesh->mVertices[v].x;
            vertex.positionU.y         = sceneMesh->mVertices[v].y;
            vertex.positionU.z         = sceneMesh->mVertices[v].z;

            vertex.normalV.x = sceneMesh->mNormals[v].x;
            vertex.normalV.y = sceneMesh->mNormals[v].y;
            vertex.normalV.z = sceneMesh->mNormals[v].z;

            vertex.positionU.w = sceneMesh->mTextureCoords[0][v].x;
            vertex.normalV.w   = sceneMesh->mTextureCoords[0][v].y;

            aiMaterial* material = scene->mMaterials[sceneMesh->mMaterialIndex];
            if (material) {
                aiColor3D color;
                material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
                vertex.color.x = color.r;
                vertex.color.y = color.g;
                vertex.color.z = color.b;
                vertex.color.w = 1.0f;
            } else {
                vertex.color.x = 1.0f;
                vertex.color.y = 1.0f;
                vertex.color.z = 1.0f;
                vertex.color.w = 1.0f;
            }

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = (u32)meshBufferSource->vertices.size();
                meshBufferSource->vertices.push_back(vertex);
            }
            meshBufferSource->indices.push_back(uniqueVertices[vertex]);
        }
    }
}

PrototypeMeshBuffer::PrototypeMeshBuffer(std::string name)
  : _id(++PrototypeStaticInitializer::_meshBufferUUID)
  , _name(std::move(name))
  , _timestamp(0)
  , _fullpath("")
  , userData(nullptr)
  , _needsUpload(false)
{}

PrototypeMeshBuffer::~PrototypeMeshBuffer() { unsetData(); }

const u32&
PrototypeMeshBuffer::id() const
{
    return _id;
}

const std::string&
PrototypeMeshBuffer::name() const
{
    return _name;
}

const PrototypeMeshBufferSource&
PrototypeMeshBuffer::source() const
{
    return *_source;
}

const std::string&
PrototypeMeshBuffer::fullpath() const
{
    return _fullpath;
}

const time_t&
PrototypeMeshBuffer::timestamp() const
{
    return _timestamp;
}

const bool&
PrototypeMeshBuffer::needsUpload() const
{
    return _needsUpload;
}

void
PrototypeMeshBuffer::setFullpath(std::string fullpath)
{
    _fullpath = fullpath;
}

void
PrototypeMeshBuffer::setTimestamp(time_t timestamp)
{
    _timestamp = timestamp;
}

void
PrototypeMeshBuffer::setSource(std::unique_ptr<PrototypeMeshBufferSource> source)
{
    _source = std::move(source);
}

void
PrototypeMeshBuffer::stageChange()
{
    _timestamp = PrototypeIo::filestamp(_fullpath);
    loadSourceFromFile(_source.get(), _fullpath);
    _needsUpload = true;
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    PrototypeEngineInternalApplication::renderer->ui()->signalBuffersChanged(true);
#endif
}

void
PrototypeMeshBuffer::commitChange()
{
    if (_needsUpload) {
        _needsUpload = false;
        PrototypeEngineInternalApplication::renderer->onMeshBufferGpuUpload(this);
    }
}

void
PrototypeMeshBuffer::unsetData()
{
    _source->vertices.clear();
    _source->vertices.shrink_to_fit();
    _source->indices.clear();
    _source->indices.shrink_to_fit();
}

void
PrototypeMeshBuffer::to_json(nlohmann::json& j, const PrototypeMeshBuffer& meshBuffer)
{
    const char* field_id       = "id";
    const char* field_type     = "type";
    const char* field_name     = "name";
    const char* field_vertices = "vertices";
    const char* field_indices  = "indices";

    j[field_id]   = meshBuffer.id();
    j[field_name] = meshBuffer.name();
    switch (meshBuffer.source().type) {
        case PrototypeMeshBufferType_Triangles: {
            j[field_type] = PrototypeMeshBufferType_Triangles_Str;
        } break;
        case PrototypeMeshBufferType_Constant_Triangles: {
            j[field_type] = PrototypeMeshBufferType_Constant_Triangles_Str;
        } break;
        case PrototypeMeshBufferType_Constant_Triangles_2D: {
            j[field_type] = PrototypeMeshBufferType_Constant_Triangles_2D_Str;
        } break;
        case PrototypeMeshBufferType_Constant_Triangles_3D: {
            j[field_type] = PrototypeMeshBufferType_Constant_Triangles_3D_Str;
        } break;
        case PrototypeMeshBufferType_Constant_Colored_Triangles_2D: {
            j[field_type] = PrototypeMeshBufferType_Constant_Colored_Triangles_2D_Str;
        } break;
        case PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_2D: {
            j[field_type] = PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_2D_Str;
        } break;
        case PrototypeMeshBufferType_Constant_Colored_Triangles_3D: {
            j[field_type] = PrototypeMeshBufferType_Constant_Colored_Triangles_3D_Str;
        } break;
        case PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_3D: {
            j[field_type] = PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_3D_Str;
        } break;
        case PrototypeMeshBufferType_Quads: {
            j[field_type] = PrototypeMeshBufferType_Quads_Str;
        } break;
        case PrototypeMeshBufferType_Constant_Quads_2D: {
            j[field_type] = PrototypeMeshBufferType_Constant_Quads_2D_Str;
        } break;
        case PrototypeMeshBufferType_Constant_Quads_3D: {
            j[field_type] = PrototypeMeshBufferType_Constant_Quads_3D_Str;
        } break;
        case PrototypeMeshBufferType_Constant_Colored_Quads_2D: {
            j[field_type] = PrototypeMeshBufferType_Constant_Colored_Quads_2D_Str;
        } break;
        case PrototypeMeshBufferType_Constant_Colored_Quads_3D: {
            j[field_type] = PrototypeMeshBufferType_Constant_Colored_Quads_3D_Str;
        } break;
        case PrototypeMeshBufferType_Count: {
            PrototypeLogger::fatal("Unimplemented!(Unreachable)");
        } break;
    }
    PrototypeMeshVertex::to_json(j[field_vertices], meshBuffer.source().vertices);
    j[field_indices] = nlohmann::json(meshBuffer.source().indices);
}

void
PrototypeMeshBuffer::from_json(const nlohmann::json& j)
{
    if (j.is_null()) { return; }

    const char* field_type     = "type";
    const char* field_path     = "path";
    const char* field_name     = "name";
    const char* field_vertices = "vertices";
    const char* field_indices  = "indices";

    const auto meshType = j.at(field_type).get<std::string>();

    auto meshBufferSource = std::make_unique<PrototypeMeshBufferSource>();

    if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType == PrototypeMeshBufferType_Triangles_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Triangles;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType == PrototypeMeshBufferType_Constant_Triangles_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Constant_Triangles;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType == PrototypeMeshBufferType_Constant_Triangles_2D_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Constant_Triangles_2D;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType == PrototypeMeshBufferType_Constant_Triangles_3D_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Constant_Triangles_3D;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType ==
               PrototypeMeshBufferType_Constant_Colored_Triangles_2D_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Constant_Colored_Triangles_2D;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType ==
               PrototypeMeshBufferType_Constant_Colored_Triangles_3D_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Constant_Colored_Triangles_3D;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType ==
               PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_2D_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_2D;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType ==
               PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_3D_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_3D;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType == PrototypeMeshBufferType_Quads_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Quads;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType == PrototypeMeshBufferType_Constant_Quads_2D_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Constant_Quads_2D;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType == PrototypeMeshBufferType_Constant_Quads_3D_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Constant_Quads_3D;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType ==
               PrototypeMeshBufferType_Constant_Colored_Quads_2D_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Constant_Colored_Quads_2D;
    } else if (PROTOTYPE_STRINGIFY(PrototypeMeshBufferType_) + meshType ==
               PrototypeMeshBufferType_Constant_Colored_Quads_3D_Str) {
        meshBufferSource->type = PrototypeMeshBufferType_Constant_Colored_Quads_3D;
    } else {
        PrototypeLogger::fatal("Unknown mesh buffer type <%s>", meshType.c_str());
        return;
    }

    if (meshBufferSource->type == PrototypeMeshBufferType_Triangles) {
        const auto& meshPath = j.at(field_path).get<std::string>();
        if (meshPath.empty()) { return; }
        if (PrototypeEngineInternalApplication::database->meshBuffers.find(meshPath) !=
            PrototypeEngineInternalApplication::database->meshBuffers.end()) {
            return;
        }

        const std::string& meshFullPath = PROTOTYPE_MESH_PATH("") + meshPath;
        loadSourceFromFile(meshBufferSource.get(), meshFullPath);
        if (meshBufferSource->vertices.empty() || meshBufferSource->indices.empty()) {
            PrototypeLogger::warn("Mesh file is probably empty %s", meshFullPath.c_str());
            return;
        }
        PrototypeMeshBuffer* meshBuffer = PrototypeEngineInternalApplication::database->allocateMeshBuffer(meshPath);
        meshBuffer->_fullpath           = meshFullPath;
        meshBuffer->_timestamp          = PrototypeIo::filestamp(meshBuffer->_fullpath);
        meshBuffer->setSource(std::move(meshBufferSource));
        PrototypeEngineInternalApplication::database->meshBuffers.insert({ meshPath, meshBuffer });

    } else if (meshBufferSource->type == PrototypeMeshBufferType_Constant_Triangles) {
        const auto& name = j.at(field_name).get<std::string>();
        if (name.empty()) { return; }
        if (PrototypeEngineInternalApplication::database->meshBuffers.find(name) !=
            PrototypeEngineInternalApplication::database->meshBuffers.end()) {
            return;
        }
        // meshBufferSource->type = PrototypeMeshBufferType_Triangles;
        auto vertices             = j.at(field_vertices).get<std::vector<f32>>();
        meshBufferSource->indices = j.at(field_indices).get<std::vector<u32>>();

        size_t offset = 4 + 4 + 4; // positionU, normalV, color
        meshBufferSource->vertices.clear();
        meshBufferSource->vertices.reserve(vertices.size() / offset);
        for (size_t v = 0; v < vertices.size(); v += offset) {
            meshBufferSource->vertices.push_back({ { vertices[v + 0], vertices[v + 1], vertices[v + 2], vertices[v + 3] },
                                                   { vertices[v + 4], vertices[v + 5], vertices[v + 6], vertices[v + 7] },
                                                   { vertices[v + 8], vertices[v + 9], vertices[v + 10], vertices[v + 11] } });
        }
        PrototypeMeshBuffer* meshBuffer = PrototypeEngineInternalApplication::database->allocateMeshBuffer(name);
        meshBuffer->_fullpath           = name;
        meshBuffer->_timestamp          = 0;
        meshBuffer->setSource(std::move(meshBufferSource));
        PrototypeEngineInternalApplication::database->meshBuffers.insert({ name, meshBuffer });
    } else if (meshBufferSource->type == PrototypeMeshBufferType_Constant_Triangles_2D) {
        const auto& name = j.at(field_name).get<std::string>();
        if (name.empty()) { return; }
        if (PrototypeEngineInternalApplication::database->meshBuffers.find(name) !=
            PrototypeEngineInternalApplication::database->meshBuffers.end()) {
            return;
        }
        const size_t padding  = 2;
        const auto&  vertices = j.at(field_vertices).get<std::vector<f32>>();
        meshBufferSource->vertices.resize(vertices.size() / padding);
        for (size_t i = 0; i < vertices.size() / padding; ++i) {
            meshBufferSource->vertices[i].positionU = { vertices[i * padding], vertices[i * padding + 1], 0.0f, 0.0f };
            meshBufferSource->vertices[i].normalV   = { 0.0f, 0.0f, 0.0f, 0.0f };
        }
        meshBufferSource->indices       = j.at(field_indices).get<std::vector<u32>>();
        PrototypeMeshBuffer* meshBuffer = PrototypeEngineInternalApplication::database->allocateMeshBuffer(name);
        meshBuffer->_fullpath           = name;
        meshBuffer->_timestamp          = 0;
        meshBuffer->setSource(std::move(meshBufferSource));
        PrototypeEngineInternalApplication::database->meshBuffers.insert({ name, meshBuffer });
    } else if (meshBufferSource->type == PrototypeMeshBufferType_Constant_Triangles_3D) {
        const auto& name = j.at(field_name).get<std::string>();
        if (name.empty()) { return; }
        if (PrototypeEngineInternalApplication::database->meshBuffers.find(name) !=
            PrototypeEngineInternalApplication::database->meshBuffers.end()) {
            return;
        }
        const size_t padding  = 3;
        const auto&  vertices = j.at(field_vertices).get<std::vector<f32>>();
        meshBufferSource->vertices.resize(vertices.size() / padding);
        for (size_t i = 0; i < vertices.size() / padding; ++i) {
            meshBufferSource->vertices[i].positionU = {
                vertices[i * padding], vertices[i * padding + 1], vertices[i * padding + 2], 0.0f
            };
            meshBufferSource->vertices[i].normalV = { 0.0f, 0.0f, 0.0f, 0.0f };
        }
        meshBufferSource->indices       = j.at(field_indices).get<std::vector<u32>>();
        PrototypeMeshBuffer* meshBuffer = PrototypeEngineInternalApplication::database->allocateMeshBuffer(name);
        meshBuffer->_fullpath           = name;
        meshBuffer->_timestamp          = 0;
        meshBuffer->setSource(std::move(meshBufferSource));
        PrototypeEngineInternalApplication::database->meshBuffers.insert({ name, meshBuffer });
    } else if (meshBufferSource->type == PrototypeMeshBufferType_Constant_Colored_Triangles_2D) {
        const auto& name = j.at(field_name).get<std::string>();
        if (name.empty()) { return; }
        if (PrototypeEngineInternalApplication::database->meshBuffers.find(name) !=
            PrototypeEngineInternalApplication::database->meshBuffers.end()) {
            return;
        }
        const size_t padding  = 5;
        const auto&  vertices = j.at(field_vertices).get<std::vector<f32>>();
        meshBufferSource->vertices.resize(vertices.size() / padding);
        for (size_t i = 0; i < vertices.size() / padding; ++i) {
            meshBufferSource->vertices[i].positionU = { vertices[i * padding], vertices[i * padding + 1], 0.0f, 0.0f };
            meshBufferSource->vertices[i].normalV   = {
                vertices[i * padding + 2], vertices[i * padding + 3], vertices[i * padding + 4], 0.0f
            };
        }
        meshBufferSource->indices       = j.at(field_indices).get<std::vector<u32>>();
        PrototypeMeshBuffer* meshBuffer = PrototypeEngineInternalApplication::database->allocateMeshBuffer(name);
        meshBuffer->_fullpath           = name;
        meshBuffer->_timestamp          = 0;
        meshBuffer->setSource(std::move(meshBufferSource));
        PrototypeEngineInternalApplication::database->meshBuffers.insert({ name, meshBuffer });
    } else if (meshBufferSource->type == PrototypeMeshBufferType_Constant_Colored_Triangles_3D) {
        const auto& name = j.at(field_name).get<std::string>();
        if (name.empty()) { return; }
        if (PrototypeEngineInternalApplication::database->meshBuffers.find(name) !=
            PrototypeEngineInternalApplication::database->meshBuffers.end()) {
            return;
        }
        const size_t padding  = 6;
        const auto&  vertices = j.at(field_vertices).get<std::vector<f32>>();
        meshBufferSource->vertices.resize(vertices.size() / padding);
        for (size_t i = 0; i < vertices.size() / padding; ++i) {
            meshBufferSource->vertices[i].positionU = {
                vertices[i * padding], vertices[i * padding + 1], vertices[i * padding + 2], 0.0f
            };
            meshBufferSource->vertices[i].normalV = {
                vertices[i * padding + 3], vertices[i * padding + 4], vertices[i * padding + 5], 0.0f
            };
        }
        meshBufferSource->indices       = j.at(field_indices).get<std::vector<u32>>();
        PrototypeMeshBuffer* meshBuffer = PrototypeEngineInternalApplication::database->allocateMeshBuffer(name);
        meshBuffer->_fullpath           = name;
        meshBuffer->_timestamp          = 0;
        meshBuffer->setSource(std::move(meshBufferSource));
        PrototypeEngineInternalApplication::database->meshBuffers.insert({ name, meshBuffer });
    } else if (meshBufferSource->type == PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_2D) {
        const auto& name = j.at(field_name).get<std::string>();
        if (name.empty()) { return; }
        if (PrototypeEngineInternalApplication::database->meshBuffers.find(name) !=
            PrototypeEngineInternalApplication::database->meshBuffers.end()) {
            return;
        }
        const size_t padding  = 7;
        const auto&  vertices = j.at(field_vertices).get<std::vector<f32>>();
        meshBufferSource->vertices.resize(vertices.size() / padding);
        for (size_t i = 0; i < vertices.size() / padding; ++i) {
            meshBufferSource->vertices[i].positionU = {
                vertices[i * padding], vertices[i * padding + 1], 0.0f, vertices[i * padding + 2]
            };
            meshBufferSource->vertices[i].normalV = {
                vertices[i * padding + 4], vertices[i * padding + 5], vertices[i * padding + 6], vertices[i * padding + 3]
            };
        }
        meshBufferSource->indices       = j.at(field_indices).get<std::vector<u32>>();
        PrototypeMeshBuffer* meshBuffer = PrototypeEngineInternalApplication::database->allocateMeshBuffer(name);
        meshBuffer->_fullpath           = name;
        meshBuffer->_timestamp          = 0;
        meshBuffer->setSource(std::move(meshBufferSource));
        PrototypeEngineInternalApplication::database->meshBuffers.insert({ name, meshBuffer });
    } else if (meshBufferSource->type == PrototypeMeshBufferType_Constant_Colored_Textured_Triangles_3D) {
        const auto& name = j.at(field_name).get<std::string>();
        if (name.empty()) { return; }
        if (PrototypeEngineInternalApplication::database->meshBuffers.find(name) !=
            PrototypeEngineInternalApplication::database->meshBuffers.end()) {
            return;
        }
        const size_t padding  = 8;
        const auto&  vertices = j.at(field_vertices).get<std::vector<f32>>();
        meshBufferSource->vertices.resize(vertices.size() / padding);
        for (size_t i = 0; i < vertices.size() / padding; ++i) {
            meshBufferSource->vertices[i].positionU = {
                vertices[i * padding], vertices[i * padding + 1], vertices[i * padding + 2], vertices[i * padding + 3]
            };
            meshBufferSource->vertices[i].normalV = {
                vertices[i * padding + 4], vertices[i * padding + 5], vertices[i * padding + 6], vertices[i * padding + 7]
            };
        }
        meshBufferSource->indices       = j.at(field_indices).get<std::vector<u32>>();
        PrototypeMeshBuffer* meshBuffer = PrototypeEngineInternalApplication::database->allocateMeshBuffer(name);
        meshBuffer->_fullpath           = name;
        meshBuffer->_timestamp          = 0;
        meshBuffer->setSource(std::move(meshBufferSource));
        PrototypeEngineInternalApplication::database->meshBuffers.insert({ name, meshBuffer });
    } else {
        PrototypeLogger::fatal("TODO: implement constant 2d triangulated mesh buffer");
        return;
    }
}

void
PrototypeMeshVertex::to_json(nlohmann::json& j, const std::vector<PrototypeMeshVertex>& vertices)
{
    std::vector<f32> data;
    for (const auto& vertex : vertices) {
        data.push_back(vertex.positionU.x);
        data.push_back(vertex.positionU.y);
        data.push_back(vertex.positionU.z);
        data.push_back(vertex.positionU.w);
        data.push_back(vertex.normalV.x);
        data.push_back(vertex.normalV.y);
        data.push_back(vertex.normalV.z);
        data.push_back(vertex.normalV.w);
    }
    j.push_back(nlohmann::json(data));
}