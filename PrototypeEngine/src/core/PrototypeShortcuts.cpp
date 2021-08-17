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

#include "PrototypeShortcuts.h"

#include "PrototypeEngine.h"

#include "../../include/PrototypeEngine/PrototypeEngineApplication.h"

#include <PrototypeTraitSystem/PrototypeTraitSystemTypes.h>

#include "PrototypeCameraSystem.h"
#include "PrototypeDatabase.h"
#include "PrototypePluginInstance.h"
#include "PrototypeRenderer.h"
#include "PrototypeScene.h"
#include "PrototypeSceneLayer.h"
#include "PrototypeSceneNode.h"
#include "PrototypeUI.h"
#include "PrototypeWindow.h"

#include "PrototypeMeshBuffer.h"

#include "../physx/PrototypePhysxPhysics.h"

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include <PrototypeCommon/Logger.h>

#include <GLFW/glfw3.h>

#include <sstream>

#define DEFAULT_ANGULAR_DAMPING 1.0f
#define DEFAULT_LINEAR_DAMPING  1.0f

extern PrototypeObject*
shotcutCreateCloneObjectToLayer(const std::string& nodeName, MASK_TYPE traitMask, PrototypeSceneLayer* parentLayer)
{
    if (!parentLayer) return nullptr;

    if (!parentLayer->nodesByName(nodeName).has_value()) {
        auto       node      = PrototypeEngineInternalApplication::database->allocateSceneNode(nodeName);
        const auto optObject = node->object();
        if (optObject.has_value()) {
            PrototypeObject* object = optObject.value();
            object->setParentNode(static_cast<void*>(node));
            node->setParentLayer(parentLayer);
            parentLayer->addNode(node);
            object->add(traitMask);
            return object;
        }
    }
    return nullptr;
}

extern PrototypeObject*
shotcutCreateCloneObjectToNode(const std::string& nodeName, MASK_TYPE traitMask, PrototypeSceneNode* parentNode)
{
    if (!parentNode) return nullptr;

    if (!parentNode->nodesByName(nodeName).has_value()) {
        auto       node      = PrototypeEngineInternalApplication::database->allocateSceneNode(nodeName);
        const auto optObject = node->object();
        if (optObject.has_value()) {
            PrototypeObject* object = optObject.value();
            object->setParentNode(static_cast<void*>(node));
            node->setParentNode(parentNode);
            parentNode->addNode(node);
            object->add(traitMask);
            return object;
        }
    }
    return nullptr;
}

extern void
shortcutSetupObjectMeshRendererTrait(PrototypeObject* object, const std::string& meshName, const std::string& materialName)
{
    MeshRenderer* mr = object->getMeshRendererTrait();
    if (mr->data().empty()) {
        MeshRendererMeshMaterialTuple mrmmt = {};
        mrmmt.mesh                          = meshName;
        mrmmt.material                      = materialName;
        mrmmt.polygonMode                   = MeshRendererPolygonMode_FILL;
        mr->data().emplace_back(mrmmt);
    } else {
        mr->data()[0].mesh        = meshName;
        mr->data()[0].material    = materialName;
        mr->data()[0].polygonMode = MeshRendererPolygonMode_FILL;
    }
}

extern void
shortcutSetupObjectTransformTrait(PrototypeObject* object,
                                  const glm::vec3  position,
                                  const glm::vec3  rotation,
                                  const glm::vec3  scale)
{
    Transform* tr = object->getTransformTrait();
    glm::mat4  model;
    PrototypeMaths::buildModelMatrix(model, position, rotation);
    PrototypeMaths::buildModelMatrixWithScale(model, scale);
    tr->setModelScaled(&model[0][0]);
    tr->updateComponentsFromMatrix();
    tr->setNeedsPhysicsSync(false);
}

extern void
shortcutSetupObjectColliderTrait(PrototypeObject* object, const f32 radius)
{}

extern void
shortcutSetupObjectColliderTrait(PrototypeObject*     object,
                                 const ColliderShape_ shapeType,
                                 const f32            width,
                                 const f32            height,
                                 const f32            depth)
{
    Collider* cl = object->getColliderTrait();
    cl->setShapeType(shapeType);
    cl->setWidth(width);
    cl->setHeight(height);
    cl->setDepth(depth);
}

