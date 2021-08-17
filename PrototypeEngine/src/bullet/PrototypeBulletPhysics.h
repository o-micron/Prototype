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

#include "../core/PrototypePhysics.h"

#include <PrototypeCommon/Maths.h>

struct PrototypeObject;

struct PrototypeBulletPhysics final : PrototypePhysics
{
    PrototypeBulletPhysics();
    ~PrototypeBulletPhysics() final = default;

    // initialize physics
    bool init() final;

    // de-initialize physics
    void deInit() final;

    // play or resume physics simulation
    void play() final;

    // pause physics simulation
    void pause() final;

    // update the physics simulation (++tick)
    bool update() final;

    // schedule a physics record pass
    void scheduleRecordPass() final;

    // start recording instructions
    void beginRecordPass() final;

    // stop recording instructions
    void endRecordPass() final;

    // check if simulation is playing
    bool isPlaying() final;

    // force move the rigidbody from a random transformation, overwrite the simulation constraints and forces etc ..
    void overrideRigidbodyGlobalPos(PrototypeObject* object) final;

    // shoot a ray to the unknown from a point and a direction and length of the ray
    std::optional<PrototypeObject*> raycast(const glm::vec3& origin, const glm::vec3& dir, f32 length) final;

    // get the model matrix for the given rigidbody
    void fetchModelMatrix(void* rigidbody, void* shape, glm::mat4& model) final;

    // create a new plane collider for the given object
    void createPlaneCollider(PrototypeObject* object) final;

    // create a new box collider
    void createBoxCollider(PrototypeObject* object) final;

    // create a new sphere collider for the given object
    void createSphereCollider(PrototypeObject* object) final;

    // create a new capsule collider for the given object and geometry data
    void createCapsuleCollider(const float& radius, const float& halfHeight, const float& density, PrototypeObject* object) final;

    // create a new convex mesh collider for the given object and mesh data
    void createConvexMeshCollider(const std::vector<glm::vec3>& vertices,
                                  const std::vector<u32>&       indices,
                                  PrototypeObject*              object) final;

    // create a new static triangulated mesh collider for the given object and mesh data
    void createTriMeshCollider(const std::vector<glm::vec3>& vertices,
                               const std::vector<u32>&       indices,
                               PrototypeObject*              object) final;

    // create a new static mesh collider for the given object and mesh data
    void createStaticCollider(const std::vector<glm::vec3>& vertices,
                              const std::vector<u32>&       indices,
                              PrototypeObject*              object) final;

    // create a new vehicle for the given objects
    void createVehicle(const std::vector<glm::vec3>& chasisVertices,
                       const std::vector<u32>&       chasisIndices,
                       const std::vector<glm::vec3>& wheelVertices,
                       const std::vector<u32>&       wheelIndices,
                       PrototypeObject*              chasisObject,
                       PrototypeObject*              wheelFRObject,
                       PrototypeObject*              wheelFLObject,
                       PrototypeObject*              wheelBRObject,
                       PrototypeObject*              wheelBLObject) final;

    // control the currently driven vehicle, acceleration, brake, steer wheel ..
    void updateVehicleController(PrototypeObject* object, f32 acceleration, f32 brake, f32 steer) final;

    // update the rigidbody for whther it's static or dynamic
    void updateRigidbodyStatic(PrototypeObject* object) final;

    // update the rigidbody for whther it's trigger or not
    void updateRigidbodyTrigger(PrototypeObject* object) final;

    // overwrite the simulation's linear velocity for the given object's rigidbody
    void updateRigidbodyLinearVelocity(PrototypeObject* object) final;

    // overwrite the simulation's linear damping for the given object's rigidbody
    void updateRigidbodyLinearDamping(PrototypeObject* object) final;

    // overwrite the simulation's angular velocity for the given object's rigidbody
    void updateRigidbodyAngularVelocity(PrototypeObject* object) final;

    // overwrite the simulation's angular damping for the given object's rigidbody
    void updateRigidbodyAngularDamping(PrototypeObject* object) final;

    // overwrite the simulation's mass for the given object's rigidbody
    void updateRigidbodyMass(PrototypeObject* object) final;

    // overwrite the simulation's linear velocity lock for the given object's rigidbody
    void updateRigidbodyLockLinear(PrototypeObject* object) final;

    // overwrite the simulation's angular velocity lock for the given object's rigidbody
    void updateRigidbodyLockAngular(PrototypeObject* object) final;

    // changes collider for the given object
    void updateCollider(PrototypeObject* object, const std::string& shapeName) final;

    // scales collider for the given object
    void scaleCollider(PrototypeObject* object, const glm::vec3& scale) final;

    // creates a new rigidbody from the object's rigidbody and collider traits configuration
    void createRigidbody(PrototypeObject* object) final;

    // destroys a rigidbody
    void destroyRigidbody(void* rigidbody) final;

    // spawn a new vehicle
    void spawnVehicle() final;

    // toggle vehicle access controller
    void toggleVehicleAccessControl(PrototypeObject* object) final;

    // iterate forward to gain access control to the next vehicle in the scene
    void requestNextVehicleAccessControl() final;

    // iterate backward to gain access control to the previous vehicle in the scene
    void requestPreviousVehicleAccessControl() final;

    // set vehicle gears
    void controlledVehiclesSetGear(PrototypePhysicsVehicleGear gear) final;

    // toggle vehicle gears (forward/backward)
    void controlledVehiclesToggleGearDirection() final;

    // flips the vehicle back to normal (call in case vehicle is upside down)
    void controlledVehiclesFlip() final;

    // called when mouse clicks events triggers
    void onMouse(i32 button, i32 action, i32 mods) final;

    // called when mouse cursor movement event triggers
    void onMouseMove(f64 xpos, f64 ypos) final;

    // called when mouse drag event triggers
    void onMouseDrag(i32 button, f64 xoffset, f64 yoffset) final;

    // called when mouse scroll event triggers
    void onMouseScroll(f64 xoffset, f64 yoffset) final;

    // called when keyboard keys events trigger
    void onKeyboard(i32 key, i32 scancode, i32 action, i32 mods) final;

    // called when window resize event triggers
    void onWindowResize(i32 width, i32 height) final;

    // called when drag and dropping files event triggers
    void onWindowDragDrop(i32 numFiles, const char** names) final;

  private:
    static bool _isPlaying;

    bool _needsRecord;
};
