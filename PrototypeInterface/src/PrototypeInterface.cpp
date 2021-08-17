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

#include "../include/PrototypeInterface/PrototypeInterface.h"

#include <PrototypeCommon/Definitions.h>

#include <PrototypeEngine/PrototypeEngineApplication.h>

#include <PrototypeEngine/../../src/core/PrototypeCameraSystem.h>
#include <PrototypeEngine/../../src/core/PrototypeEngine.h>
#include <PrototypeEngine/../../src/core/PrototypePhysics.h>
#include <PrototypeEngine/../../src/core/PrototypeRenderer.h>
#include <PrototypeEngine/../../src/core/PrototypeScene.h>
#include <PrototypeEngine/../../src/core/PrototypeSceneLayer.h>
#include <PrototypeEngine/../../src/core/PrototypeSceneNode.h>
#include <PrototypeEngine/../../src/core/PrototypeShortcuts.h>
#include <PrototypeEngine/../../src/core/PrototypeUI.h>
#include <PrototypeEngine/../../src/core/PrototypeUiView.h>
#include <PrototypeEngine/../../src/core/PrototypeWindow.h>

#include <PrototypeCommon/Logger.h>

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include <GLFW/glfw3.h>

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
LoadContext(PrototypeEngineContext* engineContext, PrototypeLoggerData* loggerData)
{
    PrototypeLogger::setData(loggerData);
    PrototypeEngineInternalApplication::application  = engineContext->application;
    PrototypeEngineInternalApplication::renderingApi = engineContext->renderingApi;
    PrototypeEngineInternalApplication::physicsApi   = engineContext->physicsApi;
    PrototypeEngineInternalApplication::shouldQuit   = engineContext->shouldQuit;
    PrototypeEngineInternalApplication::database     = engineContext->database;
    PrototypeEngineInternalApplication::window       = engineContext->window;
    PrototypeEngineInternalApplication::renderer     = engineContext->renderer;
    PrototypeEngineInternalApplication::physics      = engineContext->physics;
    PrototypeEngineInternalApplication::scene        = engineContext->scene;
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PrototypeEngineInternalApplication::profiler = engineContext->profiler;
#endif
    PrototypeEngineInternalApplication::traitSystemData = engineContext->traitSystemData;
    PrototypeTraitSystemSetData(engineContext->traitSystemData);
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ReloadContext(PrototypeEngineContext* engineContext, PrototypeLoggerData* loggerData)
{
    PrototypeLogger::setData(loggerData);
    PrototypeEngineInternalApplication::application  = engineContext->application;
    PrototypeEngineInternalApplication::renderingApi = engineContext->renderingApi;
    PrototypeEngineInternalApplication::physicsApi   = engineContext->physicsApi;
    PrototypeEngineInternalApplication::shouldQuit   = engineContext->shouldQuit;
    PrototypeEngineInternalApplication::database     = engineContext->database;
    PrototypeEngineInternalApplication::window       = engineContext->window;
    PrototypeEngineInternalApplication::renderer     = engineContext->renderer;
    PrototypeEngineInternalApplication::physics      = engineContext->physics;
    PrototypeEngineInternalApplication::scene        = engineContext->scene;
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PrototypeEngineInternalApplication::profiler = engineContext->profiler;
#endif
    PrototypeEngineInternalApplication::traitSystemData = engineContext->traitSystemData;
    PrototypeTraitSystemSetData(engineContext->traitSystemData);
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectDestroy(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->destroy(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectGetId(void* object, int64_t* id)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) {
        *id = o->id();
        return;
    }
    *id = -1;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectGetName(void* object, const char** name)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) {
        PrototypeSceneLayer* layer = (PrototypeSceneLayer*)o->parentNode();
        if (layer) {
            *name = layer->name().c_str();
            return;
        } else {
            PrototypeSceneNode* node = (PrototypeSceneNode*)o->parentNode();
            if (node) {
                *name = node->name().c_str();
                return;
            }
        }
    }
    *name = nullptr;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddCameraTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->addCameraTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasCameraTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { return o->hasCameraTrait(); }
    return false;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveCameraTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->removeCameraTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
MainCamera(void** camera)
{
    auto cameraObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera);
    auto cameraObject  = *cameraObjects.begin();
    *camera            = cameraObject->getCameraTrait();
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetLookSensitivityFactor(void* camera, float& factor)
{
    Camera* cam = (Camera*)camera;
    factor      = cam->_lookSensitivity;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetLookSensitivityFactor(void* camera, float factor)
{
    Camera* cam           = (Camera*)camera;
    cam->_lookSensitivity = factor;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetMoveSensitivityFactor(void* camera, float& factor)
{
    Camera* cam = (Camera*)camera;
    factor      = cam->_moveSensitivity;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetMoveSensitivityFactor(void* camera, float factor)
{
    Camera* cam           = (Camera*)camera;
    cam->_moveSensitivity = factor;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetRotationPitchYaw(void* camera, FieldVec2& pitchYaw)
{
    Camera*     cam = (Camera*)camera;
    const auto& v2  = cam->_rotation;
    pitchYaw.x      = v2.x;
    pitchYaw.y      = v2.y;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetRotationPitchYaw(void* camera, FieldVec2 pitchYaw)
{
    Camera*   cam = (Camera*)camera;
    glm::vec2 rotation;
    rotation.x = pitchYaw.x;
    rotation.y = pitchYaw.y;
    CameraSystemSetRotation(cam, rotation);
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraRotate(void* camera, FieldVec2 pitchYaw)
{
    Camera* cam = (Camera*)camera;
    CameraSystemRotate(cam, pitchYaw.x, pitchYaw.y);
}

// PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
// CameraGetRotationQuaternion(void* camera, FieldVec4& q)
// {
//     Camera*     cam  = (Camera*)camera;
//     const auto& quat = cam->rotationQuat();
//     q.x              = quat.x;
//     q.y              = quat.y;
//     q.z              = quat.z;
//     q.w              = quat.w;
// }

// PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
// CameraSetRotationQuaternion(void* camera, FieldVec4 q)
// {
//     Camera* cam = (Camera*)camera;
//     CameraSystemSetRotat
// }

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetTranslation(void* camera, FieldVec3& translation)
{
    Camera* cam   = (Camera*)camera;
    translation.x = cam->_position.x;
    translation.y = cam->_position.y;
    translation.z = cam->_position.z;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetTranslation(void* camera, FieldVec3 translation)
{
    Camera*   cam = (Camera*)camera;
    glm::vec3 p;
    p.x = translation.x;
    p.y = translation.y;
    p.z = translation.z;
    CameraSystemSetTranslation(cam, p);
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraTranslate(void* camera, FieldVec3 translation)
{
    Camera* cam = (Camera*)camera;
    CameraSystemTranslate(cam, translation.x, translation.y, translation.z);
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetViewMatrix(void* camera, FieldMat4& viewMat)
{
    Camera* cam = (Camera*)camera;
    memcpy(&viewMat._00, &cam->_viewMatrix[0][0], sizeof(cam->_viewMatrix[0][0]) * 16);
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetViewMatrix(void* camera, FieldMat4 viewMat)
{
    Camera* cam = (Camera*)camera;
    memcpy(&cam->_viewMatrix[0][0], &viewMat._00, sizeof(cam->_viewMatrix[0][0]) * 16);
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraIsOrbital(void* camera, bool& orbital)
{
    Camera* cam = (Camera*)camera;
    orbital     = cam->_orbital;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetOrbital(void* camera, bool orbital)
{
    Camera* cam   = (Camera*)camera;
    cam->_orbital = orbital;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetFieldOfView(void* camera, float& fov)
{
    Camera* cam = (Camera*)camera;
    fov         = cam->_fov;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetFieldOfView(void* camera, float fov)
{
    Camera* cam = (Camera*)camera;
    cam->_fov   = fov;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetAspectRatio(void* camera, float& aspectRatio)
{
    Camera* cam = (Camera*)camera;
    aspectRatio = cam->_aspectRatio;
}

// PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
// CameraSetAspectRatio(void* camera, float aspectRatio)
// {
//     Camera* cam       = (Camera*)camera;
//     cam->_aspectRatio = aspectRatio;
// }

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetZNear(void* camera, float& znear)
{
    Camera* cam = (Camera*)camera;
    znear       = cam->_near;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetZNear(void* camera, float znear)
{
    Camera* cam = (Camera*)camera;
    cam->_near  = znear;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetZFar(void* camera, float& zfar)
{
    Camera* cam = (Camera*)camera;
    zfar        = cam->_far;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetZFar(void* camera, float zfar)
{
    Camera* cam = (Camera*)camera;
    cam->_far   = zfar;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetResolution(void* camera, FieldVec2& resolution)
{
    Camera* cam  = (Camera*)camera;
    resolution.x = cam->_resolution.x;
    resolution.y = cam->_resolution.y;
}

// PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
// CameraSetResolution(void* camera, FieldVec2 resolution)
// {
//     Camera* cam        = (Camera*)camera;
//     cam->_resolution.x = resolution.x;
//     cam->_resolution.y = resolution.y;
// }

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetProjectionMatrix(void* camera, FieldMat4& projectionMat)
{
    Camera* cam = (Camera*)camera;
    memcpy(&projectionMat._00, &cam->_projectionMatrix[0][0], sizeof(cam->_projectionMatrix[0][0]) * 16);
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetProjectionMatrix(void* camera, FieldMat4 projectionMat)
{
    Camera* cam = (Camera*)camera;
    memcpy(&cam->_projectionMatrix[0][0], &projectionMat._00, sizeof(cam->_projectionMatrix[0][0]) * 16);
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddColliderTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->addColliderTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasColliderTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { return o->hasColliderTrait(); }
    return false;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveColliderTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->removeColliderTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddMeshRendererTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->addMeshRendererTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasMeshRendererTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { return o->hasMeshRendererTrait(); }
    return false;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveMeshRendererTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->removeMeshRendererTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddRigidbodyTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->addRigidbodyTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasRigidbodyTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { return o->hasRigidbodyTrait(); }
    return false;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveRigidbodyTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->removeRigidbodyTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddScriptTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->addScriptTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasScriptTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->hasScriptTrait(); }
    return false;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveScriptTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->removeScriptTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddTransformTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->addTransformTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasTransformTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { return o->hasTransformTrait(); }
    return false;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveTransformTrait(void* object)
{
    PrototypeObject* o = (PrototypeObject*)object;
    if (o) { o->removeTransformTrait(); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitGetForward(void* object, FieldVec3& forward)
{
    PrototypeObject* o          = (PrototypeObject*)object;
    Transform*       transform  = o->getTransformTrait();
    glm::mat4        viewMatrix = glm::inverse(transform->model());
    glm::vec3        fwd        = glm::normalize(glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]));
    forward.x                   = fwd.x;
    forward.y                   = fwd.y;
    forward.z                   = fwd.z;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitGetTranslation(void* object, FieldVec3& translation)
{
    PrototypeObject* o         = (PrototypeObject*)object;
    Transform*       transform = o->getTransformTrait();
    translation.x              = transform->position().x;
    translation.y              = transform->position().y;
    translation.z              = transform->position().z;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitGetRotation(void* object, FieldVec3& eulerAngles)
{
    PrototypeObject* o         = (PrototypeObject*)object;
    Transform*       transform = o->getTransformTrait();
    eulerAngles.x              = transform->rotation().x;
    eulerAngles.y              = transform->rotation().y;
    eulerAngles.z              = transform->rotation().z;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitSetTranslation(void* object, const FieldVec3& translation)
{
    PrototypeObject* o         = (PrototypeObject*)object;
    Transform*       transform = o->getTransformTrait();
    transform->positionMut().x = translation.x;
    transform->positionMut().y = translation.y;
    transform->positionMut().z = translation.z;
    glm::mat4 model;
    PrototypeMaths::buildModelMatrix(model, transform->position(), transform->rotation());
    PrototypeMaths::buildModelMatrixWithScale(model, transform->scale());
    transform->setModelScaled(&model[0][0]);
    transform->updateComponentsFromMatrix();
    if (o->hasColliderTrait()) { transform->setNeedsPhysicsSync(true); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitSetRotation(void* object, const FieldVec3& eulerAngles)
{
    PrototypeObject* o         = (PrototypeObject*)object;
    Transform*       transform = o->getTransformTrait();
    transform->rotationMut().x = eulerAngles.x;
    transform->rotationMut().y = eulerAngles.y;
    transform->rotationMut().z = eulerAngles.z;
    glm::mat4 model;
    PrototypeMaths::buildModelMatrix(model, transform->position(), transform->rotation());
    PrototypeMaths::buildModelMatrixWithScale(model, transform->scale());
    transform->setModelScaled(&model[0][0]);
    transform->updateComponentsFromMatrix();
    if (o->hasColliderTrait()) { transform->setNeedsPhysicsSync(true); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitSetScale(void* object, const FieldVec3& scale)
{
    PrototypeObject* o         = (PrototypeObject*)object;
    Transform*       transform = o->getTransformTrait();
    transform->scaleMut().x += scale.x;
    transform->scaleMut().y += scale.y;
    transform->scaleMut().z += scale.z;
    glm::mat4 model;
    PrototypeMaths::buildModelMatrix(model, transform->position(), transform->rotation());
    transform->setModel(&model[0][0]);
    if (o->hasColliderTrait()) { PrototypeEngineInternalApplication::physics->scaleCollider(o, transform->scale()); }
    PrototypeMaths::buildModelMatrixWithScale(model, transform->scale());
    transform->setModelScaled(&model[0][0]);
    transform->updateComponentsFromMatrix();
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitTranslate(void* object, const FieldVec3& direction, float velocity)
{
    PrototypeObject* o         = (PrototypeObject*)object;
    Transform*       transform = o->getTransformTrait();
    transform->positionMut().x += direction.x * velocity;
    transform->positionMut().y += direction.y * velocity;
    transform->positionMut().z += direction.z * velocity;
    glm::mat4 model;
    PrototypeMaths::buildModelMatrix(model, transform->position(), transform->rotation());
    PrototypeMaths::buildModelMatrixWithScale(model, transform->scale());
    transform->setModelScaled(&model[0][0]);
    transform->updateComponentsFromMatrix();
    if (o->hasColliderTrait()) { transform->setNeedsPhysicsSync(true); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitRotate(void* object, const FieldVec3& angle, float velocity)
{
    PrototypeObject* o         = (PrototypeObject*)object;
    Transform*       transform = o->getTransformTrait();
    transform->rotationMut().x += angle.x * velocity;
    transform->rotationMut().y += angle.y * velocity;
    transform->rotationMut().z += angle.z * velocity;
    glm::mat4 model;
    PrototypeMaths::buildModelMatrix(model, transform->position(), transform->rotation());
    PrototypeMaths::buildModelMatrixWithScale(model, transform->scale());
    transform->setModelScaled(&model[0][0]);
    transform->updateComponentsFromMatrix();
    if (o->hasColliderTrait()) { transform->setNeedsPhysicsSync(true); }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
consoleLog(void* object, const char* file, int line, const char* text)
{
    PrototypeLogger::log(file, line, text);
}

PROTOTYPE_EXTERN PROTOTYPE_INTERFACE_API void
spawnCube()
{
    auto cameraObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera);
    auto cameraObject  = *cameraObjects.begin();
    auto selectedNodes = PrototypeEngineInternalApplication::scene->selectedNodes();
    PrototypeEngineInternalApplication::scene->clearSelectedNodes();
    Camera*          cam                 = cameraObject->getCameraTrait();
    const auto&      camPosition         = cam->position();
    const auto&      camViewMatrix       = cam->viewMatrix();
    const auto&      camProjectionMatrix = cam->projectionMatrix();
    const auto&      camRotation         = cam->rotation();
    const auto&      camResolution       = cam->resolution();
    const glm::vec2& sceneViewSize       = PrototypeEngineInternalApplication::renderer->ui()->sceneView()->dimensions();
    const glm::vec2& sceneViewCursorCoordinates =
      PrototypeEngineInternalApplication::renderer->ui()->sceneView()->cursorCoordinates();
    glm::vec3 ray;
    PrototypeMaths::projectRayFromClipSpacePoint(ray,
                                                 camViewMatrix,
                                                 camProjectionMatrix,
                                                 sceneViewCursorCoordinates.x,
                                                 sceneViewCursorCoordinates.y,
                                                 sceneViewSize.x,
                                                 sceneViewSize.y);
    glm::vec3   pos   = { camPosition.x, camPosition.y, camPosition.z };
    glm::vec3   rot   = { camRotation.x, camRotation.y, 0.0f };
    const float speed = 10.0f;
    ray.x *= speed;
    ray.y *= speed;
    ray.z *= speed;

    //    shortcutSpawnSphere(pos, rot, ray);
    shortcutSpawnCube(pos, rot, ray);
    //    shortcutSpawnConvexMesh(pos, rot, ray, "ico.obj", PROTOTYPE_DEFAULT_MATERIAL);
    //    shortcutSpawnConvexMesh(pos, rot, ray, "monkey.obj", PROTOTYPE_DEFAULT_MATERIAL);
    //    shortcutSpawnConvexMesh(pos, rot, ray, "torus.obj", PROTOTYPE_DEFAULT_MATERIAL);
    //    shortcutSpawnTriMesh(pos, rot, ray, "torus.obj", PROTOTYPE_DEFAULT_MATERIAL);
    for (auto& node : selectedNodes) { PrototypeEngineInternalApplication::scene->addSelectedNode(node); }
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
}

PROTOTYPE_EXTERN PROTOTYPE_INTERFACE_API void
spawnSphere()
{
    auto cameraObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera);
    auto cameraObject  = *cameraObjects.begin();
    auto selectedNodes = PrototypeEngineInternalApplication::scene->selectedNodes();
    PrototypeEngineInternalApplication::scene->clearSelectedNodes();
    Camera*          cam                 = cameraObject->getCameraTrait();
    const auto&      camPosition         = cam->position();
    const auto&      camViewMatrix       = cam->viewMatrix();
    const auto&      camProjectionMatrix = cam->projectionMatrix();
    const auto&      camRotation         = cam->rotation();
    const auto&      camResolution       = cam->resolution();
    const glm::vec2& sceneViewSize       = PrototypeEngineInternalApplication::renderer->ui()->sceneView()->dimensions();
    const glm::vec2& sceneViewCursorCoordinates =
      PrototypeEngineInternalApplication::renderer->ui()->sceneView()->cursorCoordinates();
    glm::vec3 ray;
    PrototypeMaths::projectRayFromClipSpacePoint(ray,
                                                 camViewMatrix,
                                                 camProjectionMatrix,
                                                 sceneViewCursorCoordinates.x,
                                                 sceneViewCursorCoordinates.y,
                                                 sceneViewSize.x,
                                                 sceneViewSize.y);
    glm::vec3   pos   = { camPosition.x, camPosition.y, camPosition.z };
    glm::vec3   rot   = { camRotation.x, camRotation.y, 0.0f };
    const float speed = 10.0f;
    ray.x *= speed;
    ray.y *= speed;
    ray.z *= speed;

    shortcutSpawnSphere(pos, rot, ray);
    //      shortcutSpawnCube(pos, rot, ray);
    //    shortcutSpawnConvexMesh(pos, rot, ray, "ico.obj", PROTOTYPE_DEFAULT_MATERIAL);
    //    shortcutSpawnConvexMesh(pos, rot, ray, "monkey.obj", PROTOTYPE_DEFAULT_MATERIAL);
    //    shortcutSpawnConvexMesh(pos, rot, ray, "torus.obj", PROTOTYPE_DEFAULT_MATERIAL);
    //    shortcutSpawnTriMesh(pos, rot, ray, "torus.obj", PROTOTYPE_DEFAULT_MATERIAL);
    for (auto& node : selectedNodes) { PrototypeEngineInternalApplication::scene->addSelectedNode(node); }
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
}

PROTOTYPE_EXTERN PROTOTYPE_INTERFACE_API void
spawnConvexMesh(const char* name)
{
    auto cameraObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera);
    auto cameraObject  = *cameraObjects.begin();
    auto selectedNodes = PrototypeEngineInternalApplication::scene->selectedNodes();
    PrototypeEngineInternalApplication::scene->clearSelectedNodes();
    Camera*          cam                 = cameraObject->getCameraTrait();
    const auto&      camPosition         = cam->position();
    const auto&      camViewMatrix       = cam->viewMatrix();
    const auto&      camProjectionMatrix = cam->projectionMatrix();
    const auto&      camRotation         = cam->rotation();
    const auto&      camResolution       = cam->resolution();
    const glm::vec2& sceneViewSize       = PrototypeEngineInternalApplication::renderer->ui()->sceneView()->dimensions();
    const glm::vec2& sceneViewCursorCoordinates =
      PrototypeEngineInternalApplication::renderer->ui()->sceneView()->cursorCoordinates();
    glm::vec3 ray;
    PrototypeMaths::projectRayFromClipSpacePoint(ray,
                                                 camViewMatrix,
                                                 camProjectionMatrix,
                                                 sceneViewCursorCoordinates.x,
                                                 sceneViewCursorCoordinates.y,
                                                 sceneViewSize.x,
                                                 sceneViewSize.y);
    glm::vec3   pos   = { camPosition.x, camPosition.y, camPosition.z };
    glm::vec3   rot   = { camRotation.x, camRotation.y, 0.0f };
    const float speed = 10.0f;
    ray.x *= speed;
    ray.y *= speed;
    ray.z *= speed;

    //    shortcutSpawnSphere(pos, rot, ray);
    //  shortcutSpawnCube(pos, rot, ray);
    shortcutSpawnConvexMesh(pos, rot, ray, name, PROTOTYPE_DEFAULT_MATERIAL);
    //    shortcutSpawnConvexMesh(pos, rot, ray, "monkey.obj", PROTOTYPE_DEFAULT_MATERIAL);
    //    shortcutSpawnConvexMesh(pos, rot, ray, "torus.obj", PROTOTYPE_DEFAULT_MATERIAL);
    //    shortcutSpawnTriMesh(pos, rot, ray, "torus.obj", PROTOTYPE_DEFAULT_MATERIAL);
    for (auto& node : selectedNodes) { PrototypeEngineInternalApplication::scene->addSelectedNode(node); }
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
}

PROTOTYPE_EXTERN PROTOTYPE_INTERFACE_API void
spawnTriMesh(const char* name)
{
    auto cameraObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera);
    auto cameraObject  = *cameraObjects.begin();
    auto selectedNodes = PrototypeEngineInternalApplication::scene->selectedNodes();
    PrototypeEngineInternalApplication::scene->clearSelectedNodes();
    Camera*          cam                 = cameraObject->getCameraTrait();
    const auto&      camPosition         = cam->position();
    const auto&      camViewMatrix       = cam->viewMatrix();
    const auto&      camProjectionMatrix = cam->projectionMatrix();
    const auto&      camRotation         = cam->rotation();
    const auto&      camResolution       = cam->resolution();
    const glm::vec2& sceneViewSize       = PrototypeEngineInternalApplication::renderer->ui()->sceneView()->dimensions();
    const glm::vec2& sceneViewCursorCoordinates =
      PrototypeEngineInternalApplication::renderer->ui()->sceneView()->cursorCoordinates();
    glm::vec3 ray;
    PrototypeMaths::projectRayFromClipSpacePoint(ray,
                                                 camViewMatrix,
                                                 camProjectionMatrix,
                                                 sceneViewCursorCoordinates.x,
                                                 sceneViewCursorCoordinates.y,
                                                 sceneViewSize.x,
                                                 sceneViewSize.y);
    glm::vec3   pos   = { camPosition.x, camPosition.y, camPosition.z };
    glm::vec3   rot   = { camRotation.x, camRotation.y, 0.0f };
    const float speed = 10.0f;
    ray.x *= speed;
    ray.y *= speed;
    ray.z *= speed;

    //    shortcutSpawnSphere(pos, rot, ray);
    //  shortcutSpawnCube(pos, rot, ray);
    //    shortcutSpawnConvexMesh(pos, rot, ray, "ico.obj", PROTOTYPE_DEFAULT_MATERIAL);
    //    shortcutSpawnConvexMesh(pos, rot, ray, "monkey.obj", PROTOTYPE_DEFAULT_MATERIAL);
    //    shortcutSpawnConvexMesh(pos, rot, ray, "torus.obj", PROTOTYPE_DEFAULT_MATERIAL);
    shortcutSpawnTriMesh(pos, rot, ray, name, PROTOTYPE_DEFAULT_MATERIAL);
    for (auto& node : selectedNodes) { PrototypeEngineInternalApplication::scene->addSelectedNode(node); }
    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
}

//
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API double
Time()
{
    return glfwGetTime();
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API double
DeltaTime()
{
    return PrototypeEngineInternalApplication::window->deltaTime();
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
RendererGetSceneViewSize(FieldVec2& viewSize)
{
    const glm::vec2& sceneViewSize = PrototypeEngineInternalApplication::renderer->ui()->sceneView()->dimensions();
    viewSize.x                     = sceneViewSize.x;
    viewSize.y                     = sceneViewSize.y;
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
PhysicsRaycastFromLocationAndDirection(void** hitObject, const FieldVec3& origin, const FieldVec3& direction, float rayLength)
{
    glm::vec3* glmorigin    = (glm::vec3*)&origin;
    glm::vec3* glmdirection = (glm::vec3*)&direction;
    *glmdirection           = glm::normalize(*glmdirection);
    auto hit                = PrototypeEngineInternalApplication::physics->raycast(*glmorigin, *glmdirection, rayLength);
    if (hit.has_value()) {
        *hitObject = hit.value();
    } else {
        *hitObject = nullptr;
    }
}

PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
PhysicsRaycastFromMainCameraViewport(void** hitObject, double x, double y, float rayLength)
{
    auto        cameraObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera);
    auto        cameraObject  = *cameraObjects.begin();
    Camera*     cam           = cameraObject->getCameraTrait();
    const auto& camPosition   = cam->position();
    const auto& camViewMatrix = cam->viewMatrix();
    const auto& camProjectionMatrix = cam->projectionMatrix();
    const glm::vec2& sceneViewSize  = PrototypeEngineInternalApplication::renderer->ui()->sceneView()->dimensions();
    glm::vec3        ray;
    PrototypeMaths::projectRayFromClipSpacePoint(ray, camViewMatrix, camProjectionMatrix, x, y, sceneViewSize.x, sceneViewSize.y);
    glm::vec3 pos = { camPosition.x, camPosition.y, camPosition.z };
    auto      hit = PrototypeEngineInternalApplication::physics->raycast(pos, ray, rayLength);
    if (hit.has_value()) { *hitObject = hit.value(); }
}