extern void
shortcutSetupObjectPlaneColliderTrait(PrototypeObject* object, const f32 width, const f32 height, const glm::vec3& velocity)
{
    Collider* cl = object->getColliderTrait();
    cl->setShapeType(ColliderShape_Plane);
    cl->setWidth(width);
    cl->setHeight(height);
    cl->setDepth(0.0f);
    Rigidbody* rb = object->getRigidbodyTrait();
    rb->setLinearVelocity(velocity);
    rb->setLinearDamping(DEFAULT_LINEAR_DAMPING);
    rb->setAngularVelocity({ 0.0f, 0.0f, 0.0f });
    rb->setAngularDamping(DEFAULT_ANGULAR_DAMPING);
    rb->setMass(1.0f);
    rb->setLockLinearX(false);
    rb->setLockLinearY(false);
    rb->setLockLinearZ(false);
    rb->setLockAngularX(false);
    rb->setLockAngularY(false);
    rb->setLockAngularZ(false);
    PrototypeEngineInternalApplication::physics->createPlaneCollider(object);
    cl->setNameRef("PLANE");
}

extern void
shortcutSetupObjectSphereColliderTrait(PrototypeObject* object, const f32 radius, const glm::vec3& velocity)
{
    Collider* cl = object->getColliderTrait();
    cl->setShapeType(ColliderShape_Sphere);
    cl->setRadius(radius);
    Rigidbody* rb = object->getRigidbodyTrait();
    rb->setLinearVelocity(velocity);
    rb->setLinearDamping(DEFAULT_LINEAR_DAMPING);
    rb->setAngularVelocity({ 0.0f, 0.0f, 0.0f });
    rb->setAngularDamping(DEFAULT_ANGULAR_DAMPING);
    rb->setMass(1.0f);
    rb->setLockLinearX(false);
    rb->setLockLinearY(false);
    rb->setLockLinearZ(false);
    rb->setLockAngularX(false);
    rb->setLockAngularY(false);
    rb->setLockAngularZ(false);
    PrototypeEngineInternalApplication::physics->createSphereCollider(object);
    cl->setNameRef("SPHERE");
}

extern void
shortcutSetupObjectCubeColliderTrait(PrototypeObject* object,
                                     const f32        width,
                                     const f32        height,
                                     const f32        depth,
                                     const glm::vec3& velocity)
{
    Collider* cl = object->getColliderTrait();
    cl->setShapeType(ColliderShape_Box);
    cl->setWidth(width);
    cl->setHeight(height);
    cl->setDepth(depth);
    Rigidbody* rb = object->getRigidbodyTrait();
    rb->setLinearVelocity(velocity);
    rb->setLinearDamping(DEFAULT_LINEAR_DAMPING);
    rb->setAngularVelocity({ 0.0f, 0.0f, 0.0f });
    rb->setAngularDamping(DEFAULT_ANGULAR_DAMPING);
    rb->setMass(1.0f);
    rb->setLockLinearX(false);
    rb->setLockLinearY(false);
    rb->setLockLinearZ(false);
    rb->setLockAngularX(false);
    rb->setLockAngularY(false);
    rb->setLockAngularZ(false);
    PrototypeEngineInternalApplication::physics->createBoxCollider(object);
    cl->setNameRef("CUBE");
}

extern void
shortcutSetupObjectConvexMeshColliderTrait(PrototypeObject* object, const glm::vec3& velocity)
{
    Collider* cl = object->getColliderTrait();
    cl->setShapeType(ColliderShape_ConvexMesh);
    cl->setWidth(1.0f);
    cl->setHeight(1.0f);
    cl->setDepth(1.0f);
    Rigidbody* rb = object->getRigidbodyTrait();
    rb->setLinearVelocity(velocity);
    rb->setLinearDamping(DEFAULT_LINEAR_DAMPING);
    rb->setAngularVelocity({ 0.0f, 0.0f, 0.0f });
    rb->setAngularDamping(DEFAULT_ANGULAR_DAMPING);
    rb->setMass(1.0f);
    rb->setLockLinearX(false);
    rb->setLockLinearY(false);
    rb->setLockLinearZ(false);
    rb->setLockAngularX(false);
    rb->setLockAngularY(false);
    rb->setLockAngularZ(false);
    const std::string      meshName = object->getMeshRendererTrait()->data()[0].mesh;
    auto                   source   = PrototypeEngineInternalApplication::database->meshBuffers[meshName]->source();
    std::vector<glm::vec3> _verts(source.vertices.size());
    for (size_t v = 0; v < source.vertices.size(); ++v) {
        _verts[v].x = source.vertices[v].positionU.x;
        _verts[v].y = source.vertices[v].positionU.y;
        _verts[v].z = source.vertices[v].positionU.z;
    }
    PrototypeEngineInternalApplication::physics->createConvexMeshCollider(_verts, source.indices, &(*object));
    cl->setNameRef(std::string("(CONVEX) ").append(meshName));
}

