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

#include "PrototypeSceneLoader.h"
#include "PrototypeFrameBuffer.h"
#include "PrototypeMaterial.h"
#include "PrototypeMeshBuffer.h"
#include "PrototypeShaderBuffer.h"
#include "PrototypeTextureBuffer.h"

#include <PrototypeCommon/IO.h>
#include <PrototypeCommon/Logger.h>

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include "PrototypeDatabase.h"
#include "PrototypeEngine.h"
#include "PrototypePhysics.h"
#include "PrototypeRenderer.h"
#include "PrototypeScene.h"
#include "PrototypeSceneLayer.h"
#include "PrototypeSceneNode.h"

#include "PrototypePreloadedAssets.h"

#include <filesystem>
#include <fstream>
#include <thread>

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

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

#include <glm/gtc/type_ptr.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#include <tinygltf/tiny_gltf.h>

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
PrototypeSceneLoader::loadResourcesFromFile(const char* filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        PrototypeLogger::warn("Couldn't load resources from <%s>", filepath);
        return;
    }
    nlohmann::json j;
    file >> j;
    file.close();

    if (j.is_null()) { return; }

    const char* field_meshes       = "meshes";
    const char* field_shaders      = "shaders";
    const char* field_textures     = "textures";
    const char* field_materials    = "materials";
    const char* field_framebuffers = "framebuffers";

    if (!j.contains(field_meshes)) {
        PrototypeLogger::warn("Resources don't have \"%s\"", field_meshes);
        return;
    }
    if (!j.contains(field_shaders)) {
        PrototypeLogger::warn("Resources don't have \"%s\"", field_shaders);
        return;
    }
    if (!j.contains(field_textures)) {
        PrototypeLogger::warn("Resources don't have \"%s\"", field_textures);
        return;
    }
    if (!j.contains(field_materials)) {
        PrototypeLogger::warn("Resources don't have \"%s\"", field_materials);
        return;
    }
    if (!j.contains(field_framebuffers)) {
        PrototypeLogger::warn("Resources don't have \"%s\"", field_framebuffers);
        return;
    }

    std::thread texturesThread([&]() {
        for (auto jtexture : j.at(field_textures)) { PrototypeTextureBuffer::from_json(jtexture); }
    });

    std::thread shadersThread([&]() {
        for (auto jshader : j.at(field_shaders)) { PrototypeShaderBuffer::from_json(jshader); }
    });

    std::thread meshesThread([&]() {
        PrototypeMeshBuffer::from_json(colored_triangle_2d);
        PrototypeMeshBuffer::from_json(colored_plane_2d);
        PrototypeMeshBuffer::from_json(colored_textured_plane_2d);
        PrototypeMeshBuffer::from_json(plane);
        PrototypeMeshBuffer::from_json(cube);
        for (auto jmesh : j.at(field_meshes)) { PrototypeMeshBuffer::from_json(jmesh); }
    });

    meshesThread.join();
    shadersThread.join();
    texturesThread.join();

    for (auto jmaterial : j.at(field_materials)) { PrototypeMaterial::from_json(jmaterial); }

    for (auto jframebuffer : j.at(field_framebuffers)) { PrototypeFrameBuffer::from_json(jframebuffer); }
}

void
PrototypeSceneLoader::loadPrototypeSceneFromFile(const char* filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        PrototypeLogger::warn("Couldn't load scene from <%s>", filepath);
        return;
    }
    nlohmann::json j;
    file >> j;
    file.close();

    if (j.is_null()) { return; }

    auto optScene = PrototypeScene::from_json(j);
    if (optScene.has_value()) {
        auto scene = optScene.value();
        PrototypeEngineInternalApplication::database->scenes.insert({ scene->name(), scene });
    }
}

