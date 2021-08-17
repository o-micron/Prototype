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

#include <PrototypeCommon/Definitions.h>
#include <PrototypeCommon/Maths.h>

#include <optional>

struct PrototypeScene;
struct PrototypeObject;

enum class PrototypePhysicsVehicleGear
{
    Reverse       = 0,
    NEUTRAL       = 1,
    FIRST         = 2,
    SECOND        = 3,
    THIRD         = 4,
    FOURTH        = 5,
    FIFTH         = 6,
    SIXTH         = 7,
    SEVENTH       = 8,
    EIGHTH        = 9,
    NINTH         = 10,
    TENTH         = 11,
    ELEVENTH      = 12,
    TWELFTH       = 13,
    THIRTEENTH    = 14,
    FOURTEENTH    = 15,
    FIFTEENTH     = 16,
    SIXTEENTH     = 17,
    SEVENTEENTH   = 18,
    EIGHTEENTH    = 19,
    NINETEENTH    = 20,
    TWENTIETH     = 21,
    TWENTYFIRST   = 22,
    TWENTYSECOND  = 23,
    TWENTYTHIRD   = 24,
    TWENTYFOURTH  = 25,
    TWENTYFIFTH   = 26,
    TWENTYSIXTH   = 27,
    TWENTYSEVENTH = 28,
    TWENTYEIGHTH  = 29,
    TWENTYNINTH   = 30,
    THIRTIETH     = 31
};

struct PROTOTYPE_PURE_ABSTRACT PrototypePhysics
{
    // deconstructor
    virtual ~PrototypePhysics() = default;

    // initialize physics
    virtual bool init() = 0;

    // de-initialize physics
    virtual void deInit() = 0;

    // play or resume physics simulation
    virtual void play() = 0;

    // pause physics simulation
    virtual void pause() = 0;

    // update the physics simulation (++tick)
    virtual bool update() = 0;

    // schedule a physics record pass
    virtual void scheduleRecordPass() = 0;

    // start recording instructions
    virtual void beginRecordPass() = 0;

    // stop recording instructions
    virtual void endRecordPass() = 0;

    // check if simulation is playing
    virtual bool isPlaying() = 0;

    // force move the rigidbody from a random transformation, overwrite the simulation constraints and forces etc ..
    virtual void overrideRigidbodyGlobalPos(PrototypeObject* object) = 0;

    // shoot a ray to the unknown from a point and a direction and length of the ray
    virtual std::optional<PrototypeObject*> raycast(const glm::vec3& origin, const glm::vec3& dir, const f32 length) = 0;

    // get the model matrix for the given rigidbody
    virtual void fetchModelMatrix(void* rigidbody, void* shape, glm::mat4& model) = 0;

    // create a new plane collider for the given object
    virtual void createPlaneCollider(PrototypeObject* object) = 0;

    // create a new box collider
    virtual void createBoxCollider(PrototypeObject* object) = 0;

    // create a new sphere collider for the given object
    virtual void createSphereCollider(PrototypeObject* object) = 0;

    // create a new capsule collider for the given object and geometry data
    virtual void createCapsuleCollider(const float&     radius,
                                       const float&     halfHeight,
                                       const float&     density,
                                       PrototypeObject* object) = 0;

    // create a new convex mesh collider for the given object and mesh data
    virtual void createConvexMeshCollider(const std::vector<glm::vec3>& vertices,
                                          const std::vector<u32>&       indices,
                                          PrototypeObject*              object) = 0;

    // create a new static triangulated mesh collider for the given object and mesh data
    virtual void createTriMeshCollider(const std::vector<glm::vec3>& vertices,
                                       const std::vector<u32>&       indices,
                                       PrototypeObject*              object) = 0;

    // create a new static mesh collider for the given object and mesh data
    virtual void createStaticCollider(const std::vector<glm::vec3>& vertices,
                                      const std::vector<u32>&       indices,
                                      PrototypeObject*              object) = 0;

