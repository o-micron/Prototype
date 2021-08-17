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

#include "PrototypeBulletPhysics.h"

#include "../core/PrototypeEngine.h"
#include "../core/PrototypeRenderer.h"
#include "../core/PrototypeShortcuts.h"

#include "../core/PrototypeScene.h"

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include <PrototypeCommon/Logger.h>

#include <GLFW/glfw3.h>

#include <stdarg.h>

bool PrototypeBulletPhysics::_isPlaying = false;

PrototypeBulletPhysics::PrototypeBulletPhysics()
  : _needsRecord(true)
{}

bool
PrototypeBulletPhysics::init()
{
    return true;
}

void
PrototypeBulletPhysics::deInit()
{}

void
PrototypeBulletPhysics::play()
{
    _isPlaying = true;
}

void
PrototypeBulletPhysics::pause()
{
    _isPlaying = false;
}

bool
PrototypeBulletPhysics::update()
{
    return true;
}

void
PrototypeBulletPhysics::scheduleRecordPass()
{}

void
PrototypeBulletPhysics::beginRecordPass()
{}

void
PrototypeBulletPhysics::endRecordPass()
{}

bool
PrototypeBulletPhysics::isPlaying()
{
    return _isPlaying;
}

void
PrototypeBulletPhysics::overrideRigidbodyGlobalPos(PrototypeObject* object)
{}

std::optional<PrototypeObject*>
PrototypeBulletPhysics::raycast(const glm::vec3& origin, const glm::vec3& dir, const f32 length)
{
    return {};
}

void
PrototypeBulletPhysics::fetchModelMatrix(void* rigidbody, void* shape, glm::mat4& model)
{}

void
PrototypeBulletPhysics::createPlaneCollider(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::createBoxCollider(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::createSphereCollider(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::createCapsuleCollider(const float&     radius,
                                              const float&     halfHeight,
                                              const float&     density,
                                              PrototypeObject* object)
{}

void
PrototypeBulletPhysics::createConvexMeshCollider(const std::vector<glm::vec3>& vertices,
                                                 const std::vector<u32>&       indices,
                                                 PrototypeObject*              object)
{}

void
PrototypeBulletPhysics::createTriMeshCollider(const std::vector<glm::vec3>& vertices,
                                              const std::vector<u32>&       indices,
                                              PrototypeObject*              object)
{}

void
PrototypeBulletPhysics::createStaticCollider(const std::vector<glm::vec3>& vertices,
                                             const std::vector<u32>&       indices,
                                             PrototypeObject*              object)
{}

void
PrototypeBulletPhysics::createVehicle(const std::vector<glm::vec3>& chasisVertices,
                                      const std::vector<u32>&       chasisIndices,
                                      const std::vector<glm::vec3>& wheelVertices,
                                      const std::vector<u32>&       wheelIndices,
                                      PrototypeObject*              chasisObject,
                                      PrototypeObject*              wheelFRObject,
                                      PrototypeObject*              wheelFLObject,
                                      PrototypeObject*              wheelBRObject,
                                      PrototypeObject*              wheelBLObject)
{}

void
PrototypeBulletPhysics::updateVehicleController(PrototypeObject* object, f32 acceleration, f32 brake, f32 steer)
{}

void
PrototypeBulletPhysics::updateRigidbodyStatic(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::updateRigidbodyTrigger(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::updateRigidbodyLinearVelocity(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::updateRigidbodyLinearDamping(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::updateRigidbodyAngularVelocity(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::updateRigidbodyAngularDamping(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::updateRigidbodyMass(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::updateRigidbodyLockLinear(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::updateRigidbodyLockAngular(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::updateCollider(PrototypeObject* object, const std::string& shapeName)
{}

void
PrototypeBulletPhysics::scaleCollider(PrototypeObject* object, const glm::vec3& scale)
{}

void
PrototypeBulletPhysics::createRigidbody(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::destroyRigidbody(void* rigidbody)
{}

void
PrototypeBulletPhysics::spawnVehicle()
{}

void
PrototypeBulletPhysics::toggleVehicleAccessControl(PrototypeObject* object)
{}

void
PrototypeBulletPhysics::requestNextVehicleAccessControl()
{}

void
PrototypeBulletPhysics::requestPreviousVehicleAccessControl()
{}

void
PrototypeBulletPhysics::controlledVehiclesSetGear(PrototypePhysicsVehicleGear gear)
{}

void
PrototypeBulletPhysics::controlledVehiclesToggleGearDirection()
{}

void
PrototypeBulletPhysics::controlledVehiclesFlip()
{}

void
PrototypeBulletPhysics::onMouse(i32 button, i32 action, i32 mods)
{}

void
PrototypeBulletPhysics::onMouseMove(f64 x, f64 y)
{}

void
PrototypeBulletPhysics::onMouseDrag(i32 button, f64 x, f64 y)
{}

void
PrototypeBulletPhysics::onMouseScroll(f64 x, f64 y)
{}

void
PrototypeBulletPhysics::onKeyboard(i32 key, i32 scancode, i32 action, i32 mods)
{}

void
PrototypeBulletPhysics::onWindowResize(i32 width, i32 height)
{}

void
PrototypeBulletPhysics::onWindowDragDrop(i32 numFiles, const char** names)
{}