extern void
shortcutSetupObjectTriMeshColliderTrait(PrototypeObject* object, const glm::vec3& velocity)
{
    Collider* cl = object->getColliderTrait();
    cl->setShapeType(ColliderShape_TriangleMesh);
    cl->setWidth(1.0f);
    cl->setHeight(1.0f);
    cl->setDepth(1.0f);
    Rigidbody* rb = object->getRigidbodyTrait();
    rb->setLinearVelocity(velocity);
    rb->setLinearDamping(DEFAULT_LINEAR_DAMPING);
    rb->setAngularVelocity({ 0.0f, 0.0f, 0.0f });
    rb->setAngularDamping(DEFAULT_ANGULAR_DAMPING);
    rb->setMass(1.0f);
    rb->setLockLinearX(true);
    rb->setLockLinearY(true);
    rb->setLockLinearZ(true);
    rb->setLockAngularX(true);
    rb->setLockAngularY(true);
    rb->setLockAngularZ(true);
    const std::string      meshName = object->getMeshRendererTrait()->data()[0].mesh;
    auto                   source   = PrototypeEngineInternalApplication::database->meshBuffers[meshName]->source();
    std::vector<glm::vec3> _verts(source.vertices.size());
    for (size_t v = 0; v < source.vertices.size(); ++v) {
        _verts[v].x = source.vertices[v].positionU.x;
        _verts[v].y = source.vertices[v].positionU.y;
        _verts[v].z = source.vertices[v].positionU.z;
    }
    PrototypeEngineInternalApplication::physics->createTriMeshCollider(_verts, source.indices, &(*object));
    cl->setNameRef(std::string("(TRIMESH) ").append(meshName));
}

extern void
shortcutSetupObjectVehicleChasisWheelsTrait(PrototypeObject* chasisObject,
                                            PrototypeObject* wheelFRObject,
                                            PrototypeObject* wheelFLObject,
                                            PrototypeObject* wheelBRObject,
                                            PrototypeObject* wheelBLObject)
{
    std::vector<glm::vec3> chasisVertices;
    std::vector<u32>       chasisIndices;
    std::vector<glm::vec3> wheelVertices;
    std::vector<u32>       wheelIndices;
    PrototypeEngineInternalApplication::physics->createVehicle(chasisVertices,
                                                               chasisIndices,
                                                               wheelVertices,
                                                               wheelIndices,
                                                               chasisObject,
                                                               wheelFRObject,
                                                               wheelFLObject,
                                                               wheelBRObject,
                                                               wheelBLObject);
}

extern void
shortcutSpawnSphere(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& dir)
{
    auto              defaultLayer          = PrototypeEngineInternalApplication::scene->layers().begin()->second;
    static size_t     spawnedSpheresCounter = 0;
    std::stringstream ss;
    ss << "Sphere (Clone) " << spawnedSpheresCounter++;
    PrototypeObject* object =
      shotcutCreateCloneObjectToLayer(ss.str(),
                                      PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskRigidbody |
                                        PrototypeTraitTypeMaskCollider | PrototypeTraitTypeMaskMeshRenderer,
                                      defaultLayer);
    if (object) {
        glm::vec3 sca = { 1.0f, 1.0f, 1.0f };
        shortcutSetupObjectTransformTrait(object, position, rotation, sca);
        shortcutSetupObjectMeshRendererTrait(object, "sphere.obj", PROTOTYPE_DEFAULT_MATERIAL);
        shortcutSetupObjectSphereColliderTrait(object, 1.0f, dir);
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        PrototypeEngineInternalApplication::physics->scheduleRecordPass();
    }
}