    // create a new vehicle for the given objects
    virtual void createVehicle(const std::vector<glm::vec3>& chasisVertices,
                               const std::vector<u32>&       chasisIndices,
                               const std::vector<glm::vec3>& wheelVertices,
                               const std::vector<u32>&       wheelIndices,
                               PrototypeObject*              chasisObject,
                               PrototypeObject*              wheelFRObject,
                               PrototypeObject*              wheelFLObject,
                               PrototypeObject*              wheelBRObject,
                               PrototypeObject*              wheelBLObject) = 0;

    // control the currently driven vehicle, acceleration, brake, steer wheel ..
    virtual void updateVehicleController(PrototypeObject* object, f32 acceleration, f32 brake, f32 steer) = 0;

    // update the rigidbody for whther it's static or dynamic
    virtual void updateRigidbodyStatic(PrototypeObject* object) = 0;

    // update the rigidbody for whther it's trigger or not
    virtual void updateRigidbodyTrigger(PrototypeObject* object) = 0;

    // overwrite the simulation's linear velocity for the given object's rigidbody
    virtual void updateRigidbodyLinearVelocity(PrototypeObject* object) = 0;

    // overwrite the simulation's linear damping for the given object's rigidbody
    virtual void updateRigidbodyLinearDamping(PrototypeObject* object) = 0;

    // overwrite the simulation's angular velocity for the given object's rigidbody
    virtual void updateRigidbodyAngularVelocity(PrototypeObject* object) = 0;

    // overwrite the simulation's angular damping for the given object's rigidbody
    virtual void updateRigidbodyAngularDamping(PrototypeObject* object) = 0;

    // overwrite the simulation's mass for the given object's rigidbody
    virtual void updateRigidbodyMass(PrototypeObject* object) = 0;

    // overwrite the simulation's linear velocity lock for the given object's rigidbody
    virtual void updateRigidbodyLockLinear(PrototypeObject* object) = 0;

    // overwrite the simulation's angular velocity lock for the given object's rigidbody
    virtual void updateRigidbodyLockAngular(PrototypeObject* object) = 0;

    // changes collider for the given object
    virtual void updateCollider(PrototypeObject* object, const std::string& shapeName) = 0;

    // scales collider for the given object
    virtual void scaleCollider(PrototypeObject* object, const glm::vec3& scale) = 0;

    // creates a new rigidbody from the object's rigidbody and collider traits configuration
    virtual void createRigidbody(PrototypeObject* object) = 0;

    // destroys a rigidbody
    virtual void destroyRigidbody(void* rigidbody) = 0;

    // spawn a new vehicle
    virtual void spawnVehicle() = 0;

    // toggle vehicle access controller
    virtual void toggleVehicleAccessControl(PrototypeObject* object) = 0;

    // iterate forward to gain access control to the next vehicle in the scene
    virtual void requestNextVehicleAccessControl() = 0;

    // iterate backward to gain access control to the previous vehicle in the scene
    virtual void requestPreviousVehicleAccessControl() = 0;

    // set vehicle gears
    virtual void controlledVehiclesSetGear(PrototypePhysicsVehicleGear gear) = 0;

    // toggle vehicle gears (forward/backward)
    virtual void controlledVehiclesToggleGearDirection() = 0;

    // flips the vehicle back to normal (call in case vehicle is upside down)
    virtual void controlledVehiclesFlip() = 0;

    // called when mouse clicks events triggers
    virtual void onMouse(i32 button, i32 action, i32 mods) = 0;

    // called when mouse cursor movement event triggers
    virtual void onMouseMove(f64 x, f64 y) = 0;

    // called when mouse drag event triggers
    virtual void onMouseDrag(i32 button, f64 x, f64 y) = 0;

    // called when mouse scroll event triggers
    virtual void onMouseScroll(f64 x, f64 y) = 0;

    // called when keyboard keys events trigger
    virtual void onKeyboard(i32 key, i32 scancode, i32 action, i32 mods) = 0;

    // called when window resize event triggers
    virtual void onWindowResize(i32 width, i32 height) = 0;

    // called when drag and dropping files event triggers
    virtual void onWindowDragDrop(i32 numFiles, const char** names) = 0;
};