static void
tinygltfLoadMaterials(const BundleConfig config, tinygltf::Model& gltfModel)
{
    for (tinygltf::Material& mat : gltfModel.materials) {
        std::stringstream uniqueMaterialName;
        uniqueMaterialName << config.filepath << "/" << mat.name;

        if (PrototypeEngineInternalApplication::database->materials.find(uniqueMaterialName.str()) !=
            PrototypeEngineInternalApplication::database->materials.end()) {
            continue;
        }

        glm::vec3 baseColor(1.0f, 1.0f, 1.0f);
        f32       metallic  = 0.0f;
        f32       roughness = 0.0f;

        // if (mat.values.find("baseColorTexture") != mat.values.end()) {
        //     material.baseColorTexture       = &textures[mat.values["baseColorTexture"].TextureIndex()];
        //     material.texCoordSets.baseColor = mat.values["baseColorTexture"].TextureTexCoord();
        // }
        // if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
        //     material.metallicRoughnessTexture       = &textures[mat.values["metallicRoughnessTexture"].TextureIndex()];
        //     material.texCoordSets.metallicRoughness = mat.values["metallicRoughnessTexture"].TextureTexCoord();
        // }
        if (mat.values.find("roughnessFactor") != mat.values.end()) {
            roughness = static_cast<float>(mat.values["roughnessFactor"].Factor());
        }
        if (mat.values.find("metallicFactor") != mat.values.end()) {
            metallic = static_cast<float>(mat.values["metallicFactor"].Factor());
        }
        if (mat.values.find("baseColorFactor") != mat.values.end()) {
            baseColor.r = mat.values["baseColorFactor"].ColorFactor()[0];
            baseColor.g = mat.values["baseColorFactor"].ColorFactor()[1];
            baseColor.b = mat.values["baseColorFactor"].ColorFactor()[2];
        }
        // if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
        //     material.normalTexture       = &textures[mat.additionalValues["normalTexture"].TextureIndex()];
        //     material.texCoordSets.normal = mat.additionalValues["normalTexture"].TextureTexCoord();
        // }
        // if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
        //     material.emissiveTexture       = &textures[mat.additionalValues["emissiveTexture"].TextureIndex()];
        //     material.texCoordSets.emissive = mat.additionalValues["emissiveTexture"].TextureTexCoord();
        // }
        // if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
        //     material.occlusionTexture       = &textures[mat.additionalValues["occlusionTexture"].TextureIndex()];
        //     material.texCoordSets.occlusion = mat.additionalValues["occlusionTexture"].TextureTexCoord();
        // }
        // if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
        //     tinygltf::Parameter param = mat.additionalValues["alphaMode"];
        //     if (param.string_value == "BLEND") { material.alphaMode = Material::ALPHAMODE_BLEND; }
        //     if (param.string_value == "MASK") {
        //         material.alphaCutoff = 0.5f;
        //         material.alphaMode   = Material::ALPHAMODE_MASK;
        //     }
        // }
        // if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
        //     material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
        // }
        // if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end()) {
        //     material.emissiveFactor =
        //     glm::vec4(glm::make_vec3(mat.additionalValues["emissiveFactor"].ColorFactor().data()), 1.0);
        //     material.emissiveFactor = glm::vec4(0.0f);
        // }

        // Extensions
        // @TODO: Find out if there is a nicer way of reading these properties with recent tinygltf headers
        // if (mat.extensions.find("KHR_materials_pbrSpecularGlossiness") != mat.extensions.end()) {
        //     auto ext = mat.extensions.find("KHR_materials_pbrSpecularGlossiness");
        //     if (ext->second.Has("specularGlossinessTexture")) {
        //         auto index                                   = ext->second.Get("specularGlossinessTexture").Get("index");
        //         material.extension.specularGlossinessTexture = &textures[index.Get<int>()];
        //         auto texCoordSet                             = ext->second.Get("specularGlossinessTexture").Get("texCoord");
        //         material.texCoordSets.specularGlossiness     = texCoordSet.Get<int>();
        //         material.pbrWorkflows.specularGlossiness     = true;
        //     }
        //     if (ext->second.Has("diffuseTexture")) {
        //         auto index                        = ext->second.Get("diffuseTexture").Get("index");
        //         material.extension.diffuseTexture = &textures[index.Get<int>()];
        //     }
        //     if (ext->second.Has("diffuseFactor")) {
        //         auto factor = ext->second.Get("diffuseFactor");
        //         for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
        //             auto val                            = factor.Get(i);
        //             material.extension.diffuseFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
        //         }
        //     }
        //     if (ext->second.Has("specularFactor")) {
        //         auto factor = ext->second.Get("specularFactor");
        //         for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
        //             auto val                             = factor.Get(i);
        //             material.extension.specularFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
        //         }
        //     }
        // }

        // let's create a new material with a per scene unique name
        // create a new material
        nlohmann::json j = {
            { "name", uniqueMaterialName.str() },
            { "shader", "pbr" },
            { "textures",
              { PROTOTYPE_DEFAULT_TEXTURE, PROTOTYPE_DEFAULT_TEXTURE, PROTOTYPE_DEFAULT_TEXTURE, PROTOTYPE_DEFAULT_TEXTURE } },
            { "baseColor", { baseColor.r, baseColor.g, baseColor.b } },
            { "metallic", metallic },
            { "roughness", roughness }
        };
        PrototypeMaterial::from_json(j);
    }
}