extern void
shortcutSpawnCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& dir)
{
    auto              defaultLayer        = PrototypeEngineInternalApplication::scene->layers().begin()->second;
    static size_t     spawnedCubesCounter = 0;
    std::stringstream ss;
    ss << "Cube (Clone) " << spawnedCubesCounter++;
    PrototypeObject* object =
      shotcutCreateCloneObjectToLayer(ss.str(),
                                      PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskRigidbody |
                                        PrototypeTraitTypeMaskCollider | PrototypeTraitTypeMaskMeshRenderer,
                                      defaultLayer);
    if (object) {
        glm::vec3 sca = { 1.0f, 1.0f, 1.0f };
        shortcutSetupObjectTransformTrait(object, position, rotation, sca);
        shortcutSetupObjectMeshRendererTrait(object, "CUBE", PROTOTYPE_DEFAULT_MATERIAL);
        shortcutSetupObjectCubeColliderTrait(object, sca.x, sca.y, sca.z, dir);
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        PrototypeEngineInternalApplication::physics->scheduleRecordPass();
    }
}

extern void
shortcutSpawnConvexMesh(const glm::vec3&   position,
                        const glm::vec3&   rotation,
                        const glm::vec3&   dir,
                        const std::string& mesh,
                        const std::string& material)
{
    auto              defaultLayer         = PrototypeEngineInternalApplication::scene->layers().begin()->second;
    static size_t     spawnedConvexCounter = 0;
    std::stringstream ss;
    ss << "Convex (Clone) " << spawnedConvexCounter++;
    PrototypeObject* object =
      shotcutCreateCloneObjectToLayer(ss.str(),
                                      PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskRigidbody |
                                        PrototypeTraitTypeMaskCollider | PrototypeTraitTypeMaskMeshRenderer,
                                      defaultLayer);
    if (object) {
        glm::vec3 sca = { 1.0f, 1.0f, 1.0f };
        shortcutSetupObjectTransformTrait(object, position, rotation, sca);
        shortcutSetupObjectMeshRendererTrait(object, mesh, material);
        shortcutSetupObjectConvexMeshColliderTrait(object, dir);
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        PrototypeEngineInternalApplication::physics->scheduleRecordPass();
    }
}

extern void
shortcutSpawnTriMesh(const glm::vec3&   position,
                     const glm::vec3&   rotation,
                     const glm::vec3&   dir,
                     const std::string& mesh,
                     const std::string& material)
{
    auto              defaultLayer          = PrototypeEngineInternalApplication::scene->layers().begin()->second;
    static size_t     spawnedTriMeshCounter = 0;
    std::stringstream ss;
    ss << "TriMesh (Clone) " << spawnedTriMeshCounter++;
    PrototypeObject* object =
      shotcutCreateCloneObjectToLayer(ss.str(),
                                      PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskRigidbody |
                                        PrototypeTraitTypeMaskCollider | PrototypeTraitTypeMaskMeshRenderer,
                                      defaultLayer);
    if (object) {
        glm::vec3 sca = { 1.0f, 1.0f, 1.0f };
        shortcutSetupObjectTransformTrait(object, position, rotation, sca);
        shortcutSetupObjectMeshRendererTrait(object, mesh, material);
        shortcutSetupObjectTriMeshColliderTrait(object, dir);
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        PrototypeEngineInternalApplication::physics->scheduleRecordPass();
    }
}

