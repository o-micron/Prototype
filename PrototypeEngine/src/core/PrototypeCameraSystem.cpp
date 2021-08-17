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

#include "PrototypeCameraSystem.h"

#include "PrototypeEngine.h"
#include "PrototypeInput.h"
#include "PrototypeWindow.h"

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include <glm/gtx/euler_angles.hpp>

extern void
CameraSystemUpdateViewMatrix(Camera* camera, f32 xdir, f32 ydir, f32 zdir)
{
    CameraSystemTranslate(camera, xdir, ydir, zdir);

    if (camera->_interpolationTime < 1.0f) {
        camera->_interpolationTime +=
          0.01f + (1.0f - camera->_interpolationTime) * 10.0f * PrototypeEngineInternalApplication::window->deltaTime();
        camera->_linearRotation = glm::lerp(camera->_linearRotation, glm::vec2(0.0f, 0.0f), camera->_interpolationTime);

        camera->_rotation.x -= camera->_linearRotation.y * camera->_lookSensitivity;
        camera->_rotation.y -= camera->_linearRotation.x * camera->_lookSensitivity;
        camera->_rotation.x   = glm::clamp(camera->_rotation.x, -80.0f, 80.0f);
        camera->_rotation.y   = fmod(camera->_rotation.y, 360.0f);
        camera->_rotationQuat = glm::eulerAngleYX(glm::radians(camera->_rotation.y), glm::radians(camera->_rotation.x));
    }

    if (camera->orbital()) {
        PrototypeMaths::buildOrbitalViewMatrix(camera->_viewMatrix, camera->_position, camera->_rotationQuat);
    } else {
        PrototypeMaths::buildFreeViewMatrix(camera->_viewMatrix, camera->_position, camera->_rotationQuat);
    }
}

extern void
CameraSystemUpdateProjectionMatrix(Camera* camera)
{
    PrototypeMaths::buildPerspectiveProjection(
      camera->_projectionMatrix, camera->_fov, camera->_aspectRatio, camera->_near, camera->_far);
}

extern void
CameraSystemSetRotation(Camera* camera, const glm::vec2& rotation)
{
    camera->_rotation          = rotation;
    camera->_rotation.x        = glm::clamp(camera->_rotation.x, -80.0f, 80.0f);
    camera->_rotation.y        = fmod(camera->_rotation.y, 360.0f);
    camera->_rotationQuat      = glm::eulerAngleYX(glm::radians(rotation.y), glm::radians(rotation.x));
    camera->_interpolationTime = 1.0f;
}

extern void
CameraSystemSetTranslation(Camera* camera, const glm::vec3& translation)
{
    camera->_position = translation;
}

extern void
CameraSystemRotate(Camera* camera, f32 x, f32 y)
{
    camera->_interpolationTime = 0.0f;
    camera->_linearRotation.x += x;
    camera->_linearRotation.y += y;
}

extern void
CameraSystemTranslate(Camera* camera, f32 x, f32 y, f32 z)
{
    if (camera->_orbital) {
        camera->_position.x -= x * camera->_moveSensitivity;
        camera->_position.y -= y * camera->_moveSensitivity;
        camera->_position.z -= z * camera->_moveSensitivity;
    } else {
        float height = camera->_position.y;
        camera->_position -= camera->_rotationQuat *
                             glm::vec3(x * camera->_moveSensitivity, y * camera->_moveSensitivity, z * camera->_moveSensitivity);
        camera->_position.y = height - y * camera->_moveSensitivity;
    }
}

extern void
CameraSystemSetResolution(Camera* camera, f32 width, f32 height)
{
    camera->_resolution  = { width, height };
    camera->_aspectRatio = width / height;
}

extern void
CameraSystemSetFov(Camera* camera, f32 fov)
{
    camera->_fov = fov;
}

extern void
CameraSystemSetNear(Camera* camera, f32 near)
{
    camera->_near = near;
}

extern void
CameraSystemSetFar(Camera* camera, f32 far)
{
    camera->_far = far;
}

extern void
CameraSystemSetViewMatrix(Camera* camera, const glm::mat4& matrix)
{
    camera->_viewMatrix = matrix;
}

extern void
CameraSystemSetProjectionMatrix(Camera* camera, const glm::mat4& matrix)
{
    camera->_projectionMatrix = matrix;
}