static void
tinygltfProcessMesh(const BundleConfig     config,
                    PrototypeScene*        scene,
                    const tinygltf::Model& gltfModel,
                    const tinygltf::Node&  gltfNode,
                    const tinygltf::Mesh&  gltfMesh,
                    glm::mat4              modelMatrix)
{
    if (!config.loaded) return;

    std::string meshName = std::string(config.filepath).append("/").append(gltfMesh.name);
    // add node to prototype scene
    {
        PrototypeSceneNode* sceneNode = PrototypeEngineInternalApplication::database->allocateSceneNode(gltfNode.name);
        const auto          optObject = sceneNode->object();
        assert(optObject.has_value() && "Node must automatically create an object in the construction phase !");
        if (optObject.has_value()) {
            PrototypeObject* object = optObject.value();
            object->setParentNode(static_cast<void*>(sceneNode));
            {
                std::stringstream uniqueMaterialName;
                int               materialIndex = -1;
                for (size_t p = 0; p < gltfMesh.primitives.size(); ++p) {
                    if (gltfMesh.primitives[p].material > -1) {
                        materialIndex = gltfMesh.primitives[p].material;
                        break;
                    }
                }
                if (materialIndex > -1) {
                    uniqueMaterialName << config.filepath << "/" << gltfModel.materials[materialIndex].name;
                } else {
                    uniqueMaterialName << PROTOTYPE_DEFAULT_MATERIAL;
                }

                object->addMeshRendererTrait();
                MeshRenderer*  mr = object->getMeshRendererTrait();
                nlohmann::json j  = { { "name", "MeshRenderer" },
                                     { "data", { { { "mesh", meshName }, { "material", uniqueMaterialName.str() } } } } };
                MeshRenderer::from_json(j, *mr, object);
            }
            if (!config.colliders.empty()) {
                {
                    object->addRigidbodyTrait();
                    Rigidbody*     rb = object->getRigidbodyTrait();
                    nlohmann::json j  = { { "name", "Rigidbody" }, { "static", config.isStatic } };
                    Rigidbody::from_json(j, *rb, object);
                }
                {
                    object->addColliderTrait();
                    Collider*      collider = object->getColliderTrait();
                    nlohmann::json j        = { { "name", "Collider" },
                                         { "nameRef", gltfMesh.name },
                                         { "shapeType", config.colliders },
                                         { "width", 1.0 },
                                         { "height", 1.0 },
                                         { "depth", 1.0 } };
                    Collider::from_json(j, *collider, object);
                }
            }

            object->addTransformTrait();
            glm::vec3  position(0.0f, 0.0f, 0.0f);
            glm::vec3  rotation(0.0f, 0.0f, 0.0f);
            glm::vec3  scale(1.0f, 1.0f, 1.0f);
            Transform* tr = object->getTransformTrait();
            PrototypeMaths::decomposeModelMatrix(modelMatrix, position, rotation, scale);
            nlohmann::json j = { { "name", "Transform" },
                                 { "position", { position.x, position.y, position.z } },
                                 { "rotation", { rotation.x, rotation.y, rotation.z } },
                                 { "scale", { scale.x, scale.y, scale.z } } };
            Transform::from_json(j, *tr, object);
        }
        auto defaultLayer = scene->layers().begin()->second;
        sceneNode->setParentLayer(defaultLayer);
        sceneNode->setParentNode(nullptr);
        defaultLayer->addNode(sceneNode);
        PrototypeEngineInternalApplication::database->sceneNodes[scene].insert({ sceneNode->name(), sceneNode });
    }
}

static void
tinygltfProcessNode(const BundleConfig    config,
                    PrototypeScene*       scene,
                    tinygltf::Model&      gltfModel,
                    const tinygltf::Node& gltfNode,
                    glm::mat4             modelMatrix)
{
    // Apply xform
    if (gltfNode.matrix.size() == 16) {
        // Use `matrix' attribute
        glm::mat4 localMat = glm::make_mat4(gltfNode.matrix.data());
        modelMatrix        = localMat;
    } else {
        // Assume Trans x Rotate x Scale order
        glm::mat4 scaleMatrix(1.0f), rotationMatrix(1.0f), translationMatrix(1.0f);

        if (gltfNode.scale.size() == 3) {
            scaleMatrix = glm::scale(scaleMatrix, glm::vec3(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]));
        }

        if (gltfNode.rotation.size() == 4) {
            glm::quat q;
            q.x            = gltfNode.rotation[0];
            q.y            = gltfNode.rotation[1];
            q.z            = gltfNode.rotation[2];
            q.w            = gltfNode.rotation[3];
            rotationMatrix = glm::toMat4(glm::rotate(q, glm::radians(180.0f), glm::vec3(1.0f, 0.0, 0.0f)));
        }

        if (gltfNode.translation.size() == 3) {
            translationMatrix = glm::translate(
              translationMatrix, glm::vec3(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]));
        }

        modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    }

    if (gltfNode.mesh > -1) {
        assert(gltfNode.mesh < gltfModel.meshes.size());
        tinygltfProcessMesh(config, scene, gltfModel, gltfNode, gltfModel.meshes[gltfNode.mesh], modelMatrix);
    }

    // Draw child nodes.
    for (size_t i = 0; i < gltfNode.children.size(); i++) {
        assert(gltfNode.children[i] < gltfModel.nodes.size());
        tinygltfProcessNode(config, scene, gltfModel, gltfModel.nodes[gltfNode.children[i]], modelMatrix);
    }
}

static void
tinygltfProcessModel(const BundleConfig config, PrototypeScene* scene, tinygltf::Model& gltfModel)
{
    int                    scene_to_display = gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0;
    const tinygltf::Scene& gltfScene        = gltfModel.scenes[scene_to_display];
    for (size_t i = 0; i < gltfScene.nodes.size(); i++) {
        tinygltfProcessNode(config, scene, gltfModel, gltfModel.nodes[gltfScene.nodes[i]], glm::mat4(1.0f));
    }
}