extern void
shortcutSpawnVehicle(const glm::vec3&   position,
                     const glm::vec3&   rotation,
                     const glm::vec3&   dir,
                     const std::string& mesh,
                     const std::string& material)
{
    auto              defaultLayer          = PrototypeEngineInternalApplication::scene->layers().begin()->second;
    static size_t     spawnedVehicleCounter = 0;
    std::stringstream chasisSS, wheelFRSS, wheelFLSS, wheelBRSS, wheelBLSS;
    chasisSS << "Vehicle Chasis (Clone) " << spawnedVehicleCounter++;
    wheelFRSS << "Vehicle Wheel FR (Clone) " << spawnedVehicleCounter;
    wheelFLSS << "Vehicle Wheel FL (Clone) " << spawnedVehicleCounter;
    wheelBRSS << "Vehicle Wheel BR (Clone) " << spawnedVehicleCounter;
    wheelBLSS << "Vehicle Wheel BL (Clone) " << spawnedVehicleCounter;
    PrototypeObject* chasisObject = shotcutCreateCloneObjectToLayer(
      chasisSS.str(),
      PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskMeshRenderer | PrototypeTraitTypeMaskVehicleChasis,
      defaultLayer);
    PrototypeObject* wheelFRObject = shotcutCreateCloneObjectToLayer(
      wheelFRSS.str(), PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskMeshRenderer, defaultLayer);
    PrototypeObject* wheelFLObject = shotcutCreateCloneObjectToLayer(
      wheelFLSS.str(), PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskMeshRenderer, defaultLayer);
    PrototypeObject* wheelBRObject = shotcutCreateCloneObjectToLayer(
      wheelBRSS.str(), PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskMeshRenderer, defaultLayer);
    PrototypeObject* wheelBLObject = shotcutCreateCloneObjectToLayer(
      wheelBLSS.str(), PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskMeshRenderer, defaultLayer);
    if (chasisObject && wheelFRObject && wheelFLObject && wheelBRObject && wheelBLObject) {
        glm::vec3 chasisScale = { 2.5f, 2.0f, 5.0f };
        shortcutSetupObjectTransformTrait(chasisObject, position, rotation, chasisScale);
        shortcutSetupObjectMeshRendererTrait(chasisObject, "CUBE", "default");

        glm::vec3 wheelScale = { .5f, .5f, .5f };
        shortcutSetupObjectTransformTrait(wheelFRObject, position, rotation, wheelScale);
        shortcutSetupObjectMeshRendererTrait(wheelFRObject, "cylinder.obj", "default");

        shortcutSetupObjectTransformTrait(wheelFLObject, position, rotation, wheelScale);
        shortcutSetupObjectMeshRendererTrait(wheelFLObject, "cylinder.obj", "default");

        shortcutSetupObjectTransformTrait(wheelBRObject, position, rotation, wheelScale);
        shortcutSetupObjectMeshRendererTrait(wheelBRObject, "cylinder.obj", "default");

        shortcutSetupObjectTransformTrait(wheelBLObject, position, rotation, wheelScale);
        shortcutSetupObjectMeshRendererTrait(wheelBLObject, "cylinder.obj", "default");

        shortcutSetupObjectVehicleChasisWheelsTrait(chasisObject, wheelFRObject, wheelFLObject, wheelBRObject, wheelBLObject);

        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        PrototypeEngineInternalApplication::physics->scheduleRecordPass();
    }
}

