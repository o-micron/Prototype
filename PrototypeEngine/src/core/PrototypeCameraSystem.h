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

struct Camera;
struct PrototypeSystem;

extern void
CameraSystemUpdateViewMatrix(Camera* camera, f32 xdir, f32 ydir, f32 zdir);
extern void
CameraSystemUpdateProjectionMatrix(Camera* camera);
extern void
CameraSystemSetRotation(Camera* camera, const glm::vec2& rotation);
extern void
CameraSystemSetTranslation(Camera* camera, const glm::vec3& translation);
extern void
CameraSystemRotate(Camera* camera, f32 x, f32 y);
extern void
CameraSystemTranslate(Camera* camera, f32 x, f32 y, f32 z);
extern void
CameraSystemSetResolution(Camera* camera, f32 width, f32 height);
extern void
CameraSystemSetFov(Camera* camera, f32 fov);
extern void
CameraSystemSetNear(Camera* camera, f32 near);
extern void
CameraSystemSetFar(Camera* camera, f32 far);
extern void
CameraSystemSetViewMatrix(Camera* camera, const glm::mat4& matrix);
extern void
CameraSystemSetProjectionMatrix(Camera* camera, const glm::mat4& matrix);