void
PrototypeSceneLoader::tinygltfImportScene(BundleConfig config, PrototypeScene* scene)
{
    tinygltf::Model    gltfModel;
    tinygltf::TinyGLTF gltfLoader;
    std::string        err;
    std::string        warn;

    std::string gltfFilenameFullpath = std::string(PROTOTYPE_BUNDLE_PATH("")).append(config.filepath);

    bool ret = true;

    std::filesystem::path p(gltfFilenameFullpath);
    if (p.extension().string() == ".glb") {
        ret = gltfLoader.LoadBinaryFromFile(&gltfModel, &err, &warn, gltfFilenameFullpath.c_str());
    } else if (p.extension().string() == ".gltf") {
        ret = gltfLoader.LoadASCIIFromFile(&gltfModel, &err, &warn, gltfFilenameFullpath.c_str());
    } else {
        PrototypeLogger::warn("Not a gltf file %s\n%s", gltfFilenameFullpath.c_str());
        PrototypeLogger::warn("%s", warn.c_str());
        PrototypeLogger::error("%s", err.c_str());
        return;
    }

    if (!warn.empty()) { PrototypeLogger::warn("%s", warn.c_str()); }

    if (!err.empty()) { PrototypeLogger::error("%s", err.c_str()); }

    if (!ret) {
        PrototypeLogger::error("Failed to parse glTF file %s", gltfFilenameFullpath.c_str());
        return;
    }

    std::unordered_map<int, std::string> meshesIdToNameMap;
    {
        for (size_t m = 0; m < gltfModel.meshes.size(); m++) {
            tinygltf::Mesh&                            modelMesh        = gltfModel.meshes[m];
            std::unique_ptr<PrototypeMeshBufferSource> meshBufferSource = std::make_unique<PrototypeMeshBufferSource>();
            meshBufferSource->type                                      = PrototypeMeshBufferType_Constant_Triangles_3D;

            for (size_t p = 0; p < modelMesh.primitives.size(); ++p) {
                tinygltf::Primitive& modelMeshPrimitive = modelMesh.primitives[p];
                if (modelMeshPrimitive.mode != 4) {
                    // i only use the GL_TRIANGLES mesh primitives mode for now ..
                    continue;
                }
                {
                    // indices
                    {
                        int                 indicesRef      = modelMeshPrimitive.indices;
                        tinygltf::Accessor& accessor        = gltfModel.accessors[indicesRef];
                        int                 bufferViewRef   = accessor.bufferView;
                        size_t              sizeOfComponent = 1;
                        if (accessor.componentType == 5120) { // GL_BYTE
                            sizeOfComponent = sizeof(i8);
                        } else if (accessor.componentType == 5121) { // GL_UNSIGNED_BYTE
                            sizeOfComponent = sizeof(u8);
                        } else if (accessor.componentType == 5122) { // GL_SHORT
                            sizeOfComponent = sizeof(i16);
                        } else if (accessor.componentType == 5123) { // GL_UNSIGNED_SHORT
                            sizeOfComponent = sizeof(u16);
                        } else if (accessor.componentType == 5124) { // GL_INT
                            sizeOfComponent = sizeof(i32);
                        } else if (accessor.componentType == 5125) { // GL_UNSIGNED_INT
                            sizeOfComponent = sizeof(u32);
                        } else if (accessor.componentType == 5126) { // GL_FLOAT
                            sizeOfComponent = sizeof(f32);
                        }
                        if (bufferViewRef > -1) {
                            tinygltf::BufferView& bufferview   = gltfModel.bufferViews[bufferViewRef];
                            int                   bufferRef    = bufferview.buffer;
                            tinygltf::Buffer&     buffer       = gltfModel.buffers[bufferRef];
                            size_t                bufferLength = bufferview.byteLength;
                            size_t                offset       = bufferview.byteOffset + accessor.byteOffset;
                            size_t                stride       = bufferview.byteStride;
                            int                   target       = bufferview.target;
                            size_t                count        = accessor.count;
                            std::vector<uint8_t>& data         = buffer.data;
                            std::string&          uri          = buffer.uri;

                            if (!data.empty()) {
                                size_t numBatch  = bufferLength;
                                size_t nextIndex = sizeOfComponent;
                                if (stride > 0) {
                                    numBatch /= stride;
                                    nextIndex = stride;
                                }
                                meshBufferSource->indices.resize(count);
                                for (size_t j = 0; j < count; ++j) {
                                    std::memcpy(&meshBufferSource->indices[j], &data[offset], sizeOfComponent);
                                    offset += nextIndex;
                                }
                            }
                        }
                    }

                    // vertices
                    for (const auto& attribute : modelMeshPrimitive.attributes) {
                        tinygltf::Accessor& accessor              = gltfModel.accessors[attribute.second];
                        int                 verticesBufferViewRef = accessor.bufferView;
                        size_t              sizeOfComponent       = 1;
                        if (accessor.componentType == 5120) { // GL_BYTE
                            sizeOfComponent = sizeof(i8);
                        } else if (accessor.componentType == 5121) { // GL_UNSIGNED_BYTE
                            sizeOfComponent = sizeof(u8);
                        } else if (accessor.componentType == 5122) { // GL_SHORT
                            sizeOfComponent = sizeof(i16);
                        } else if (accessor.componentType == 5123) { // GL_UNSIGNED_SHORT
                            sizeOfComponent = sizeof(u16);
                        } else if (accessor.componentType == 5124) { // GL_INT
                            sizeOfComponent = sizeof(i32);
                        } else if (accessor.componentType == 5125) { // GL_UNSIGNED_INT
                            sizeOfComponent = sizeof(u32);
                        } else if (accessor.componentType == 5126) { // GL_FLOAT
                            sizeOfComponent = sizeof(f32);
                        }
                        tinygltf::BufferView& bufferview = gltfModel.bufferViews[verticesBufferViewRef];
                        if (verticesBufferViewRef > -1) {
                            int                   bufferRef    = bufferview.buffer;
                            tinygltf::Buffer&     buffer       = gltfModel.buffers[bufferRef];
                            size_t                bufferLength = bufferview.byteLength;
                            size_t                offset       = bufferview.byteOffset + accessor.byteOffset;
                            size_t                stride       = bufferview.byteStride;
                            int                   target       = bufferview.target;
                            size_t                count        = accessor.count;
                            std::vector<uint8_t>& data         = buffer.data;
                            std::string&          uri          = buffer.uri;
                            meshBufferSource->vertices.resize(count);

                            if (!data.empty()) {
                                size_t numBatch     = bufferLength;
                                size_t numPositions = numBatch * 3;
                                size_t numNormals   = numBatch * 3;
                                size_t numTexcoords = numBatch * 2;
                                if (attribute.first == "POSITION") {
                                    size_t nextIndex = 3 * sizeOfComponent;
                                    if (stride > 0) {
                                        numBatch /= stride;
                                        nextIndex = stride;
                                    }
                                    /*snprintf(printBuff, sizeof(printBuff), "POSITION: %i", attribute.second);
                                    PrototypeLogger::log(__FILE__, __LINE__, printBuff);*/
                                    size_t i = offset;
                                    for (size_t j = 0; j < count; ++j) {
                                        std::memcpy(&meshBufferSource->vertices[j].positionU.x, &data[i], 3 * sizeOfComponent);
                                        i += nextIndex;
                                    }
                                } else if (attribute.first == "NORMAL") {
                                    size_t nextIndex = 3 * sizeOfComponent;
                                    if (stride > 0) {
                                        numBatch /= stride;
                                        nextIndex = stride;
                                    }
                                    /*snprintf(printBuff, sizeof(printBuff), "NORMAL: %i", attribute.second);
                                    PrototypeLogger::log(__FILE__, __LINE__, printBuff);*/
                                    size_t i = offset;
                                    for (size_t j = 0; j < count; ++j) {
                                        std::memcpy(&meshBufferSource->vertices[j].normalV.x, &data[i], 3 * sizeOfComponent);
                                        i += nextIndex;
                                    }
                                } else if (attribute.first == "TEXCOORD_0") {
                                    size_t nextIndex = 2 * sizeOfComponent;
                                    if (stride > 0) {
                                        numBatch /= stride;
                                        nextIndex = stride;
                                    }
                                    /*snprintf(printBuff, sizeof(printBuff), "TEXCOORD_0: %i", attribute.second);
                                    PrototypeLogger::log(__FILE__, __LINE__, printBuff);*/
                                    size_t i = offset;
                                    for (size_t j = 0; j < count; ++j) {
                                        std::memcpy(&meshBufferSource->vertices[j].positionU.w, &data[i], sizeOfComponent);
                                        std::memcpy(
                                          &meshBufferSource->vertices[j].normalV.w, &data[i + sizeOfComponent], sizeOfComponent);
                                        i += nextIndex;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            std::string meshName = modelMesh.name;
            if (meshName.empty()) {
                static size_t     nameCounter = 0;
                std::stringstream ss;
                ss << "UntitledGltfMesh " << ++nameCounter;
                meshName = ss.str();
            }
            std::string meshPath = std::string(config.filepath).append("/").append(meshName);
            std::string meshFullpath =
              std::string(PROTOTYPE_BUNDLE_PATH("")).append(config.filepath).append("/").append(meshName);
            PrototypeMeshBuffer* meshBuffer = PrototypeEngineInternalApplication::database->allocateMeshBuffer(meshPath);
            meshBuffer->setFullpath(meshFullpath);
            meshBuffer->setTimestamp(PrototypeIo::filestamp(meshFullpath));
            meshBuffer->setSource(std::move(meshBufferSource));
            PrototypeEngineInternalApplication::database->meshBuffers.insert({ meshPath, meshBuffer });
            meshesIdToNameMap.insert({ m, meshPath });
        }
    }

    tinygltfLoadMaterials(config, gltfModel);
    tinygltfProcessModel(config, scene, gltfModel);
}

static std::unique_ptr<PrototypeMeshBufferSource>
assimpProcessMesh(const BundleConfig config, const aiScene* assimpScene, aiNode* assimpNode, aiMesh* assimpMesh)
{
    auto meshBufferSource  = std::make_unique<PrototypeMeshBufferSource>();
    meshBufferSource->type = PrototypeMeshBufferType_Triangles;
    meshBufferSource->vertices.clear();
    meshBufferSource->indices.clear();

    for (u32 v = 0; v < assimpMesh->mNumVertices; ++v) {
        PrototypeMeshVertex vertex = {};
        vertex.positionU.x         = assimpMesh->mVertices[v].x;
        vertex.positionU.y         = assimpMesh->mVertices[v].y;
        vertex.positionU.z         = assimpMesh->mVertices[v].z;

        vertex.normalV.x = assimpMesh->mNormals[v].x;
        vertex.normalV.y = assimpMesh->mNormals[v].y;
        vertex.normalV.z = assimpMesh->mNormals[v].z;

        vertex.positionU.w = assimpMesh->mTextureCoords[0][v].x;
        vertex.normalV.w   = assimpMesh->mTextureCoords[0][v].y;

        aiMaterial* material = assimpScene->mMaterials[assimpMesh->mMaterialIndex];
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
        meshBufferSource->vertices.emplace_back(std::move(vertex));
    }
    for (u32 f = 0; f < assimpMesh->mNumFaces; ++f) {
        aiFace face = assimpMesh->mFaces[f];
        for (u32 i = 0; i < face.mNumIndices; ++i) { meshBufferSource->indices.push_back(face.mIndices[i]); }
    }

    return std::move(meshBufferSource);
}

static void
assimpProcessNode(const BundleConfig      config,
                  const aiScene*          assimpScene,
                  aiNode*                 assimpNode,
                  PrototypeScene*         scene,
                  std::vector<glm::mat4>& st)
{
    C_STRUCT aiMatrix4x4 mat = assimpNode->mTransformation;
    aiTransposeMatrix4(&mat);
    glm::mat4 NodeMatrix = glm::make_mat4((float*)&mat);
    NodeMatrix           = st.back() * NodeMatrix;

    if (assimpNode->mNumMeshes > 0) {
        std::stringstream uniqueNodeName;
        uniqueNodeName << config.filepath << "/" << assimpScene->mName.C_Str() << "/" << assimpNode->mName.C_Str();

        std::vector<MeshRendererMeshMaterialTuple> meshMaterialPairs;

        // process all the node's meshes (if any)
        for (unsigned int i = 0; i < assimpNode->mNumMeshes; ++i) {
            aiMesh* assimpMesh       = assimpScene->mMeshes[assimpNode->mMeshes[i]];
            auto    meshBufferSource = assimpProcessMesh(config, assimpScene, assimpNode, assimpMesh);

            // Material //
            std::stringstream uniqueMaterialName;
            uniqueMaterialName << config.filepath << "/"
                               << assimpScene->mMaterials[assimpMesh->mMaterialIndex]->GetName().C_Str();
            if (PrototypeEngineInternalApplication::database->materials.find(uniqueMaterialName.str()) ==
                PrototypeEngineInternalApplication::database->materials.end()) {
                // let's create a new material with a per scene unique name
                // create a new material
                nlohmann::json j = { { "name", uniqueMaterialName.str() },
                                     { "shader", "pbr" },
                                     { "textures",
                                       { PROTOTYPE_DEFAULT_TEXTURE,
                                         PROTOTYPE_DEFAULT_TEXTURE,
                                         PROTOTYPE_DEFAULT_TEXTURE,
                                         PROTOTYPE_DEFAULT_TEXTURE } },
                                     { "baseColor", { 1.0f, 1.0f, 1.0f } },
                                     { "metallic", 0.0f },
                                     { "roughness", 0.6f } };
                PrototypeMaterial::from_json(j);
            }

            // Mesh //
            std::stringstream uniqueMeshName;
            uniqueMeshName << config.filepath << "/" << assimpMesh->mName.C_Str();
            if (PrototypeEngineInternalApplication::database->meshBuffers.find(uniqueMeshName.str()) ==
                PrototypeEngineInternalApplication::database->meshBuffers.end()) {
                PrototypeMeshBuffer* meshBuffer =
                  PrototypeEngineInternalApplication::database->allocateMeshBuffer(uniqueMeshName.str());
                meshBuffer->setFullpath(uniqueMeshName.str());
                meshBuffer->setTimestamp(PrototypeIo::filestamp(meshBuffer->fullpath()));
                meshBuffer->setSource(std::move(meshBufferSource));
                PrototypeEngineInternalApplication::database->meshBuffers.insert({ meshBuffer->name(), meshBuffer });
            }

            MeshRendererMeshMaterialTuple mmt = {};
            mmt.material                      = uniqueMaterialName.str();
            mmt.mesh                          = uniqueMeshName.str();
            mmt.polygonMode                   = MeshRendererPolygonMode_FILL;
            meshMaterialPairs.push_back(std::move(mmt));
        }

        // add node to prototype scene
        if (config.loaded) {
            PrototypeSceneNode* sceneNode = PrototypeEngineInternalApplication::database->allocateSceneNode(uniqueNodeName.str());
            const auto          optObject = sceneNode->object();
            assert(optObject.has_value() && "Node must automatically create an object in the construction phase !");
            if (optObject.has_value()) {
                PrototypeObject* object = optObject.value();
                object->setParentNode(static_cast<void*>(sceneNode));
                {
                    nlohmann::json jmeshMaterialPairs = {};

                    for (const auto& mmpair : meshMaterialPairs) {
                        jmeshMaterialPairs.push_back({ { "mesh", mmpair.mesh }, { "material", mmpair.material } });
                    }

                    object->addMeshRendererTrait();
                    MeshRenderer*  mr = object->getMeshRendererTrait();
                    nlohmann::json j  = { { "name", "MeshRenderer" }, { "data", jmeshMaterialPairs } };
                    MeshRenderer::from_json(j, *mr, object);
                }
                // if (!config.colliders.empty()) {
                //     {
                //         object->addRigidbodyTrait();
                //         Rigidbody*     rb = object->getRigidbodyTrait();
                //         nlohmann::json j  = { { "name", "Rigidbody" }, { "static", config.isStatic } };
                //         Rigidbody::from_json(j, *rb, object);
                //     }
                //     {
                //         object->addColliderTrait();
                //         Collider*      collider = object->getColliderTrait();
                //         nlohmann::json j        = { { "name", "Collider" },
                //                              { "nameRef", modelMesh.name },
                //                              { "shapeType", config.colliders },
                //                              { "width", 1.0 },
                //                              { "height", 1.0 },
                //                              { "depth", 1.0 } };
                //         Collider::from_json(j, *collider, object);
                //     }
                // }

                object->addTransformTrait();
                glm::vec3  position(0.0f, 0.0f, 0.0f);
                glm::vec3  rotation(0.0f, 0.0f, 0.0f);
                glm::vec3  scale(1.0f, 1.0f, 1.0f);
                Transform* tr = object->getTransformTrait();
                PrototypeMaths::decomposeModelMatrix(NodeMatrix, position, rotation, scale);
                nlohmann::json j = { { "name", "Transform" },
                                     { "position", { position.x, position.y, position.z } },
                                     { "rotation", { rotation.x, rotation.y, rotation.z } },
                                     { "scale", { scale.x, scale.y, scale.z } } };
                Transform::from_json(j, *tr, object);
            }
            auto defaultLayer = scene->layers().begin()->second;
            sceneNode->setParentLayer(defaultLayer);
            sceneNode->setParentNode(nullptr);
            defaultLayer->addNode(sceneNode);
            PrototypeEngineInternalApplication::database->sceneNodes[scene].insert({ sceneNode->name(), sceneNode });
        }
    }

    st.push_back(NodeMatrix);

    // then do the same for each of its children
    for (u32 i = 0; i < assimpNode->mNumChildren; ++i) {
        assimpProcessNode(config, assimpScene, assimpNode->mChildren[i], scene, st);
    }

    st.pop_back();
}

// static void
// recursiveLoadScene(const BundleConfig                                   config,
//                    const aiScene*                                       assimpScene,
//                    const aiNode*                                        assimpSceneNode,
//                    PrototypeScene*                                      scene,
//                    std::vector<glm::mat4>&                              st,
//                    std::unordered_map<std::string, PrototypeMaterial*>& materials)
// {
//     if (!assimpSceneNode) return;

//     MASK_TYPE traitMask = PrototypeTraitTypeMaskTransform;
//     if (!config.colliders.empty()) { traitMask |= PrototypeTraitTypeMaskRigidbody | PrototypeTraitTypeMaskCollider; }
//     if (assimpSceneNode->mNumMeshes > 0) { traitMask |= PrototypeTraitTypeMaskMeshRenderer; }
//     PrototypeObject* object =
//       shotcutCreateCloneObjectToLayer(assimpSceneNode->mName.C_Str(), traitMask, scene->layers().begin()->second);
//     if (object->hasMeshRendererTrait()) { object->getMeshRendererTrait()->data().resize(assimpSceneNode->mNumMeshes); }

//     C_STRUCT aiMatrix4x4 m = assimpSceneNode->mTransformation;
//     if (assimpSceneNode->mParent) { m = m * assimpSceneNode->mParent->mTransformation; }
//     aiTransposeMatrix4(&m);
//     // multiply this m by parent node m
//     glm::mat4 mm = glm::make_mat4((float*)&m);
//     // mm           = st.back() * mm;
//     st.push_back(mm);

//     object->getTransformTrait()->positionMut() = glm::vec3(0.0f, 0.0f, 0.0f);
//     object->getTransformTrait()->rotationMut() = glm::vec3(0.0f, 0.0f, 0.0f);
//     object->getTransformTrait()->scaleMut()    = glm::vec3(mm[0][0], mm[1][1], mm[2][2]);
//     PrototypeMaths::decomposeModelMatrix(mm,
//                                          object->getTransformTrait()->positionMut(),
//                                          object->getTransformTrait()->rotationMut(),
//                                          object->getTransformTrait()->scaleMut());
//     object->getTransformTrait()->setModelScaled(&mm[0][0]);
//     object->getTransformTrait()->updateComponentsFromMatrix();

//     // draw all meshes assigned to this node (all meshes and materials assigned to the mesh renderer ..)
//     for (u32 mi = 0; mi < assimpSceneNode->mNumMeshes; ++mi) {
//         const C_STRUCT aiMesh* sceneMesh = assimpScene->mMeshes[assimpSceneNode->mMeshes[mi]];
//         // TODO: maybe try to get color as well .. ?

//         std::stringstream uniqueMaterialName;
//         uniqueMaterialName << config.filepath << "/" << assimpScene->mMaterials[sceneMesh->mMaterialIndex]->GetName().C_Str();
//         if (materials.find(uniqueMaterialName.str()) == materials.end()) {
//             // let's create a new material with a per scene unique name
//             // create a new material
//             nlohmann::json j = { { "name", uniqueMaterialName.str() },
//                                  { "shader", "pbr" },
//                                  { "textures",
//                                    { "default.jpg", "Metal01/metallic.jpg", "Metal01/normal.jpg", "Metal01/roughness.jpg" } },
//                                  { "baseColor", { 1.0f, 1.0f, 1.0f } },
//                                  { "metallic", 0.0f },
//                                  { "roughness", 0.6f } };
//             PrototypeMaterial::from_json(j);
//         }

//         auto meshBufferSource  = std::make_unique<PrototypeMeshBufferSource>();
//         meshBufferSource->type = PrototypeMeshBufferType_Triangles;
//         meshBufferSource->vertices.clear();
//         meshBufferSource->indices.clear();
//         std::unordered_map<PrototypeMeshVertex, u32> uniqueVertices{};

//         for (u32 v = 0; v < sceneMesh->mNumVertices; ++v) {
//             PrototypeMeshVertex vertex = {};
//             vertex.positionU.x         = sceneMesh->mVertices[v].x;
//             vertex.positionU.y         = sceneMesh->mVertices[v].y;
//             vertex.positionU.z         = sceneMesh->mVertices[v].z;

//             vertex.normalV.x = sceneMesh->mNormals[v].x;
//             vertex.normalV.y = sceneMesh->mNormals[v].y;
//             vertex.normalV.z = sceneMesh->mNormals[v].z;

//             vertex.positionU.w = sceneMesh->mTextureCoords[0][v].x;
//             vertex.normalV.w   = sceneMesh->mTextureCoords[0][v].y;

//             aiMaterial* material = assimpScene->mMaterials[sceneMesh->mMaterialIndex];
//             if (material) {
//                 aiColor3D color;
//                 material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
//                 vertex.color.x = color.r;
//                 vertex.color.y = color.g;
//                 vertex.color.z = color.b;
//                 vertex.color.w = 1.0f;
//             } else {
//                 vertex.color.x = 1.0f;
//                 vertex.color.y = 1.0f;
//                 vertex.color.z = 1.0f;
//                 vertex.color.w = 1.0f;
//             }

//             if (uniqueVertices.count(vertex) == 0) {
//                 uniqueVertices[vertex] = (u32)meshBufferSource->vertices.size();
//                 meshBufferSource->vertices.push_back(vertex);
//             }
//             meshBufferSource->indices.push_back(uniqueVertices[vertex]);
//         }
//     }

//     /* draw all children */
//     for (u32 childi = 0; childi < assimpSceneNode->mNumChildren; ++childi) {
//         recursiveLoadScene(config, assimpScene, assimpSceneNode->mChildren[childi], scene, st, materials);
//     }
//     st.pop_back();
// }

void
PrototypeSceneLoader::assimpImportScene(BundleConfig config, PrototypeScene* scene)
{
    std::string filenameFullpath = std::string(PROTOTYPE_BUNDLE_PATH("")).append(config.filepath);

    i32              defaultFlags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
    Assimp::Importer importer;
    const aiScene*   assimpScene = importer.ReadFile(filenameFullpath, defaultFlags);
    if (!assimpScene || assimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimpScene->mRootNode) {
        PrototypeLogger::warn("Cannot import scene from file, file not found %s", filenameFullpath.c_str());
        return;
    }
    std::vector<glm::mat4> st;
    st.push_back(glm::mat4(1.0f));
    assimpProcessNode(config, assimpScene, assimpScene->mRootNode, scene, st);
    // create empty node to act as a parent to all nodes in that scene
    // loop on each node in the scene
    // if a node has a mesh import it
    // if the same node has a material fetch it
    // check if material is common by its name (maybe it was loaded before and in that case we need to use same material for
    // multiple objects) don't load textures for now .. just create materials .. we need to figure out relation of shaders as well
}