extern void
shortcutEditorAddSceneNodeToLayer(PrototypeSceneLayer* parentLayer, glm::vec3 position, glm::vec3 rotation, glm::vec3 dir)
{
    if (!parentLayer) return;

    static size_t     spawnedEditorSceneNodeCounter = 0;
    std::stringstream ss;
    ss << "Node/Layer (Editor) " << spawnedEditorSceneNodeCounter++;
    PrototypeObject* object =
      shotcutCreateCloneObjectToLayer(ss.str(),
                                      PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskRigidbody |
                                        PrototypeTraitTypeMaskCollider | PrototypeTraitTypeMaskMeshRenderer,
                                      parentLayer);
    if (object) {
        // auto      cameraObjects =
        // PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera); auto      defaultCamera
        // = *cameraObjects.begin(); auto      camera        = defaultCamera->getCameraTrait(); glm::vec3 ray           = {};
        // PrototypeMaths::projectRayFromClipSpaceCenterPoint(ray, camera->viewMatrix());
        // glm::vec3 position = camera->position();
        // position += ray * 10.0f;
        // glm::vec3 rotation = { camera->rotation().x, camera->rotation().y, 0.0f };
        glm::vec3 sca = { 1.0f, 1.0f, 1.0f };
        shortcutSetupObjectTransformTrait(object, position, rotation, sca);
        shortcutSetupObjectMeshRendererTrait(object, PROTOTYPE_DEFAULT_MESH, PROTOTYPE_DEFAULT_MATERIAL);
        shortcutSetupObjectCubeColliderTrait(object, 1.0f, 1.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        PrototypeEngineInternalApplication::physics->scheduleRecordPass();
    }
}

extern void
shortcutEditorAddSceneNodeToNode(PrototypeSceneNode* parentNode, glm::vec3 position, glm::vec3 rotation, glm::vec3 dir)
{
    if (!parentNode) return;

    static size_t     spawnedEditorSceneNodeCounter = 0;
    std::stringstream ss;
    ss << "Node/Node (Editor) " << spawnedEditorSceneNodeCounter++;
    PrototypeObject* object =
      shotcutCreateCloneObjectToNode(ss.str(),
                                     PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskRigidbody |
                                       PrototypeTraitTypeMaskCollider | PrototypeTraitTypeMaskMeshRenderer,
                                     parentNode);
    if (object) {
        // auto      cameraObjects =
        // PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera); auto      defaultCamera
        // = *cameraObjects.begin(); auto      camera        = defaultCamera->getCameraTrait(); glm::vec3 ray           = {};
        // PrototypeMaths::projectRayFromClipSpaceCenterPoint(ray, camera->viewMatrix());
        // glm::vec3 position = camera->position();
        // position += ray * 10.0f;
        // glm::vec3 rotation = { camera->rotation().x, camera->rotation().y, 0.0f };
        glm::vec3 sca = { 1.0f, 1.0f, 1.0f };
        shortcutSetupObjectTransformTrait(object, position, rotation, sca);
        shortcutSetupObjectMeshRendererTrait(object, PROTOTYPE_DEFAULT_MESH, PROTOTYPE_DEFAULT_MATERIAL);
        shortcutSetupObjectCubeColliderTrait(object, 1.0f, 1.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        PrototypeEngineInternalApplication::physics->scheduleRecordPass();
    }
}

// extern void
// shortcutEditorRemoveSceneLayer(PrototypeSceneLayer* layer)
// {
//     if (PrototypeEngineInternalApplication::scene->removeLayerById(layer->id())) {
//         PrototypeEngineInternalApplication::renderer->recordPass();
//     }
// }

extern void
shortcutEditorRemoveSceneNode(PrototypeSceneNode* node)
{
    PrototypeEngineInternalApplication::scene->clearSelectedNodes();
    if (node->parentLayer()) {
        node->parentLayer()->removeNodeById(node->id());
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        PrototypeEngineInternalApplication::physics->scheduleRecordPass();
    } else if (node->parentNode()) {
        node->parentNode()->removeNodeById(node->id());
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        PrototypeEngineInternalApplication::physics->scheduleRecordPass();
    }
}

extern void
shortcutEditorRemoveSelectedSceneNode()
{
    auto selectedNodes = PrototypeEngineInternalApplication::scene->selectedNodes();
    PrototypeEngineInternalApplication::scene->clearSelectedNodes();
    for (auto& selectedNode : selectedNodes) {
        if (selectedNode->parentLayer()) {
            selectedNode->parentLayer()->removeNodeById(selectedNode->id());
        } else if (selectedNode->parentNode()) {
            selectedNode->parentNode()->removeNodeById(selectedNode->id());
        }
    }
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutEditorSelectedSceneNodeAddTraits(PrototypeObject* object, MASK_TYPE traitMask)
{
    if (object) { object->add(traitMask); }
    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(), traitMask);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutEditorSelectedSceneNodeRemoveTraits(PrototypeObject* object, MASK_TYPE traitMask)
{
    if (object) {
        if (object->has(traitMask)) {
            PrototypeEngineInternalApplication::scene->removeNodeFromTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                                  traitMask);
            object->remove(traitMask);
        }
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        PrototypeEngineInternalApplication::physics->scheduleRecordPass();
    }
}

extern void
shortcutEditorRemoveScriptFromObject(PrototypeObject* object, std::string codeLink)
{
    if (object) {
        Script*        script = object->getScriptTrait();
        ScriptCodeLink scl    = {};
        scl.filepath          = codeLink;
        auto it               = script->codeLinks.find(scl.filepath);
        if (it != script->codeLinks.end()) {
            PrototypePluginInstance::safeCallEndProtocol(&it->second, object);
            script->codeLinks.erase(it);
        }
    }
}

extern void
shortcutEditorCommitReloadPlugin(PrototypePluginInstance* pluginInstance)
{
    pluginInstance->commitChange();
}

//
extern void
shortcutDefaultCameraTraitAddInitializer(PrototypeObject* object, Camera* camera)
{
    if (PrototypeEngineInternalApplication::physics == nullptr || object == nullptr) return;

    // #ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    //     const glm::vec2& viewport = PrototypeEngineInternalApplication::renderer->ui()->sceneViewSize();
    // #else
    //     const glm::vec2& viewport = PrototypeEngineInternalApplication::window->resolution();
    // #endif
    //     CameraSystemSetResolution(camera, viewport.x, viewport.y);
    CameraSystemSetNear(camera, 0.01f);
    CameraSystemSetFar(camera, 1000.0f);
    CameraSystemSetFov(camera, 60.0f);
    camera->lookSensitivityMut() = 0.01f;
    camera->moveSensitivityMut() = 0.03f;
    CameraSystemSetTranslation(camera, glm::vec3(0.0f, -10.0f, 10.0f));
    CameraSystemSetRotation(camera, glm::vec2(0.0f, 0.0f));
    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskCamera);

    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultCameraTraitReuseInitializer(PrototypeObject* object, Camera* camera)
{
    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskCamera);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultCameraTraitRemoveInitializer(PrototypeObject* object, Camera* camera)
{
    if (PrototypeEngineInternalApplication::physics == nullptr || object == nullptr) return;
    PrototypeEngineInternalApplication::scene->removeNodeFromTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                          PrototypeTraitTypeMaskCamera);

    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultCameraTraitLogInitializer(PrototypeObject* object, Camera* camera)
{}

//
extern void
shortcutDefaultColliderTraitAddInitializer(PrototypeObject* object, Collider* collider)
{
    if (PrototypeEngineInternalApplication::physics == nullptr || object == nullptr) return;
    collider->setShapeType(ColliderShape_Box);
    collider->setWidth(1.0f);
    collider->setHeight(1.0f);
    collider->setDepth(1.0f);
    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskCollider);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultColliderTraitReuseInitializer(PrototypeObject* object, Collider* collider)
{
    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskCollider);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultColliderTraitRemoveInitializer(PrototypeObject* object, Collider* collider)
{
    if (PrototypeEngineInternalApplication::physics == nullptr || object == nullptr) return;
    PrototypeEngineInternalApplication::scene->removeNodeFromTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                          PrototypeTraitTypeMaskCollider);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultColliderTraitLogInitializer(PrototypeObject* object, Collider* collider)
{}

//
extern void
shortcutDefaultMeshRendererTraitAddInitializer(PrototypeObject* object, MeshRenderer* meshRenderer)
{
    if (object == nullptr || !PrototypeEngineInternalApplication::scene) return;
    shortcutSetupObjectMeshRendererTrait(object, "CUBE", PROTOTYPE_DEFAULT_MATERIAL);
    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskMeshRenderer);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultMeshRendererTraitReuseInitializer(PrototypeObject* object, MeshRenderer* meshRenderer)
{
    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskMeshRenderer);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultMeshRendererTraitRemoveInitializer(PrototypeObject* object, MeshRenderer* meshRenderer)
{
    if (object == nullptr || !PrototypeEngineInternalApplication::scene) return;
    PrototypeEngineInternalApplication::scene->removeNodeFromTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                          PrototypeTraitTypeMaskMeshRenderer);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultMeshRendererTraitLogInitializer(PrototypeObject* object, MeshRenderer* meshRenderer)
{}

//
extern void
shortcutDefaultRigidbodyTraitAddInitializer(PrototypeObject* object, Rigidbody* rigidbody)
{
    if (object == nullptr || !PrototypeEngineInternalApplication::scene) return;
    if (!object->hasTransformTrait()) { object->addTransformTrait(); }
    if (!object->hasColliderTrait()) { object->addColliderTrait(); }

    object->getTransformTrait()->setNeedsPhysicsSync(true);
    rigidbody->setLinearVelocity({ 0.0f, 0.0f, 0.0f });
    rigidbody->setLinearDamping(DEFAULT_LINEAR_DAMPING);
    rigidbody->setAngularVelocity({ 0.0f, 0.0f, 0.0f });
    rigidbody->setAngularDamping(DEFAULT_ANGULAR_DAMPING);
    rigidbody->setMass(1.0f);
    rigidbody->setLockLinearX(false);
    rigidbody->setLockLinearY(false);
    rigidbody->setLockLinearZ(false);
    rigidbody->setLockAngularX(false);
    rigidbody->setLockAngularY(false);
    rigidbody->setLockAngularZ(false);

    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskRigidbody);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultRigidbodyTraitReuseInitializer(PrototypeObject* object, Rigidbody* rigidbody)
{
    if (!object->hasTransformTrait()) { object->addTransformTrait(); }
    if (!object->hasColliderTrait()) { object->addColliderTrait(); }

    PrototypeEngineInternalApplication::physics->createRigidbody(object);
    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskRigidbody);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultRigidbodyTraitRemoveInitializer(PrototypeObject* object, Rigidbody* rigidbody)
{
    if (object == nullptr || !PrototypeEngineInternalApplication::scene) return;
    PrototypeEngineInternalApplication::scene->removeNodeFromTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                          PrototypeTraitTypeMaskRigidbody);
    PrototypeEngineInternalApplication::physics->destroyRigidbody(rigidbody->rigidbodyRef());
    auto* node = static_cast<PrototypeSceneNode*>(object->parentNode());
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultRigidbodyTraitLogInitializer(PrototypeObject* object, Rigidbody* rigidbody)
{}

//
extern void
shortcutDefaultScriptTraitAddInitializer(PrototypeObject* object, Script* script)
{}

extern void
shortcutDefaultScriptTraitReuseInitializer(PrototypeObject* object, Script* script)
{}

extern void
shortcutDefaultScriptTraitRemoveInitializer(PrototypeObject* object, Script* script)
{}

extern void
shortcutDefaultScriptTraitLogInitializer(PrototypeObject* object, Script* script)
{}

//
extern void
shortcutDefaultTransformTraitAddInitializer(PrototypeObject* object, Transform* transform)
{
    if (object == nullptr || !PrototypeEngineInternalApplication::scene) return;

    auto        cameraObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera);
    auto        cameraObject  = *cameraObjects.begin();
    Camera*     cam           = cameraObject->getCameraTrait();
    const auto& camPosition   = cam->position();
    const auto& camRotation   = cam->rotation();
    const auto& camViewMatrix = cam->viewMatrix();
    glm::vec3   ray;
    // glm::vec2   cursor       = PrototypeEngineInternalApplication::renderer->sceneViewCursorCoordinates();
    // glm::vec2   viewportSize = PrototypeEngineInternalApplication::renderer->sceneViewSize();
    PrototypeMaths::projectRayFromClipSpaceCenterPoint(ray, camViewMatrix);
    glm::vec3 position = { -camPosition.x + (ray.x * 10.0f), -camPosition.y + (ray.y * 10.0f), -camPosition.z + (ray.z * 10.0f) };
    glm::vec3 rotation = { camRotation.x, camRotation.y, 0.0f };
    glm::vec3 scale    = { 1.0f, 1.0f, 1.0f };
    glm::mat4 model;
    PrototypeMaths::buildModelMatrix(model, position, rotation);
    PrototypeMaths::buildModelMatrixWithScale(model, scale);
    transform->setModelScaled(&model[0][0]);
    transform->updateComponentsFromMatrix();

    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskTransform);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultTransformTraitReuseInitializer(PrototypeObject* object, Transform* transform)
{
    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskTransform);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultTransformTraitRemoveInitializer(PrototypeObject* object, Transform* transform)
{
    PrototypeEngineInternalApplication::scene->removeNodeFromTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                          PrototypeTraitTypeMaskTransform);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultTransformTraitLogInitializer(PrototypeObject* object, Transform* transform)
{}

//
extern void
shortcutDefaultVehicleChasisTraitAddInitializer(PrototypeObject* object, VehicleChasis* vch)
{
    if (object == nullptr || !PrototypeEngineInternalApplication::scene) return;

    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskVehicleChasis);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultVehicleChasisTraitReuseInitializer(PrototypeObject* object, VehicleChasis* vch)
{
    PrototypeEngineInternalApplication::scene->addNodeToTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                     PrototypeTraitTypeMaskVehicleChasis);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultVehicleChasisTraitRemoveInitializer(PrototypeObject* object, VehicleChasis* vch)
{
    PrototypeEngineInternalApplication::scene->removeNodeFromTraitFilters((PrototypeSceneNode*)object->parentNode(),
                                                                          PrototypeTraitTypeMaskVehicleChasis);
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

extern void
shortcutDefaultVehicleChasisTraitLogInitializer(PrototypeObject* object, VehicleChasis* vch)
{}