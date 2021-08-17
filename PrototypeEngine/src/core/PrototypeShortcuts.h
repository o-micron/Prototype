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

#include <PrototypeTraitSystem/PrototypeTraitSystemTypes.h>

#include <set>
#include <unordered_set>

struct PrototypeSceneLayer;
struct PrototypeSceneNode;
struct PrototypeObject;
struct Camera;
struct Transform;
struct MeshRenderer;
struct Collider;
struct Rigidbody;
struct Script;
struct ScriptCodeLink;
struct PrototypePluginInstance;
struct VehicleChasis;
struct VehicleWheel;

extern PrototypeObject*
shotcutCreateCloneObjectToLayer(const std::string& nodeName, MASK_TYPE traitMask, PrototypeSceneLayer* parentLayer);
extern PrototypeObject*
shotcutCreateCloneObjectToNode(const std::string& nodeName, MASK_TYPE traitMask, PrototypeSceneNode* parentNode);
extern void
shortcutSetupObjectMeshRendererTrait(PrototypeObject* object, const std::string& meshName, const std::string& materialName);

extern void
shortcutSetupObjectTransformTrait(PrototypeObject* object, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
extern void
shortcutSetupObjectPlaneColliderTrait(PrototypeObject* object, f32 width, f32 height, f32 radius, const glm::vec3& velocity);
extern void
shortcutSetupObjectSphereColliderTrait(PrototypeObject* object, f32 radius, const glm::vec3& velocity);

extern void
shortcutSetupObjectCubeColliderTrait(PrototypeObject* object, f32 width, f32 height, f32 depth, const glm::vec3& velocity);
extern void
shortcutSetupObjectConvexMeshColliderTrait(PrototypeObject* object, const glm::vec3& velocity);
extern void
shortcutSetupObjectTriMeshColliderTrait(PrototypeObject* object, const glm::vec3& velocity);
extern void
shortcutSetupObjectVehicleChasisWheelsTrait(PrototypeObject* chasisObject,
                                            PrototypeObject* wheelFRObject,
                                            PrototypeObject* wheelFLObject,
                                            PrototypeObject* wheelBRObject,
                                            PrototypeObject* wheelBLObject);

// Spawning
extern void
shortcutSpawnSphere(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& dir);
extern void
shortcutSpawnCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& dir);
extern void
shortcutSpawnConvexMesh(const glm::vec3&   position,
                        const glm::vec3&   rotation,
                        const glm::vec3&   dir,
                        const std::string& mesh,
                        const std::string& material);
extern void
shortcutSpawnTriMesh(const glm::vec3&   position,
                     const glm::vec3&   rotation,
                     const glm::vec3&   dir,
                     const std::string& mesh,
                     const std::string& material);

extern void
shortcutSpawnVehicle(const glm::vec3&   position,
                     const glm::vec3&   rotation,
                     const glm::vec3&   dir,
                     const std::string& mesh,
                     const std::string& material);

// Editor Creations
extern void
shortcutEditorAddSceneNodeToLayer(PrototypeSceneLayer* parentLayer, glm::vec3 position, glm::vec3 rotation, glm::vec3 dir);

extern void
shortcutEditorAddSceneNodeToNode(PrototypeSceneNode* parentNode, glm::vec3 position, glm::vec3 rotation, glm::vec3 dir);

// TODO:
// So far you cannot just remove a layer immediately
// Specially whilst in mid scene rendering of course ...
// wait until there is a better way to do that
// extern void
// shortcutEditorRemoveSceneLayer(PrototypeSceneLayer* layer);

extern void
shortcutEditorRemoveSceneNode(PrototypeSceneNode* node);

extern void
shortcutEditorRemoveSelectedSceneNode();

extern void
shortcutEditorSelectedSceneNodeAddTraits(PrototypeObject* object, MASK_TYPE traitMask);

extern void
shortcutEditorSelectedSceneNodeRemoveTraits(PrototypeObject* object, MASK_TYPE traitMask);

extern void
shortcutEditorRemoveScriptFromObject(PrototypeObject* object, std::string codeLink);

extern void
shortcutEditorCommitReloadPlugin(PrototypePluginInstance* pluginInstance);

// Default Traits Initializers
extern void
shortcutDefaultCameraTraitAddInitializer(PrototypeObject* object, Camera* camera);
extern void
shortcutDefaultCameraTraitReuseInitializer(PrototypeObject* object, Camera* camera);
extern void
shortcutDefaultCameraTraitRemoveInitializer(PrototypeObject* object, Camera* camera);
extern void
shortcutDefaultCameraTraitLogInitializer(PrototypeObject* object, Camera* camera);

extern void
shortcutDefaultColliderTraitAddInitializer(PrototypeObject* object, Collider* collider);
extern void
shortcutDefaultColliderTraitReuseInitializer(PrototypeObject* object, Collider* collider);
extern void
shortcutDefaultColliderTraitRemoveInitializer(PrototypeObject* object, Collider* collider);
extern void
shortcutDefaultColliderTraitLogInitializer(PrototypeObject* object, Collider* collider);

extern void
shortcutDefaultMeshRendererTraitAddInitializer(PrototypeObject* object, MeshRenderer* meshRenderer);
extern void
shortcutDefaultMeshRendererTraitReuseInitializer(PrototypeObject* object, MeshRenderer* meshRenderer);
extern void
shortcutDefaultMeshRendererTraitRemoveInitializer(PrototypeObject* object, MeshRenderer* meshRenderer);
extern void
shortcutDefaultMeshRendererTraitLogInitializer(PrototypeObject* object, MeshRenderer* meshRenderer);

extern void
shortcutDefaultRigidbodyTraitAddInitializer(PrototypeObject* object, Rigidbody* rigidbody);
extern void
shortcutDefaultRigidbodyTraitReuseInitializer(PrototypeObject* object, Rigidbody* rigidbody);
extern void
shortcutDefaultRigidbodyTraitRemoveInitializer(PrototypeObject* object, Rigidbody* rigidbody);
extern void
shortcutDefaultRigidbodyTraitLogInitializer(PrototypeObject* object, Rigidbody* rigidbody);

extern void
shortcutDefaultScriptTraitAddInitializer(PrototypeObject* object, Script* script);
extern void
shortcutDefaultScriptTraitReuseInitializer(PrototypeObject* object, Script* script);
extern void
shortcutDefaultScriptTraitRemoveInitializer(PrototypeObject* object, Script* script);
extern void
shortcutDefaultScriptTraitLogInitializer(PrototypeObject* object, Script* script);

extern void
shortcutDefaultTransformTraitAddInitializer(PrototypeObject* object, Transform* transform);
extern void
shortcutDefaultTransformTraitReuseInitializer(PrototypeObject* object, Transform* transform);
extern void
shortcutDefaultTransformTraitRemoveInitializer(PrototypeObject* object, Transform* transform);
extern void
shortcutDefaultTransformTraitLogInitializer(PrototypeObject* object, Transform* transform);

extern void
shortcutDefaultVehicleChasisTraitAddInitializer(PrototypeObject* object, VehicleChasis* vch);
extern void
shortcutDefaultVehicleChasisTraitReuseInitializer(PrototypeObject* object, VehicleChasis* vch);
extern void
shortcutDefaultVehicleChasisTraitRemoveInitializer(PrototypeObject* object, VehicleChasis* vch);
extern void
shortcutDefaultVehicleChasisTraitLogInitializer(PrototypeObject* object, VehicleChasis* vch);