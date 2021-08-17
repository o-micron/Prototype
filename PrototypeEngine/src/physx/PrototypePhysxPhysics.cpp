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

#include "PrototypePhysxPhysics.h"

#include "../core/PrototypeDatabase.h"
#include "../core/PrototypeMeshBuffer.h"
#include "../core/PrototypeSceneLayer.h"
#include "../core/PrototypeSceneNode.h"

#include "../core/PrototypeEngine.h"
#include "../core/PrototypeRenderer.h"
#include "../core/PrototypeScene.h"
#include "../core/PrototypeShortcuts.h"
#include "../core/PrototypeWindow.h"

#include "../core/PrototypeUI.h"

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include <PrototypeCommon/Logger.h>

#include <GLFW/glfw3.h>

#include <snippetvehiclecommon/SnippetVehicleCreate.h>
#include <snippetvehiclecommon/SnippetVehicleFilterShader.h>
#include <snippetvehiclecommon/SnippetVehicleSceneQuery.h>
#include <snippetvehiclecommon/SnippetVehicleTireFriction.h>
#include <vehicle/PxVehicleUtil.h>

#include <snippetcommon/SnippetPVD.h>
#include <snippetcommon/SnippetPrint.h>
#include <snippetutils/SnippetUtils.h>

#define PVD_HOST "127.0.0.1" // the IP address of the system running the PhysX Visual Debugger that you want to connect to.

PxDefaultAllocator                                                PrototypePhysxPhysics::gAllocator;
PrototypePhysxEventsCallback*                                     PrototypePhysxPhysics::gEventsCallback         = nullptr;
PxFoundation*                                                     PrototypePhysxPhysics::gFoundation             = nullptr;
PxPhysics*                                                        PrototypePhysxPhysics::gPhysics                = nullptr;
PxDefaultCpuDispatcher*                                           PrototypePhysxPhysics::gDispatcher             = nullptr;
PxCooking*                                                        PrototypePhysxPhysics::gCooking                = nullptr;
PxScene*                                                          PrototypePhysxPhysics::gScene                  = nullptr;
PxMaterial*                                                       PrototypePhysxPhysics::gMaterial               = nullptr;
PxPvd*                                                            PrototypePhysxPhysics::gPvd                    = nullptr;
snippetvehicle::VehicleSceneQueryData*                            PrototypePhysxPhysics::gVehicleSceneQueryData  = nullptr;
physx::PxBatchQuery*                                              PrototypePhysxPhysics::gBatchQuery             = nullptr;
physx::PxVehicleDrivableSurfaceToTireFrictionPairs*               PrototypePhysxPhysics::gFrictionPairs          = nullptr;
physx::PxVehicleDrive4W**                                         PrototypePhysxPhysics::gVehicles               = nullptr;
physx::PxVehicleDrive4WRawInputData*                              PrototypePhysxPhysics::gVehicleInputData       = nullptr;
size_t*                                                           PrototypePhysxPhysics::gNumVehicles            = nullptr;
size_t*                                                           PrototypePhysxPhysics::gControlledVehicleIndex = nullptr;
std::array<PxWheelQueryResult, PROTOTYPE_MAX_NUM_VEHICLES * 4>    PrototypePhysxPhysics::gWheelQueryResults;
std::array<PxVehicleWheelQueryResult, PROTOTYPE_MAX_NUM_VEHICLES> PrototypePhysxPhysics::gVehiclesQueryResults;
bool                                                              PrototypePhysxPhysics::_isPlaying = true;
std::unordered_map<std::string, PhysxSceneData>                   PrototypePhysxPhysics::_scenes;

PxDefaultErrorCallback defaultErrorCallback;

PxFilterFlags
PrototypeFilterShader(PxFilterObjectAttributes attributes0,
                      PxFilterData             filterData0,
                      PxFilterObjectAttributes attributes1,
                      PxFilterData             filterData1,
                      PxPairFlags&             pairFlags,
                      const void*              constantBlock,
                      PxU32                    constantBlockSize)
{
    PX_UNUSED(constantBlock);
    PX_UNUSED(constantBlockSize);

    // let triggers through
    if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)) {
        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        return PxFilterFlag::eDEFAULT;
    }

    if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
        return PxFilterFlag::eSUPPRESS;

    pairFlags = PxPairFlag::eCONTACT_DEFAULT;
    pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));

    return PxFilterFlags();
}

PxF32 gSteerVsForwardSpeedData[2 * 8] = {
    0.0f,       0.75f,      //
    5.0f,       0.75f,      //
    30.0f,      0.125f,     //
    120.0f,     0.1f,       //
    PX_MAX_F32, PX_MAX_F32, //
    PX_MAX_F32, PX_MAX_F32, //
    PX_MAX_F32, PX_MAX_F32, //
    PX_MAX_F32, PX_MAX_F32  //
};

PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

PxVehicleKeySmoothingData gKeySmoothingData = { {
                                                  6.0f, // rise rate eANALOG_INPUT_ACCEL
                                                  6.0f, // rise rate eANALOG_INPUT_BRAKE
                                                  6.0f, // rise rate eANALOG_INPUT_HANDBRAKE
                                                  2.5f, // rise rate eANALOG_INPUT_STEER_LEFT
                                                  2.5f, // rise rate eANALOG_INPUT_STEER_RIGHT
                                                },
                                                {
                                                  10.0f, // fall rate eANALOG_INPUT_ACCEL
                                                  10.0f, // fall rate eANALOG_INPUT_BRAKE
                                                  10.0f, // fall rate eANALOG_INPUT_HANDBRAKE
                                                  5.0f,  // fall rate eANALOG_INPUT_STEER_LEFT
                                                  5.0f   // fall rate eANALOG_INPUT_STEER_RIGHT
                                                } };

PxVehiclePadSmoothingData gPadSmoothingData = { {
                                                  6.0f, // rise rate eANALOG_INPUT_ACCEL
                                                  6.0f, // rise rate eANALOG_INPUT_BRAKE
                                                  6.0f, // rise rate eANALOG_INPUT_HANDBRAKE
                                                  2.5f, // rise rate eANALOG_INPUT_STEER_LEFT
                                                  2.5f, // rise rate eANALOG_INPUT_STEER_RIGHT
                                                },
                                                {
                                                  10.0f, // fall rate eANALOG_INPUT_ACCEL
                                                  10.0f, // fall rate eANALOG_INPUT_BRAKE
                                                  10.0f, // fall rate eANALOG_INPUT_HANDBRAKE
                                                  5.0f,  // fall rate eANALOG_INPUT_STEER_LEFT
                                                  5.0f   // fall rate eANALOG_INPUT_STEER_RIGHT
                                                } };

enum DriveMode
{
    eDRIVE_MODE_ACCEL_FORWARDS = 0,
    eDRIVE_MODE_ACCEL_REVERSE,
    eDRIVE_MODE_HARD_TURN_LEFT,
    eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
    eDRIVE_MODE_HARD_TURN_RIGHT,
    eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
    eDRIVE_MODE_BRAKE,
    eDRIVE_MODE_NONE
};

DriveMode gDriveModeOrder[] = { eDRIVE_MODE_BRAKE,
                                eDRIVE_MODE_ACCEL_FORWARDS,
                                eDRIVE_MODE_BRAKE,
                                eDRIVE_MODE_ACCEL_REVERSE,
                                eDRIVE_MODE_BRAKE,
                                eDRIVE_MODE_HARD_TURN_LEFT,
                                eDRIVE_MODE_BRAKE,
                                eDRIVE_MODE_HARD_TURN_RIGHT,
                                eDRIVE_MODE_ACCEL_FORWARDS,
                                eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
                                eDRIVE_MODE_ACCEL_FORWARDS,
                                eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
                                eDRIVE_MODE_NONE };

snippetvehicle::VehicleDesc
PrototypePhysxPhysics::vehicleInitDesc()
{
    // Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    // The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    // Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
    const PxF32  chassisMass = 1000.0f;
    const PxVec3 chassisDims(2.38f, 1.569f, 5.65f);
    const PxVec3 chassisMOI((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
                            (chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
                            (chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
    const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

    // Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    // Moment of inertia is just the moment of inertia of a cylinder.
    const PxF32 wheelMass   = 40.0f;
    const PxF32 wheelRadius = 0.55f;
    const PxF32 wheelWidth  = 0.344f;
    const PxF32 wheelMOI    = 0.5f * wheelMass * wheelRadius * wheelRadius;
    const PxU32 nbWheels    = 4;

    snippetvehicle::VehicleDesc vehicleDesc;

    vehicleDesc.chassisMass     = chassisMass;
    vehicleDesc.chassisDims     = chassisDims;
    vehicleDesc.chassisMOI      = chassisMOI;
    vehicleDesc.chassisCMOffset = chassisCMOffset;
    vehicleDesc.chassisMaterial = gMaterial;
    vehicleDesc.chassisSimFilterData =
      PxFilterData(snippetvehicle::COLLISION_FLAG_CHASSIS, snippetvehicle::COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

    vehicleDesc.wheelMass     = wheelMass;
    vehicleDesc.wheelRadius   = wheelRadius;
    vehicleDesc.wheelWidth    = wheelWidth;
    vehicleDesc.wheelMOI      = wheelMOI;
    vehicleDesc.numWheels     = nbWheels;
    vehicleDesc.wheelMaterial = gMaterial;
    vehicleDesc.wheelSimFilterData =
      PxFilterData(snippetvehicle::COLLISION_FLAG_WHEEL, snippetvehicle::COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

    return vehicleDesc;
}

void
PrototypePhysxPhysics::vehicleReleaseAllControls(size_t vehicleIndex)
{
    gVehicleInputData[vehicleIndex].setAnalogAccel(0.0f);
    gVehicleInputData[vehicleIndex].setAnalogSteer(0.0f);
    gVehicleInputData[vehicleIndex].setAnalogBrake(0.0f);
    gVehicleInputData[vehicleIndex].setAnalogHandbrake(0.0f);
}

PrototypePhysxPhysics::PrototypePhysxPhysics()
  : _needsRecord(true)
{}

bool
PrototypePhysxPhysics::init()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, defaultErrorCallback);
    if (!gFoundation) PrototypeLogger::fatal("PxCreateFoundation failed!");

#if defined(PROTOTYPE_DEBUG_PHYSX)
    gPvd                      = PxCreatePvd(*gFoundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
    if (!gPhysics) PrototypeLogger::fatal("PxCreatePhysics failed!");

    static physx::PxCookingParams gCookingParams = PxTolerancesScale();
    gCookingParams.midphaseDesc.setToDefault(PxMeshMidPhase::eBVH34);
    gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(gCookingParams));
    if (!gCooking) PrototypeLogger::fatal("PxCreateCooking failed!");

    PxU32 numWorkers = std::thread::hardware_concurrency();
    numWorkers       = numWorkers == 0 ? 0 : numWorkers - 1;
    gDispatcher      = PxDefaultCpuDispatcherCreate(numWorkers);

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f); // , , jumping reaction

    // vehicle
    PxInitVehicleSDK(*gPhysics);
    PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, -1));
    PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

    for (size_t i = 0; i < PROTOTYPE_MAX_NUM_VEHICLES; ++i) {
        gVehiclesQueryResults[i].nbWheelQueryResults = 4;
        gVehiclesQueryResults[i].wheelQueryResults   = &gWheelQueryResults[i * 4];
    }

    PrototypePhysxPhysics::gEventsCallback = PROTOTYPE_NEW PrototypePhysxEventsCallback();

    return true;
}

void
PrototypePhysxPhysics::deInit()
{
    for (auto pair : _scenes) {
        for (size_t i = 0; i < pair.second.numVehicles; ++i) {
            pair.second.vehicles[i]->getRigidDynamicActor()->release();
            pair.second.vehicles[i]->free();
        }
        PX_RELEASE(pair.second.batchQuery);
        pair.second.vehicleSceneQueryData->free(gAllocator);
        PX_RELEASE(pair.second.frictionPairs);
    }
    PxCloseVehicleSDK();

    // PX_RELEASE(gGroundPlane);
    PX_RELEASE(gMaterial);
    PX_RELEASE(gCooking)
    for (auto pair : _scenes) {
        PX_RELEASE(pair.second.scene);
        gScene = nullptr;
    }
    _scenes.clear();

    PX_RELEASE(gDispatcher)
    PX_RELEASE(gPhysics)
#if defined(PROTOTYPE_DEBUG_PHYSX)
    if (gPvd) {
        PxPvdTransport* transport = gPvd->getTransport();
        gPvd->release();
        gPvd = nullptr;
        PX_RELEASE(transport)
    }
#endif
    PX_RELEASE(gFoundation)

    delete PrototypePhysxPhysics::gEventsCallback;
}

void
PrototypePhysxPhysics::play()
{
    _isPlaying = true;
    PrototypeEngineInternalApplication::window->resetDeltaTime();
}

void
PrototypePhysxPhysics::pause()
{
    _isPlaying = false;
}

bool
PrototypePhysxPhysics::update()
{
    if (_isPlaying) {
        f32 timestep = (f32)PrototypeEngineInternalApplication::window->deltaTime();
        // TODO:
        // Select the correct scene using the provided PrototypeScene
        // static f32 rate = 1.0f / PrototypeEngineInternalApplication::window->refreshRate();
        gScene->simulate(timestep);
        auto colliderObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(
          PrototypeTraitTypeMaskCollider | PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskRigidbody);

        auto vehicleObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(
          PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskVehicleChasis);
        gScene->fetchResults(true);

        if (*gNumVehicles > 0) {
            // Raycasts.
            PxRaycastQueryResult* raycastResults     = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
            const PxU32           raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
            PxVehicleSuspensionRaycasts(
              gBatchQuery, *gNumVehicles, (PxVehicleWheels**)gVehicles, raycastResultsSize, raycastResults);

            // Vehicle update.
            const PxVec3 grav = gScene->getGravity();
            // PxWheelQueryResult        wheelQueryResults[4];
            // PxVehicleWheelQueryResult vehicleQueryResults[1] = { { wheelQueryResults,
            //                                                        gVehicles[voi]->mWheelsSimData.getNbWheels() } };
            PxVehicleUpdates(
              timestep, grav, *gFrictionPairs, *gNumVehicles, (PxVehicleWheels**)gVehicles, gVehiclesQueryResults.data());

            {
                auto vehicleObjectIt = vehicleObjects.begin();
                std::advance(vehicleObjectIt, *gControlledVehicleIndex);
                PrototypeObject* vehicleObject = *vehicleObjectIt;
                VehicleChasis*   vehicleChasis = vehicleObject->getVehicleChasisTrait();
                if (vehicleChasis->wheelBLObject() && vehicleChasis->wheelBRObject() && vehicleChasis->wheelFLObject() &&
                    vehicleChasis->wheelFRObject()) {
                    size_t vehicleIndex   = vehicleChasis->vehicleIndex();
                    bool   vehicleIsInAir = false;
                    // Update the control inputs for the vehicle.
                    PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData,
                                                                            gSteerVsForwardSpeedTable,
                                                                            gVehicleInputData[vehicleIndex],
                                                                            timestep,
                                                                            vehicleIsInAir,
                                                                            *gVehicles[vehicleIndex]);
                    // Work out if the vehicle is in the air.
                    // vehicleIsInAir = gVehicles[*gControlledVehicleIndex]->getRigidDynamicActor()->isSleeping()
                    //                    ? false
                    //                    : PxVehicleIsInAir(gVehiclesQueryResults[*gControlledVehicleIndex]);
                }
            }

            for (size_t i = 0; i < *gNumVehicles; ++i) {
                auto vehicleObjectIt = vehicleObjects.begin();
                std::advance(vehicleObjectIt, i);
                PrototypeObject* vehicleObject = *vehicleObjectIt;
                VehicleChasis*   vehicleChasis = vehicleObject->getVehicleChasisTrait();
                if (vehicleChasis->wheelBLObject() && vehicleChasis->wheelBRObject() && vehicleChasis->wheelFLObject() &&
                    vehicleChasis->wheelFRObject()) {
                    physx::PxVehicleDrive4W* veh         = (physx::PxVehicleDrive4W*)vehicleChasis->vehicleRef();
                    physx::PxRigidDynamic*   chasisActor = veh->getRigidDynamicActor();
                    if (!chasisActor->isSleeping()) {
                        Transform* chasisTr  = vehicleObject->getTransformTrait();
                        Transform* wheelFRTr = vehicleChasis->wheelFRObject()->getTransformTrait();
                        Transform* wheelFLTr = vehicleChasis->wheelFLObject()->getTransformTrait();
                        Transform* wheelBRTr = vehicleChasis->wheelBRObject()->getTransformTrait();
                        Transform* wheelBLTr = vehicleChasis->wheelBLObject()->getTransformTrait();

                        PxMat44 chasisMat = PxMat44(chasisActor->getGlobalPose());

                        chasisTr->setModel(chasisMat.front());

                        PxMat44 wheelFRMat = PxMat44(gWheelQueryResults[(*gControlledVehicleIndex) * 4 + 0].localPose);
                        PxMat44 wheelFLMat = PxMat44(gWheelQueryResults[(*gControlledVehicleIndex) * 4 + 1].localPose);
                        PxMat44 wheelBRMat = PxMat44(gWheelQueryResults[(*gControlledVehicleIndex) * 4 + 2].localPose);
                        PxMat44 wheelBLMat = PxMat44(gWheelQueryResults[(*gControlledVehicleIndex) * 4 + 3].localPose);

                        wheelFRTr->setModel(wheelFRMat.front());
                        wheelFRMat = chasisMat * wheelFRMat;
                        // wheelFRMat *= PxMat44(PxVec4(0.5f, 0.5f, 0.5f, 1.0f));
                        wheelFRTr->setModelScaled(wheelFRMat.front());

                        wheelFLTr->setModel(wheelFLMat.front());
                        wheelFLMat = chasisMat * wheelFLMat;
                        // wheelFLMat *= PxMat44(PxVec4(0.5f, 0.5f, 0.5f, 1.0f));
                        wheelFLTr->setModelScaled(wheelFLMat.front());

                        wheelBRTr->setModel(wheelBRMat.front());
                        wheelBRMat = chasisMat * wheelBRMat;
                        // wheelBRMat *= PxMat44(PxVec4(0.5f, 0.5f, 0.5f, 1.0f));
                        wheelBRTr->setModelScaled(wheelBRMat.front());

                        wheelBLTr->setModel(wheelBLMat.front());
                        wheelBLMat = chasisMat * wheelBLMat;
                        // wheelBLMat *= PxMat44(PxVec4(0.5f, 0.5f, 0.5f, 1.0f));
                        wheelBLTr->setModelScaled(wheelBLMat.front());

                        // chasisMat *= PxMat44(PxVec4(1.75f, 1.0f, 2.5f, 1.0f));
                        chasisTr->setModelScaled(chasisMat.front());
                    }
                }
            }
        }

        for (const auto& colliderObject : colliderObjects) {
            Rigidbody* rb = colliderObject->getRigidbodyTrait();
            if (!rb) { continue; }
            auto actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
            if (!actor) { continue; }
            Transform* tr = colliderObject->getTransformTrait();
            if (!tr->needsPhysicsSync()) {
                if (rb->isStatic()) { continue; }
                if (!actor->is<PxRigidDynamic>()->isSleeping()) {
                    auto    rigidDynamicActor = actor->is<PxRigidDynamic>();
                    PxMat44 m                 = PxMat44(actor->getGlobalPose());
                    tr->setModel(m.front());
                    tr->updateComponentsFromMatrix();
                    if (static_cast<PrototypeSceneNode*>(colliderObject->parentNode())->isSelected()) {
                        PxVec3 tempLinearVelocity  = rigidDynamicActor->getLinearVelocity();
                        PxVec3 tempAngularVelocity = rigidDynamicActor->getAngularVelocity();
                        rb->setLinearVelocity(*((glm::vec3*)&tempLinearVelocity));
                        rb->setLinearDamping(rigidDynamicActor->getLinearDamping());
                        rb->setAngularVelocity(*((glm::vec3*)&tempAngularVelocity));
                        rb->setAngularDamping(rigidDynamicActor->getAngularDamping());
                    }
                }
            } else {
                auto m = (PxMat44*)(&tr->model()[0][0]);
                actor->setGlobalPose(PxTransform(*m));
                tr->setNeedsPhysicsSync(false);
            }
        }
    } else {
        auto colliderObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(
          PrototypeTraitTypeMaskCollider | PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskRigidbody);

        for (const auto& colliderObject : colliderObjects) {
            Rigidbody* rb    = colliderObject->getRigidbodyTrait();
            auto       actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
            if (!actor) { continue; }
            Transform* tr = colliderObject->getTransformTrait();
            if (tr->needsPhysicsSync()) {
                auto m = (PxMat44*)(&tr->model()[0][0]);
                actor->setGlobalPose(PxTransform(*m));
                tr->setNeedsPhysicsSync(false);
            }
        }
    }

    auto selectedObjects = PrototypeEngineInternalApplication::scene->selectedNodes();
    for (const auto& node : selectedObjects) {
        auto optObj = node->object();
        if (optObj.has_value()) {
            auto obj = optObj.value();
            if (obj->has(PrototypeTraitTypeMaskCollider | PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskRigidbody)) {
                Rigidbody* rb = obj->getRigidbodyTrait();
                if (!rb) { continue; }
                auto actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
                if (!actor) { continue; }
                auto tr = obj->getTransformTrait();
                if (tr->needsPhysicsSync()) {
                    auto m = (PxMat44*)(&tr->model()[0][0]);
                    actor->setGlobalPose(PxTransform(*m));
                    tr->setNeedsPhysicsSync(false);
                } else {
                    if (rb->isStatic()) { continue; }
                    if (!actor->is<PxRigidDynamic>()->isSleeping()) {
                        auto    rigidDynamicActor = actor->is<PxRigidDynamic>();
                        PxMat44 m                 = PxMat44(actor->getGlobalPose());
                        tr->setModel(m.front());
                        tr->updateComponentsFromMatrix();
                        PxVec3 tempLinearVelocity  = rigidDynamicActor->getLinearVelocity();
                        PxVec3 tempAngularVelocity = rigidDynamicActor->getAngularVelocity();
                        rb->setLinearVelocity(*((glm::vec3*)&tempLinearVelocity));
                        rb->setLinearDamping(rigidDynamicActor->getLinearDamping());
                        rb->setAngularVelocity(*((glm::vec3*)&tempAngularVelocity));
                        rb->setAngularDamping(rigidDynamicActor->getAngularDamping());
                    }
                }
            }
        }
    }
    return true;
}

void
PrototypePhysxPhysics::scheduleRecordPass()
{
    _needsRecord = true;
}

void
PrototypePhysxPhysics::beginRecordPass()
{
    if (!_needsRecord) return;

    _needsRecord = false;

    std::string currentSceneName = PrototypeEngineInternalApplication::scene->name();
    auto        it               = _scenes.find(currentSceneName);
    if (it == _scenes.end()) {
        PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity       = PxVec3(0.0f, -9.81f, 0.0f);
        sceneDesc.cpuDispatcher = gDispatcher;
        sceneDesc.filterShader  = PrototypeFilterShader;

        PhysxSceneData sceneData = {};
        sceneData.scene          = gPhysics->createScene(sceneDesc);
        // Create the batched scene queries for the suspension raycasts.

        sceneData.vehicleSceneQueryData =
          snippetvehicle::VehicleSceneQueryData::allocate(PROTOTYPE_MAX_NUM_VEHICLES,
                                                          PX_MAX_NB_WHEELS,
                                                          1,
                                                          PROTOTYPE_MAX_NUM_VEHICLES,
                                                          snippetvehicle::WheelSceneQueryPreFilterBlocking,
                                                          NULL,
                                                          gAllocator);
        sceneData.batchQuery =
          snippetvehicle::VehicleSceneQueryData::setUpBatchedSceneQuery(0, *sceneData.vehicleSceneQueryData, sceneData.scene);

        // Create the friction table for each combination of tire and surface type.
        sceneData.frictionPairs          = snippetvehicle::createFrictionPairs(gMaterial);
        sceneData.numVehicles            = 0;
        sceneData.controlledVehicleIndex = -1;
        sceneData.scene->setSimulationEventCallback(PrototypePhysxPhysics::gEventsCallback);

        // insert scene data
        _scenes.insert({ currentSceneName, sceneData });
        gScene                  = _scenes[currentSceneName].scene;
        gVehicleSceneQueryData  = _scenes[currentSceneName].vehicleSceneQueryData;
        gBatchQuery             = _scenes[currentSceneName].batchQuery;
        gFrictionPairs          = _scenes[currentSceneName].frictionPairs;
        gVehicles               = _scenes[currentSceneName].vehicles;
        gVehicleInputData       = _scenes[currentSceneName].vehicleInputData;
        gNumVehicles            = &_scenes[currentSceneName].numVehicles;
        gControlledVehicleIndex = &_scenes[currentSceneName].controlledVehicleIndex;

        auto colliderObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(
          PrototypeTraitTypeMaskCollider | PrototypeTraitTypeMaskTransform | PrototypeTraitTypeMaskRigidbody);
        for (auto& colliderObject : colliderObjects) {
            // const auto& position = colliderObject.second->getTransformTrait()->position();
            // const auto& rotation = colliderObject.second->getTransformTrait()->rotation();
            // const auto& scale    = colliderObject.second->getTransformTrait()->scale();
            Collider* collider = colliderObject->getColliderTrait();
            switch (collider->shapeType()) {
                case ColliderShape_Plane: {
                    createPlaneCollider(&(*colliderObject));
                    collider->setNameRef("PLANE");
                } break;

                case ColliderShape_Box: {
                    createBoxCollider(&(*colliderObject));
                    collider->setNameRef("CUBE");
                } break;

                case ColliderShape_Sphere: {
                    createSphereCollider(&(*colliderObject));
                } break;

                case ColliderShape_Capsule: {
                    createCapsuleCollider(collider->radius(), collider->height(), collider->density(), &(*colliderObject));
                    collider->setNameRef("CAPSULE");
                } break;

                case ColliderShape_ConvexMesh: {
                    const std::string      meshName = colliderObject->getMeshRendererTrait()->data()[0].mesh;
                    auto                   source = PrototypeEngineInternalApplication::database->meshBuffers[meshName]->source();
                    std::vector<glm::vec3> vertices(source.vertices.size());
                    for (size_t v = 0; v < source.vertices.size(); ++v) {
                        vertices[v].x = source.vertices[v].positionU.x;
                        vertices[v].y = source.vertices[v].positionU.y;
                        vertices[v].z = source.vertices[v].positionU.z;
                    }
                    createConvexMeshCollider(vertices, source.indices, &(*colliderObject));
                    collider->setNameRef(std::string("(CONVEX) ").append(meshName));
                } break;

                case ColliderShape_TriangleMesh: {
                    const std::string      meshName = colliderObject->getMeshRendererTrait()->data()[0].mesh;
                    auto                   source = PrototypeEngineInternalApplication::database->meshBuffers[meshName]->source();
                    std::vector<glm::vec3> vertices(source.vertices.size());
                    for (size_t v = 0; v < source.vertices.size(); ++v) {
                        vertices[v].x = source.vertices[v].positionU.x;
                        vertices[v].y = source.vertices[v].positionU.y;
                        vertices[v].z = source.vertices[v].positionU.z;
                    }
                    createTriMeshCollider(vertices, source.indices, &(*colliderObject));
                    collider->setNameRef(std::string("(TRIMESH) ").append(meshName));
                } break;

                default: {
                    PrototypeLogger::fatal("Unhandled collider type");
                } break;
            }
        }
    } else {
        gScene                  = _scenes[currentSceneName].scene;
        gVehicleSceneQueryData  = _scenes[currentSceneName].vehicleSceneQueryData;
        gBatchQuery             = _scenes[currentSceneName].batchQuery;
        gFrictionPairs          = _scenes[currentSceneName].frictionPairs;
        gVehicles               = _scenes[currentSceneName].vehicles;
        gVehicleInputData       = _scenes[currentSceneName].vehicleInputData;
        gNumVehicles            = &_scenes[currentSceneName].numVehicles;
        gControlledVehicleIndex = &_scenes[currentSceneName].controlledVehicleIndex;
    }
#if defined(PROTOTYPE_DEBUG_PHYSX)
    gPvd->disconnect();
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient) {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
#endif
}

void
PrototypePhysxPhysics::endRecordPass()
{}

bool
PrototypePhysxPhysics::isPlaying()
{
    return _isPlaying;
}

void
PrototypePhysxPhysics::overrideRigidbodyGlobalPos(PrototypeObject* object)
{}

std::optional<PrototypeObject*>
PrototypePhysxPhysics::raycast(const glm::vec3& origin, const glm::vec3& dir, const f32 length)
{
    PxVec3          position(origin.x, origin.y, origin.z);
    PxVec3          orientation(dir.x, dir.y, dir.z);
    PxRaycastBuffer hit;
    if (gScene->raycast(position, orientation, length, hit)) {
        if (hit.block.actor && hit.block.actor->userData) { return { (PrototypeObject*)hit.block.actor->userData }; }
    }
    return {};
}

void
PrototypePhysxPhysics::fetchModelMatrix(void* rigidbody, void* shape, glm::mat4& model)
{
    auto _shape     = static_cast<PxShape*>(shape);
    auto _rigidbody = static_cast<PxRigidActor*>(rigidbody);
    auto m          = PxMat44(_rigidbody->getGlobalPose());
    memcpy(&model[0][0], &m.column0.x, sizeof(f32) * 16);
}

void
PrototypePhysxPhysics::createPlaneCollider(PrototypeObject* object)
{
    Transform*       tr              = object->getTransformTrait();
    Rigidbody*       rb              = object->getRigidbodyTrait();
    Collider*        collider        = object->getColliderTrait();
    const glm::vec3& position        = tr->position();
    const glm::vec3& rotation        = tr->rotation();
    glm::vec3        linearVelocity  = rb->linearVelocity();
    f32              linearDamping   = rb->linearDamping();
    glm::vec3        angularVelocity = rb->angularVelocity();
    f32              angularDamping  = rb->angularDamping();
    f32              mass            = rb->mass();
    bool             lockLinearX     = rb->lockLinearX();
    bool             lockLinearY     = rb->lockLinearY();
    bool             lockLinearZ     = rb->lockLinearZ();
    bool             lockAngularX    = rb->lockAngularX();
    bool             lockAngularY    = rb->lockAngularY();
    bool             lockAngularZ    = rb->lockAngularZ();

    glm::quat   qat = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
    PxTransform t(position.x, position.y, position.z, PxQuat(qat.x, qat.y, qat.z, qat.w));

    PxRigidActor* rigidbody;
    if (rb->isStatic()) {
        rigidbody = gPhysics->createRigidStatic(t);
    } else {
        rigidbody                        = gPhysics->createRigidDynamic(t);
        PxRigidDynamic* rigidbodyDynamic = ((PxRigidDynamic*)rigidbody);
        rigidbodyDynamic->setLinearVelocity(PxVec3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
        rigidbodyDynamic->setLinearDamping(PxReal(linearDamping));
        rigidbodyDynamic->setAngularVelocity(PxVec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
        rigidbodyDynamic->setAngularDamping(PxReal(angularDamping));
        rigidbodyDynamic->setMass(PxReal(mass));
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, lockLinearX);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, lockLinearY);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, lockLinearZ);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, lockAngularX);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, lockAngularY);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, lockAngularZ);
    }

    PxShape* shape = PxRigidActorExt::createExclusiveShape(*rigidbody, PxPlaneGeometry(), *gMaterial);
    shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !rb->isTrigger());
    shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, rb->isTrigger());
    if (!rb->isTrigger()) {
        PxFilterData groundPlaneSimFilterData(
          snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE, snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST, 0, 0);
        // Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
        PxFilterData qryFilterData;
        snippetvehicle::setupDrivableSurface(qryFilterData);
        shape->setQueryFilterData(qryFilterData);
        // Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the
        // wheels.
        shape->setSimulationFilterData(groundPlaneSimFilterData);
    }

    rigidbody->userData = (void*)object;
    rb->setRigidbodyRef(static_cast<void*>(rigidbody));
    shape->userData = (void*)object;
    collider->setShapeRef(static_cast<void*>(shape));
    gScene->addActor(*rigidbody);
}

void
PrototypePhysxPhysics::createBoxCollider(PrototypeObject* object)
{
    Transform*       tr               = object->getTransformTrait();
    Rigidbody*       rb               = object->getRigidbodyTrait();
    Collider*        collider         = object->getColliderTrait();
    const glm::vec3& position         = tr->position();
    const glm::vec3& rotation         = tr->rotation();
    const glm::vec3& scale            = tr->scale();
    glm::vec3        linearVelociity  = rb->linearVelocity();
    f32              linearDamping    = rb->linearDamping();
    glm::vec3        angularVelociity = rb->angularVelocity();
    f32              angularDamping   = rb->angularDamping();
    f32              mass             = rb->mass();
    bool             lockLinearX      = rb->lockLinearX();
    bool             lockLinearY      = rb->lockLinearY();
    bool             lockLinearZ      = rb->lockLinearZ();
    bool             lockAngularX     = rb->lockAngularX();
    bool             lockAngularY     = rb->lockAngularY();
    bool             lockAngularZ     = rb->lockAngularZ();

    glm::quat   qat = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
    PxTransform t(position.x, position.y, position.z, PxQuat(qat.x, qat.y, qat.z, qat.w));

    PxRigidActor* rigidbody;
    if (rb->isStatic()) {
        rigidbody = gPhysics->createRigidStatic(t);
    } else {
        rigidbody                        = gPhysics->createRigidDynamic(t);
        PxRigidDynamic* rigidbodyDynamic = ((PxRigidDynamic*)rigidbody);
        rigidbodyDynamic->setLinearVelocity(PxVec3(linearVelociity.x, linearVelociity.y, linearVelociity.z));
        rigidbodyDynamic->setLinearDamping(PxReal(linearDamping));
        rigidbodyDynamic->setAngularVelocity(PxVec3(angularVelociity.x, angularVelociity.y, angularVelociity.z));
        rigidbodyDynamic->setAngularDamping(PxReal(angularDamping));
        rigidbodyDynamic->setMass(PxReal(mass));
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, lockLinearX);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, lockLinearY);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, lockLinearZ);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, lockAngularX);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, lockAngularY);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, lockAngularZ);
    }

    PxShape* shape =
      PxRigidActorExt::createExclusiveShape(*rigidbody, PxBoxGeometry(PxVec3(scale.x, scale.y, scale.z)), *gMaterial);
    shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !rb->isTrigger());
    shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, rb->isTrigger());
    if (!rb->isTrigger()) {
        PxFilterData groundPlaneSimFilterData(
          snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE, snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST, 0, 0);
        // Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
        PxFilterData qryFilterData;
        snippetvehicle::setupDrivableSurface(qryFilterData);
        shape->setQueryFilterData(qryFilterData);
        // Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the
        // wheels.
        shape->setSimulationFilterData(groundPlaneSimFilterData);
    }

    rigidbody->userData = (void*)object;
    rb->setRigidbodyRef(static_cast<void*>(rigidbody));
    shape->userData = (void*)object;
    collider->setShapeRef(static_cast<void*>(shape));
    gScene->addActor(*rigidbody);
}

void
PrototypePhysxPhysics::createSphereCollider(PrototypeObject* object)
{
    Transform*       tr               = object->getTransformTrait();
    Rigidbody*       rb               = object->getRigidbodyTrait();
    Collider*        collider         = object->getColliderTrait();
    const glm::vec3& position         = tr->position();
    const glm::vec3& rotation         = tr->rotation();
    const glm::vec3& scale            = tr->scale();
    glm::vec3        linearVelociity  = rb->linearVelocity();
    f32              linearDamping    = rb->linearDamping();
    glm::vec3        angularVelociity = rb->angularVelocity();
    f32              angularDamping   = rb->angularDamping();
    f32              mass             = rb->mass();
    bool             lockLinearX      = rb->lockLinearX();
    bool             lockLinearY      = rb->lockLinearY();
    bool             lockLinearZ      = rb->lockLinearZ();
    bool             lockAngularX     = rb->lockAngularX();
    bool             lockAngularY     = rb->lockAngularY();
    bool             lockAngularZ     = rb->lockAngularZ();

    glm::quat   qat = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
    PxTransform t(position.x, position.y, position.z, PxQuat(qat.x, qat.y, qat.z, qat.w));

    PxRigidActor* rigidbody;
    if (rb->isStatic()) {
        rigidbody = gPhysics->createRigidStatic(t);
    } else {
        rigidbody                        = gPhysics->createRigidDynamic(t);
        PxRigidDynamic* rigidbodyDynamic = ((PxRigidDynamic*)rigidbody);
        rigidbodyDynamic->setLinearVelocity(PxVec3(linearVelociity.x, linearVelociity.y, linearVelociity.z));
        rigidbodyDynamic->setLinearDamping(PxReal(linearDamping));
        rigidbodyDynamic->setAngularVelocity(PxVec3(angularVelociity.x, angularVelociity.y, angularVelociity.z));
        rigidbodyDynamic->setAngularDamping(PxReal(angularDamping));
        rigidbodyDynamic->setMass(PxReal(mass));
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, lockLinearX);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, lockLinearY);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, lockLinearZ);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, lockAngularX);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, lockAngularY);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, lockAngularZ);
    }
    PxShape* shape = PxRigidActorExt::createExclusiveShape(*rigidbody, PxSphereGeometry(PxReal(scale.x)), *gMaterial);
    shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !rb->isTrigger());
    shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, rb->isTrigger());
    if (!rb->isTrigger()) {
        PxFilterData groundPlaneSimFilterData(
          snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE, snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST, 0, 0);
        // Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
        PxFilterData qryFilterData;
        snippetvehicle::setupDrivableSurface(qryFilterData);
        shape->setQueryFilterData(qryFilterData);
        // Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the
        // wheels.
        shape->setSimulationFilterData(groundPlaneSimFilterData);
    }

    rigidbody->userData = (void*)object;
    rb->setRigidbodyRef(static_cast<void*>(rigidbody));
    shape->userData = (void*)object;
    collider->setShapeRef(static_cast<void*>(shape));
    gScene->addActor(*rigidbody);
}

void
PrototypePhysxPhysics::createCapsuleCollider(const float&     radius,
                                             const float&     halfHeight,
                                             const float&     density,
                                             PrototypeObject* object)
{
    Transform*       tr               = object->getTransformTrait();
    Rigidbody*       rb               = object->getRigidbodyTrait();
    Collider*        collider         = object->getColliderTrait();
    const glm::vec3& position         = tr->position();
    const glm::vec3& rotation         = tr->rotation();
    glm::vec3        linearVelociity  = rb->linearVelocity();
    f32              linearDamping    = rb->linearDamping();
    glm::vec3        angularVelociity = rb->angularVelocity();
    f32              angularDamping   = rb->angularDamping();
    f32              mass             = rb->mass();
    bool             lockLinearX      = rb->lockLinearX();
    bool             lockLinearY      = rb->lockLinearY();
    bool             lockLinearZ      = rb->lockLinearZ();
    bool             lockAngularX     = rb->lockAngularX();
    bool             lockAngularY     = rb->lockAngularY();
    bool             lockAngularZ     = rb->lockAngularZ();

    glm::quat   qat = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
    PxTransform t(position.x, position.y, position.z, PxQuat(qat.x, qat.y, qat.z, qat.w));

    PxRigidActor* rigidbody;
    if (rb->isStatic()) {
        rigidbody = gPhysics->createRigidStatic(t);
    } else {
        rigidbody                        = gPhysics->createRigidDynamic(t);
        PxRigidDynamic* rigidbodyDynamic = ((PxRigidDynamic*)rigidbody);
        rigidbodyDynamic->setLinearVelocity(PxVec3(linearVelociity.x, linearVelociity.y, linearVelociity.z));
        rigidbodyDynamic->setLinearDamping(PxReal(linearDamping));
        rigidbodyDynamic->setAngularVelocity(PxVec3(angularVelociity.x, angularVelociity.y, angularVelociity.z));
        rigidbodyDynamic->setAngularDamping(PxReal(angularDamping));
        rigidbodyDynamic->setMass(PxReal(mass));
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, lockLinearX);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, lockLinearY);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, lockLinearZ);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, lockAngularX);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, lockAngularY);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, lockAngularZ);
        PxRigidBodyExt::updateMassAndInertia(*rigidbodyDynamic, PxReal(density));
    }

    PxShape* shape =
      PxRigidActorExt::createExclusiveShape(*rigidbody, PxCapsuleGeometry(PxReal(radius), PxReal(halfHeight)), *gMaterial);
    shape->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
    shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !rb->isTrigger());
    shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, rb->isTrigger());
    if (!rb->isTrigger()) {
        PxFilterData groundPlaneSimFilterData(
          snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE, snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST, 0, 0);
        // Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
        PxFilterData qryFilterData;
        snippetvehicle::setupDrivableSurface(qryFilterData);
        shape->setQueryFilterData(qryFilterData);
        // Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the
        // wheels.
        shape->setSimulationFilterData(groundPlaneSimFilterData);
    }

    rigidbody->userData = (void*)object;
    rb->setRigidbodyRef(static_cast<void*>(rigidbody));
    shape->userData = (void*)object;
    collider->setShapeRef(static_cast<void*>(shape));
    gScene->addActor(*rigidbody);
}

void
PrototypePhysxPhysics::internalCreateConvexMeshCollider(PrototypeObject* object, PxConvexMesh* convexMesh)
{
    Transform*       tr               = object->getTransformTrait();
    Rigidbody*       rb               = object->getRigidbodyTrait();
    Collider*        collider         = object->getColliderTrait();
    const glm::vec3& position         = tr->position();
    const glm::vec3& rotation         = tr->rotation();
    glm::vec3        linearVelociity  = rb->linearVelocity();
    f32              linearDamping    = rb->linearDamping();
    glm::vec3        angularVelociity = rb->angularVelocity();
    f32              angularDamping   = rb->angularDamping();
    f32              mass             = rb->mass();
    bool             lockLinearX      = rb->lockLinearX();
    bool             lockLinearY      = rb->lockLinearY();
    bool             lockLinearZ      = rb->lockLinearZ();
    bool             lockAngularX     = rb->lockAngularX();
    bool             lockAngularY     = rb->lockAngularY();
    bool             lockAngularZ     = rb->lockAngularZ();

    glm::quat   qat = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
    PxTransform t(position.x, position.y, position.z, PxQuat(qat.x, qat.y, qat.z, qat.w));

    PxRigidActor* rigidbody;
    if (rb->isStatic()) {
        rigidbody = gPhysics->createRigidStatic(t);
    } else {
        rigidbody                        = gPhysics->createRigidDynamic(t);
        PxRigidDynamic* rigidbodyDynamic = ((PxRigidDynamic*)rigidbody);
        rigidbodyDynamic->setLinearVelocity(PxVec3(linearVelociity.x, linearVelociity.y, linearVelociity.z));
        rigidbodyDynamic->setLinearDamping(PxReal(linearDamping));
        rigidbodyDynamic->setAngularVelocity(PxVec3(angularVelociity.x, angularVelociity.y, angularVelociity.z));
        rigidbodyDynamic->setAngularDamping(PxReal(angularDamping));
        rigidbodyDynamic->setMass(PxReal(mass));
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, lockLinearX);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, lockLinearY);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, lockLinearZ);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, lockAngularX);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, lockAngularY);
        rigidbodyDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, lockAngularZ);
    }
    PxMeshScale scale(*(PxVec3*)&tr->scale()[0], PxQuat(PxIdentity));
    PxShape*    shape = PxRigidActorExt::createExclusiveShape(*rigidbody, PxConvexMeshGeometry(convexMesh, scale), *gMaterial);
    shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !rb->isTrigger());
    shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, rb->isTrigger());
    if (!rb->isTrigger()) {
        PxFilterData groundPlaneSimFilterData(
          snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE, snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST, 0, 0);
        // Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
        PxFilterData qryFilterData;
        snippetvehicle::setupDrivableSurface(qryFilterData);
        shape->setQueryFilterData(qryFilterData);
        // Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the
        // wheels.
        shape->setSimulationFilterData(groundPlaneSimFilterData);
    }

    rigidbody->userData = (void*)object;
    rb->setRigidbodyRef(static_cast<void*>(rigidbody));
    shape->userData = (void*)object;
    collider->setShapeRef(static_cast<void*>(shape));
    gScene->addActor(*rigidbody);
}

void
PrototypePhysxPhysics::createConvexMeshCollider(const std::vector<glm::vec3>& vertices,
                                                const std::vector<u32>&       indices,
                                                PrototypeObject*              object)
{
    Rigidbody* rb = object->getRigidbodyTrait();

    PxConvexMeshDesc convexDesc;
    convexDesc.points.stride = sizeof(PxVec3);
    convexDesc.points.count  = (u32)vertices.size();
    convexDesc.points.data   = vertices.data();
    convexDesc.flags         = PxConvexFlag::eCOMPUTE_CONVEX;

    PxDefaultMemoryOutputStream     buf;
    PxConvexMeshCookingResult::Enum result;
    if (gCooking->cookConvexMesh(convexDesc, buf, &result)) {
        PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
        PxConvexMesh*            convexMesh = gPhysics->createConvexMesh(input);
        internalCreateConvexMeshCollider(object, convexMesh);
    }
}

void
PrototypePhysxPhysics::createTriMeshCollider(const std::vector<glm::vec3>& vertices,
                                             const std::vector<u32>&       indices,
                                             PrototypeObject*              object)
{
    Transform*       tr               = object->getTransformTrait();
    Rigidbody*       rb               = object->getRigidbodyTrait();
    Collider*        collider         = object->getColliderTrait();
    const glm::vec3& position         = tr->position();
    const glm::vec3& rotation         = tr->rotation();
    glm::vec3        linearVelociity  = rb->linearVelocity();
    f32              linearDamping    = rb->linearDamping();
    glm::vec3        angularVelociity = rb->angularVelocity();
    f32              angularDamping   = rb->angularDamping();
    f32              mass             = rb->mass();
    bool             lockLinearX      = rb->lockLinearX();
    bool             lockLinearY      = rb->lockLinearY();
    bool             lockLinearZ      = rb->lockLinearZ();
    bool             lockAngularX     = rb->lockAngularX();
    bool             lockAngularY     = rb->lockAngularY();
    bool             lockAngularZ     = rb->lockAngularZ();

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.points.count  = (u32)vertices.size();
    meshDesc.points.data   = vertices.data();

    meshDesc.triangles.stride = 3 * sizeof(PxU32);
    meshDesc.triangles.count  = (u32)indices.size();
    meshDesc.triangles.data   = indices.data();

    PxDefaultMemoryOutputStream       buf;
    PxTriangleMeshCookingResult::Enum result;

    if (gCooking->cookTriangleMesh(meshDesc, buf, &result)) {
        PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
        PxTriangleMesh*          triangleMesh = gPhysics->createTriangleMesh(input);

        glm::quat       qat = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
        PxTransform     t(position.x, position.y, position.z, PxQuat(qat.x, qat.y, qat.z, qat.w));
        PxRigidDynamic* rigidbody = gPhysics->createRigidDynamic(t);
        rigidbody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
        PxMeshScale scale(*(PxVec3*)&tr->scale()[0], PxQuat(PxIdentity));
        PxShape*    shape =
          PxRigidActorExt::createExclusiveShape(*rigidbody, PxTriangleMeshGeometry(triangleMesh, scale), *gMaterial);
        shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !rb->isTrigger());
        shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, rb->isTrigger());
        if (!rb->isTrigger()) {
            PxFilterData groundPlaneSimFilterData(
              snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE, snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST, 0, 0);
            // Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
            PxFilterData qryFilterData;
            snippetvehicle::setupDrivableSurface(qryFilterData);
            shape->setQueryFilterData(qryFilterData);
            // Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the
            // wheels.
            shape->setSimulationFilterData(groundPlaneSimFilterData);
        }

        rigidbody->setMass(PxReal(mass));
        rigidbody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, lockLinearX);
        rigidbody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, lockLinearY);
        rigidbody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, lockLinearZ);
        rigidbody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, lockAngularX);
        rigidbody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, lockAngularY);
        rigidbody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, lockAngularZ);
        rigidbody->userData = (void*)object;
        rb->setRigidbodyRef(static_cast<void*>(rigidbody));
        shape->userData = (void*)object;
        collider->setShapeRef(static_cast<void*>(shape));
        gScene->addActor(*rigidbody);
        rb->setStatic(true);
    }
}

void
PrototypePhysxPhysics::createStaticCollider(const std::vector<glm::vec3>& vertices,
                                            const std::vector<u32>&       indices,
                                            PrototypeObject*              object)
{}

void
PrototypePhysxPhysics::createVehicle(const std::vector<glm::vec3>& chasisVertices,
                                     const std::vector<u32>&       chasisIndices,
                                     const std::vector<glm::vec3>& wheelVertices,
                                     const std::vector<u32>&       wheelIndices,
                                     PrototypeObject*              chasisObject,
                                     PrototypeObject*              wheelFRObject,
                                     PrototypeObject*              wheelFLObject,
                                     PrototypeObject*              wheelBRObject,
                                     PrototypeObject*              wheelBLObject)
{
    if (*gNumVehicles == PROTOTYPE_MAX_NUM_VEHICLES) { return; }

    // Create a vehicle that will drive on the plane.
    snippetvehicle::VehicleDesc vehicleDesc = vehicleInitDesc();
    gVehicles[*gNumVehicles]                = createVehicle4W(vehicleDesc, gPhysics, gCooking, chasisObject);

    VehicleChasis* vch = chasisObject->getVehicleChasisTrait();
    vch->setVehicleRef(gVehicles[*gNumVehicles]);
    vch->setWheelFRObject(wheelFRObject);
    vch->setWheelFLObject(wheelFLObject);
    vch->setWheelBRObject(wheelBRObject);
    vch->setWheelBLObject(wheelBLObject);
    vch->setVehicleIndex(*gNumVehicles);

    PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 0),
                               PxQuat(PxIdentity));
    gVehicles[*gNumVehicles]->getRigidDynamicActor()->setGlobalPose(startTransform);
    gScene->addActor(*gVehicles[*gNumVehicles]->getRigidDynamicActor());

    // Set the vehicle to rest in first gear.
    // Set the vehicle to use auto-gears.
    // Set the vehicle to use the standard control model
    gVehicles[*gNumVehicles]->setToRestState();
    gVehicles[*gNumVehicles]->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
    gVehicles[*gNumVehicles]->mDriveDynData.setUseAutoGears(true);

    gVehicleInputData[*gNumVehicles].setAnalogBrake(1.0f);

    ++(*gNumVehicles);
}

void
PrototypePhysxPhysics::updateVehicleController(PrototypeObject* object, f32 acceleration, f32 brake, f32 steer)
{
    VehicleChasis* vch          = object->getVehicleChasisTrait();
    size_t         vehicleIndex = vch->vehicleIndex();
    gVehicleInputData[vehicleIndex].setAnalogAccel(acceleration);
    gVehicleInputData[vehicleIndex].setAnalogBrake(brake);
    gVehicleInputData[vehicleIndex].setAnalogSteer(steer);
}

void
PrototypePhysxPhysics::updateRigidbodyStatic(PrototypeObject* object)
{
    Rigidbody*         rb        = object->getRigidbodyTrait();
    Collider*          collider  = object->getColliderTrait();
    auto               actor     = static_cast<PxRigidActor*>(rb->rigidbodyRef());
    PxShape*           shape     = static_cast<PxShape*>(collider->shapeRef());
    const std::string& shapeName = collider->nameRef();
    if (shapeName == "PLANE") {
        if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
        createPlaneCollider(object);
        collider->setNameRef("PLANE");
    } else if (shapeName == "CUBE") {
        if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
        createBoxCollider(object);
        collider->setNameRef("CUBE");
    } else if (shapeName == "SPHERE") {
        if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
        createSphereCollider(object);
        collider->setNameRef("SPHERE");
    } else if (shapeName.rfind("(CONVEX) ", 0) == 0) {
        Rigidbody*       rb = object->getRigidbodyTrait();
        PxConvexMeshDesc convexDesc;
        convexDesc.points.stride = sizeof(PxVec3);
        convexDesc.points.count  = shape->getGeometry().convexMesh().convexMesh->getNbVertices();
        convexDesc.points.data   = shape->getGeometry().convexMesh().convexMesh->getVertices();
        convexDesc.flags         = PxConvexFlag::eCOMPUTE_CONVEX;
        PxDefaultMemoryOutputStream     buf;
        PxConvexMeshCookingResult::Enum result;
        if (gCooking->cookConvexMesh(convexDesc, buf, &result)) {
            PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
            PxConvexMesh*            convexMesh = gPhysics->createConvexMesh(input);
            if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
            internalCreateConvexMeshCollider(object, convexMesh);
            collider->setNameRef(shapeName);
        }
    }
}

void
PrototypePhysxPhysics::updateRigidbodyTrigger(PrototypeObject* object)
{
    Rigidbody*         rb        = object->getRigidbodyTrait();
    Collider*          collider  = object->getColliderTrait();
    auto               actor     = static_cast<PxRigidActor*>(rb->rigidbodyRef());
    PxShape*           shape     = static_cast<PxShape*>(collider->shapeRef());
    const std::string& shapeName = collider->nameRef();
    if (shapeName == "PLANE") {
        if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
        createPlaneCollider(object);
        collider->setNameRef("PLANE");
    } else if (shapeName == "CUBE") {
        if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
        createBoxCollider(object);
        collider->setNameRef("CUBE");
    } else if (shapeName == "SPHERE") {
        if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
        createSphereCollider(object);
        collider->setNameRef("SPHERE");
    } else if (shapeName.rfind("(CONVEX) ", 0) == 0) {
        Rigidbody*       rb = object->getRigidbodyTrait();
        PxConvexMeshDesc convexDesc;
        convexDesc.points.stride = sizeof(PxVec3);
        convexDesc.points.count  = shape->getGeometry().convexMesh().convexMesh->getNbVertices();
        convexDesc.points.data   = shape->getGeometry().convexMesh().convexMesh->getVertices();
        convexDesc.flags         = PxConvexFlag::eCOMPUTE_CONVEX;
        PxDefaultMemoryOutputStream     buf;
        PxConvexMeshCookingResult::Enum result;
        if (gCooking->cookConvexMesh(convexDesc, buf, &result)) {
            PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
            PxConvexMesh*            convexMesh = gPhysics->createConvexMesh(input);
            if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
            internalCreateConvexMeshCollider(object, convexMesh);
            collider->setNameRef(shapeName);
        }
    }
}

void
PrototypePhysxPhysics::updateRigidbodyLinearVelocity(PrototypeObject* object)
{
    Rigidbody* rb    = object->getRigidbodyTrait();
    auto       actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
    if (actor->is<PxRigidDynamic>()) {
        actor->is<PxRigidDynamic>()->setLinearVelocity(
          PxVec3(rb->linearVelocity().x, rb->linearVelocity().y, rb->linearVelocity().z));
    }
}

void
PrototypePhysxPhysics::updateRigidbodyLinearDamping(PrototypeObject* object)
{
    Rigidbody* rb    = object->getRigidbodyTrait();
    auto       actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
    if (actor->is<PxRigidDynamic>()) { actor->is<PxRigidDynamic>()->setLinearDamping(rb->linearDamping()); }
}

void
PrototypePhysxPhysics::updateRigidbodyAngularVelocity(PrototypeObject* object)
{
    Rigidbody* rb    = object->getRigidbodyTrait();
    auto       actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
    if (actor->is<PxRigidDynamic>()) {
        actor->is<PxRigidDynamic>()->setAngularVelocity(
          PxVec3(rb->angularVelocity().x, rb->angularVelocity().y, rb->angularVelocity().z));
    }
}

void
PrototypePhysxPhysics::updateRigidbodyAngularDamping(PrototypeObject* object)
{
    Rigidbody* rb    = object->getRigidbodyTrait();
    auto       actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
    if (actor->is<PxRigidDynamic>()) { actor->is<PxRigidDynamic>()->setAngularDamping(rb->angularDamping()); }
}

void
PrototypePhysxPhysics::updateRigidbodyMass(PrototypeObject* object)
{
    Rigidbody* rb    = object->getRigidbodyTrait();
    auto       actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
    if (actor->is<PxRigidDynamic>()) { actor->is<PxRigidDynamic>()->setMass(rb->mass()); }
}

void
PrototypePhysxPhysics::updateRigidbodyLockLinear(PrototypeObject* object)
{
    Rigidbody* rb    = object->getRigidbodyTrait();
    auto       actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
    if (actor->is<PxRigidDynamic>()) {
        actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, rb->lockLinearX());
        actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, rb->lockLinearY());
        actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, rb->lockLinearZ());
    }
}

void
PrototypePhysxPhysics::updateRigidbodyLockAngular(PrototypeObject* object)
{
    Rigidbody* rb    = object->getRigidbodyTrait();
    auto       actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
    if (actor->is<PxRigidDynamic>()) {
        actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rb->lockAngularX());
        actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rb->lockAngularY());
        actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rb->lockAngularZ());
    }
}

void
PrototypePhysxPhysics::updateCollider(PrototypeObject* object, const std::string& shapeName)
{
    Collider*  collider = object->getColliderTrait();
    Rigidbody* rb       = object->getRigidbodyTrait();
    if (!rb) return;
    auto actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
    if (shapeName == "PLANE") {
        if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
        createPlaneCollider(object);
        collider->setNameRef("PLANE");
    } else if (shapeName == "CUBE") {
        if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
        createBoxCollider(object);
        collider->setNameRef("CUBE");
    } else if (shapeName == "SPHERE") {
        if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
        createSphereCollider(object);
        collider->setNameRef("SPHERE");
    } else if (shapeName.rfind("(CONVEX) ", 0) == 0) {
        std::string meshBufferName = shapeName.substr(strlen("(CONVEX) "));
        auto        it             = PrototypeEngineInternalApplication::database->meshBuffers.find(meshBufferName);
        if (it != PrototypeEngineInternalApplication::database->meshBuffers.end()) {
            const PrototypeMeshBufferSource& source = it->second->source();
            if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
            std::vector<glm::vec3> _verts(source.vertices.size());
            for (size_t v = 0; v < source.vertices.size(); ++v) {
                _verts[v].x = source.vertices[v].positionU.x;
                _verts[v].y = source.vertices[v].positionU.y;
                _verts[v].z = source.vertices[v].positionU.z;
            }
            createConvexMeshCollider(_verts, source.indices, object);
            collider->setNameRef(shapeName);
        }
    } else if (shapeName.rfind("(TRIMESH) ", 0) == 0) {
        std::string meshBufferName = shapeName.substr(strlen("(TRIMESH) "));
        auto        it             = PrototypeEngineInternalApplication::database->meshBuffers.find(meshBufferName);
        if (it != PrototypeEngineInternalApplication::database->meshBuffers.end()) {
            const PrototypeMeshBufferSource& source = it->second->source();
            if (collider->shapeRef()) { actor->detachShape(*static_cast<PxShape*>(collider->shapeRef())); }
            std::vector<glm::vec3> _verts(source.vertices.size());
            for (size_t v = 0; v < source.vertices.size(); ++v) {
                _verts[v].x = source.vertices[v].positionU.x;
                _verts[v].y = source.vertices[v].positionU.y;
                _verts[v].z = source.vertices[v].positionU.z;
            }
            createTriMeshCollider(_verts, source.indices, object);
            collider->setNameRef(shapeName);
        }
    }
}

void
PrototypePhysxPhysics::scaleCollider(PrototypeObject* object, const glm::vec3& scale)
{
    if (object->hasColliderTrait()) {
        Transform* tr       = object->getTransformTrait();
        Collider*  collider = object->getColliderTrait();
        Rigidbody* rb       = object->getRigidbodyTrait();
        auto       shape    = static_cast<PxShape*>(collider->shapeRef());
        if (collider->shapeType() == ColliderShape_TriangleMesh) {
            PxTriangleMeshGeometry triangleMesh;
            shape->getTriangleMeshGeometry(triangleMesh);
            auto actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
            if (actor) {
                if (actor->getNbShapes() > 0) {
                    std::vector<PxShape*> shapes(actor->getNbShapes());
                    actor->getShapes(shapes.data(), actor->getNbShapes());
                    for (size_t i = 0; i < actor->getNbShapes(); ++i) { actor->detachShape(*shapes[i]); }
                }
                PxMeshScale meshScale(PxVec3(scale.x, scale.y, scale.z), PxQuat(PxIdentity));
                PxShape*    myTriMeshShape = PxRigidActorExt::createExclusiveShape(
                  *actor, PxTriangleMeshGeometry(triangleMesh.triangleMesh, meshScale), *gMaterial);

                // --------------------------------------------------------------------------------------------------
                // setup shape's filters
                // --------------------------------------------------------------------------------------------------
                PxFilterData groundPlaneSimFilterData(snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE,
                                                      snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST,
                                                      0,
                                                      0);
                // Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
                PxFilterData qryFilterData;
                snippetvehicle::setupDrivableSurface(qryFilterData);
                myTriMeshShape->setQueryFilterData(qryFilterData);
                // Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not
                // the wheels.
                myTriMeshShape->setSimulationFilterData(groundPlaneSimFilterData);
                // --------------------------------------------------------------------------------------------------

                collider->setShapeRef((void*)myTriMeshShape);
            }
        } else if (collider->shapeType() == ColliderShape_ConvexMesh) {
            PxConvexMeshGeometry convexMesh;
            shape->getConvexMeshGeometry(convexMesh);
            auto actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
            if (actor) {
                if (actor->getNbShapes() > 0) {
                    std::vector<PxShape*> shapes(actor->getNbShapes());
                    actor->getShapes(shapes.data(), actor->getNbShapes());
                    for (size_t i = 0; i < actor->getNbShapes(); ++i) { actor->detachShape(*shapes[i]); }
                }
                PxMeshScale meshScale(PxVec3(scale.x, scale.y, scale.z), PxQuat(PxIdentity));
                PxShape*    myConvexMeshShape = PxRigidActorExt::createExclusiveShape(
                  *actor, PxConvexMeshGeometry(convexMesh.convexMesh, meshScale), *gMaterial);

                // --------------------------------------------------------------------------------------------------
                // setup shape's filters
                // --------------------------------------------------------------------------------------------------
                PxFilterData groundPlaneSimFilterData(snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE,
                                                      snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST,
                                                      0,
                                                      0);
                // Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
                PxFilterData qryFilterData;
                snippetvehicle::setupDrivableSurface(qryFilterData);
                myConvexMeshShape->setQueryFilterData(qryFilterData);
                // Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not
                // the wheels.
                myConvexMeshShape->setSimulationFilterData(groundPlaneSimFilterData);
                // --------------------------------------------------------------------------------------------------

                collider->setShapeRef((void*)myConvexMeshShape);
            }
        } else if (collider->shapeType() == ColliderShape_Box) {
            PxBoxGeometry boxMesh;
            shape->getBoxGeometry(boxMesh);
            auto actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
            if (actor) {
                if (actor->getNbShapes() > 0) {
                    std::vector<PxShape*> shapes(actor->getNbShapes());
                    actor->getShapes(shapes.data(), actor->getNbShapes());
                    for (size_t i = 0; i < actor->getNbShapes(); ++i) { actor->detachShape(*shapes[i]); }
                }
                PxShape* myBoxMeshShape =
                  PxRigidActorExt::createExclusiveShape(*actor, PxBoxGeometry(PxVec3(scale.x, scale.y, scale.z)), *gMaterial);

                // --------------------------------------------------------------------------------------------------
                // setup shape's filters
                // --------------------------------------------------------------------------------------------------
                PxFilterData groundPlaneSimFilterData(snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE,
                                                      snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST,
                                                      0,
                                                      0);
                // Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
                PxFilterData qryFilterData;
                snippetvehicle::setupDrivableSurface(qryFilterData);
                myBoxMeshShape->setQueryFilterData(qryFilterData);
                // Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not
                // the wheels.
                myBoxMeshShape->setSimulationFilterData(groundPlaneSimFilterData);
                // --------------------------------------------------------------------------------------------------

                collider->setShapeRef((void*)myBoxMeshShape);
            }
        } else if (collider->shapeType() == ColliderShape_Sphere) {
            PxSphereGeometry sphereMesh;
            shape->getSphereGeometry(sphereMesh);
            auto actor = static_cast<PxRigidActor*>(rb->rigidbodyRef());
            if (actor) {
                if (actor->getNbShapes() > 0) {
                    std::vector<PxShape*> shapes(actor->getNbShapes());
                    actor->getShapes(shapes.data(), actor->getNbShapes());
                    for (size_t i = 0; i < actor->getNbShapes(); ++i) { actor->detachShape(*shapes[i]); }
                }
                PxShape* mySphereMeshShape =
                  PxRigidActorExt::createExclusiveShape(*actor, PxSphereGeometry(PxReal(scale.x)), *gMaterial);

                // --------------------------------------------------------------------------------------------------
                // setup shape's filters
                // --------------------------------------------------------------------------------------------------
                PxFilterData groundPlaneSimFilterData(snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE,
                                                      snippetvehicle::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST,
                                                      0,
                                                      0);
                // Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
                PxFilterData qryFilterData;
                snippetvehicle::setupDrivableSurface(qryFilterData);
                mySphereMeshShape->setQueryFilterData(qryFilterData);
                // Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not
                // the wheels.
                mySphereMeshShape->setSimulationFilterData(groundPlaneSimFilterData);
                // --------------------------------------------------------------------------------------------------

                collider->setShapeRef((void*)mySphereMeshShape);
            }
        }
    }
}

void
PrototypePhysxPhysics::createRigidbody(PrototypeObject* object)
{
    Collider*  collider  = object->getColliderTrait();
    Rigidbody* rigidbody = object->getRigidbodyTrait();
    if (rigidbody->isStatic()) {
        if (object->hasMeshRendererTrait()) {
            MeshRenderer*          mr       = object->getMeshRendererTrait();
            const std::string&     meshName = mr->data()[0].mesh;
            auto                   source   = PrototypeEngineInternalApplication::database->meshBuffers[meshName]->source();
            std::vector<glm::vec3> vertices(source.vertices.size());
            for (size_t v = 0; v < source.vertices.size(); ++v) {
                vertices[v].x = source.vertices[v].positionU.x;
                vertices[v].y = source.vertices[v].positionU.y;
                vertices[v].z = source.vertices[v].positionU.z;
            }
            PrototypeEngineInternalApplication::physics->createTriMeshCollider(vertices, source.indices, &(*object));
            collider->setNameRef(std::string("(TRIMESH) ").append(meshName));
        } else {
            PrototypeEngineInternalApplication::physics->createBoxCollider(&(*object));
            collider->setNameRef("CUBE");
        }
    } else {
        switch (collider->shapeType()) {
            case ColliderShape_Plane: {
                PrototypeEngineInternalApplication::physics->createPlaneCollider(&(*object));
                collider->setNameRef("PLANE");
            } break;

            case ColliderShape_Box: {
                PrototypeEngineInternalApplication::physics->createBoxCollider(&(*object));
                collider->setNameRef("CUBE");
            } break;

            case ColliderShape_Sphere: {
                PrototypeEngineInternalApplication::physics->createSphereCollider(&(*object));
            } break;

            case ColliderShape_Capsule: {
                PrototypeEngineInternalApplication::physics->createCapsuleCollider(
                  collider->radius(), collider->height(), collider->density(), &(*object));
                collider->setNameRef("CAPSULE");
            } break;

            case ColliderShape_ConvexMesh: {
                if (object->hasMeshRendererTrait()) {
                    const std::string      meshName = object->getMeshRendererTrait()->data()[0].mesh;
                    auto                   source = PrototypeEngineInternalApplication::database->meshBuffers[meshName]->source();
                    std::vector<glm::vec3> _verts(source.vertices.size());
                    for (size_t v = 0; v < source.vertices.size(); ++v) {
                        _verts[v].x = source.vertices[v].positionU.x;
                        _verts[v].y = source.vertices[v].positionU.y;
                        _verts[v].z = source.vertices[v].positionU.z;
                    }
                    PrototypeEngineInternalApplication::physics->createConvexMeshCollider(_verts, source.indices, &(*object));
                    collider->setNameRef(std::string("(CONVEX) ").append(meshName));
                } else {
                    PrototypeEngineInternalApplication::physics->createBoxCollider(&(*object));
                    collider->setNameRef("CUBE");
                }
            } break;

            case ColliderShape_TriangleMesh: {
                if (object->hasMeshRendererTrait()) {
                    const std::string      meshName = object->getMeshRendererTrait()->data()[0].mesh;
                    auto                   source = PrototypeEngineInternalApplication::database->meshBuffers[meshName]->source();
                    std::vector<glm::vec3> _verts(source.vertices.size());
                    for (size_t v = 0; v < source.vertices.size(); ++v) {
                        _verts[v].x = source.vertices[v].positionU.x;
                        _verts[v].y = source.vertices[v].positionU.y;
                        _verts[v].z = source.vertices[v].positionU.z;
                    }
                    PrototypeEngineInternalApplication::physics->createTriMeshCollider(_verts, source.indices, &(*object));
                    collider->setNameRef(std::string("(TRIMESH) ").append(meshName));
                } else {
                    PrototypeEngineInternalApplication::physics->createBoxCollider(&(*object));
                    collider->setNameRef("CUBE");
                }
            } break;

            default: {
                PrototypeLogger::fatal("Unhandled collider type");
            } break;
        }
    }
}

void
PrototypePhysxPhysics::destroyRigidbody(void* rigidbody)
{
    if (!rigidbody) return;
    auto actor = static_cast<PxRigidActor*>(rigidbody);
    if (actor) {
        gScene->removeActor(*actor);
        auto object = static_cast<PrototypeObject*>(actor->userData);
        if (object && object->hasColliderTrait()) {
            Collider* collider = object->getColliderTrait();
            auto      shape    = static_cast<PxShape*>(collider->shapeRef());
            if (!shape->isExclusive()) { actor->detachShape(*shape); }
        }
        if (actor->isReleasable()) { actor->release(); }
    }
}

void
PrototypePhysxPhysics::spawnVehicle()
{
    if (*gNumVehicles > 0) {
        if (*gControlledVehicleIndex >= 0 && *gControlledVehicleIndex < *gNumVehicles) {
            gVehicles[*gControlledVehicleIndex]->setToRestState();
            gVehicleInputData[*gControlledVehicleIndex].setAnalogBrake(1.0f);
        }
    }
    *gControlledVehicleIndex = *gNumVehicles - 1;
    vehicleReleaseAllControls(*gControlledVehicleIndex);
    gVehicles[*gControlledVehicleIndex]->setToRestState();
    gVehicles[*gControlledVehicleIndex]->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
    gVehicles[*gControlledVehicleIndex]->mDriveDynData.setUseAutoGears(true);
    gVehicleInputData[*gControlledVehicleIndex].setAnalogBrake(1.0f);
    PxTransform t = gVehicles[*gControlledVehicleIndex]->getRigidDynamicActor()->getGlobalPose();
    PxTransform startTransform(PxVec3(t.p.x, 5.0f, t.p.z), PxQuat(PxIdentity));
    gVehicles[*gControlledVehicleIndex]->getRigidDynamicActor()->setGlobalPose(startTransform);
}

void
PrototypePhysxPhysics::toggleVehicleAccessControl(PrototypeObject* object)
{}

void
PrototypePhysxPhysics::requestNextVehicleAccessControl()
{
    if (*gNumVehicles <= 0) {
        *gControlledVehicleIndex = -1;
    } else {
        gVehicles[*gControlledVehicleIndex]->setToRestState();
        gVehicleInputData[*gControlledVehicleIndex].setAnalogBrake(1.0f);
        --(*gControlledVehicleIndex);
        *gControlledVehicleIndex %= *gNumVehicles;
        vehicleReleaseAllControls(*gControlledVehicleIndex);
        gVehicles[*gControlledVehicleIndex]->setToRestState();
        gVehicles[*gControlledVehicleIndex]->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
        gVehicles[*gControlledVehicleIndex]->mDriveDynData.setUseAutoGears(true);
        gVehicleInputData[*gControlledVehicleIndex].setAnalogBrake(1.0f);
        PxTransform startTransform(PxVec3(0, 5.0f, 0), PxQuat(PxIdentity));
        gVehicles[*gControlledVehicleIndex]->getRigidDynamicActor()->setGlobalPose(startTransform);
    }
}

void
PrototypePhysxPhysics::requestPreviousVehicleAccessControl()
{
    if (*gNumVehicles <= 0) {
        *gControlledVehicleIndex = -1;
    } else {
        gVehicles[*gControlledVehicleIndex]->setToRestState();
        gVehicleInputData[*gControlledVehicleIndex].setAnalogBrake(1.0f);
        ++(*gControlledVehicleIndex);
        *gControlledVehicleIndex %= *gNumVehicles;
        vehicleReleaseAllControls(*gControlledVehicleIndex);
        gVehicles[*gControlledVehicleIndex]->setToRestState();
        gVehicles[*gControlledVehicleIndex]->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
        gVehicles[*gControlledVehicleIndex]->mDriveDynData.setUseAutoGears(true);
        gVehicleInputData[*gControlledVehicleIndex].setAnalogBrake(1.0f);
        PxTransform t = gVehicles[*gControlledVehicleIndex]->getRigidDynamicActor()->getGlobalPose();
        PxTransform startTransform(PxVec3(t.p.x, 5.0f, t.p.z), PxQuat(PxIdentity));
        gVehicles[*gControlledVehicleIndex]->getRigidDynamicActor()->setGlobalPose(startTransform);
    }
}

void
PrototypePhysxPhysics::controlledVehiclesSetGear(PrototypePhysicsVehicleGear gear)
{}

void
PrototypePhysxPhysics::controlledVehiclesToggleGearDirection()
{
    if (gVehicles[*gControlledVehicleIndex]->mDriveDynData.getCurrentGear() == PxVehicleGearsData::eREVERSE) {
        gVehicles[*gControlledVehicleIndex]->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
    } else {
        gVehicles[*gControlledVehicleIndex]->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
    }
}

void
PrototypePhysxPhysics::controlledVehiclesFlip()
{
    if (*gNumVehicles > 0 && *gControlledVehicleIndex < *gNumVehicles && *gControlledVehicleIndex >= 0) {
        vehicleReleaseAllControls(*gControlledVehicleIndex);
        gVehicles[*gControlledVehicleIndex]->setToRestState();
        gVehicles[*gControlledVehicleIndex]->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
        gVehicles[*gControlledVehicleIndex]->mDriveDynData.setUseAutoGears(true);
        gVehicleInputData[*gControlledVehicleIndex].setAnalogBrake(1.0f);
        PxTransform t = gVehicles[*gControlledVehicleIndex]->getRigidDynamicActor()->getGlobalPose();
        PxTransform startTransform(PxVec3(t.p.x, t.p.y + 1.0f, t.p.z), PxQuat(PxIdentity));
        gVehicles[*gControlledVehicleIndex]->getRigidDynamicActor()->setGlobalPose(startTransform);
    }
}

void
PrototypePhysxPhysics::onMouse(i32 button, i32 action, i32 mods)
{}

void
PrototypePhysxPhysics::onMouseMove(f64 x, f64 y)
{}

void
PrototypePhysxPhysics::onMouseDrag(i32 button, f64 x, f64 y)
{}

void
PrototypePhysxPhysics::onMouseScroll(f64 x, f64 y)
{}

void
PrototypePhysxPhysics::onKeyboard(i32 key, i32 scancode, i32 action, i32 mods)
{}

void
PrototypePhysxPhysics::onWindowResize(i32 width, i32 height)
{}

void
PrototypePhysxPhysics::onWindowDragDrop(i32 numFiles, const char** names)
{}

PrototypePhysxEventsCallback::PrototypePhysxEventsCallback() {}

PrototypePhysxEventsCallback::~PrototypePhysxEventsCallback() {}

void
PrototypePhysxEventsCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{}

void
PrototypePhysxEventsCallback::onWake(PxActor** actors, PxU32 count)
{
    for (PxU32 i = 0; i < count; ++i) {
        const char* name = ((PrototypeSceneNode*)((PrototypeObject*)actors[i]->userData)->parentNode())->name().c_str();
        char        buf[256];
        snprintf(buf, sizeof(buf), "OnWake(%s)", name);
        PrototypeLogger::log(__FILE__, __LINE__, buf);
    }
}

void
PrototypePhysxEventsCallback::onSleep(PxActor** actors, PxU32 count)
{
    for (PxU32 i = 0; i < count; ++i) {
        const char* name = ((PrototypeSceneNode*)((PrototypeObject*)actors[i]->userData)->parentNode())->name().c_str();
        char        buf[256];
        snprintf(buf, sizeof(buf), "OnSleep(%s)", name);
        PrototypeLogger::log(__FILE__, __LINE__, buf);
    }
}

void
PrototypePhysxEventsCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{}

void
PrototypePhysxEventsCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
    for (PxU32 i = 0; i < count; i++) {
        // ignore pairs when shapes have been deleted
        if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER)) continue;

        pairs[i].status;
        PxRigidActor* otherActor   = pairs[i].otherShape->getActor();
        PxRigidActor* triggerActor = pairs[i].triggerShape->getActor();

        if (otherActor->userData && triggerActor->userData) {
            auto otherObject   = (PrototypeObject*)otherActor->userData;
            auto triggerObject = (PrototypeObject*)triggerActor->userData;
            {
                // loop on each attached script and call on trigger
                // oa->getScriptTrait()->codeLinks
                char buf[512];
                snprintf(buf,
                         sizeof(buf),
                         "%s is triggered (%s) by %s",
                         ((PrototypeSceneNode*)triggerObject->parentNode())->name().c_str(),
                         pairs[i].status == PxPairFlag::Enum::eNOTIFY_TOUCH_LOST ? "lost" : "found",
                         ((PrototypeSceneNode*)otherObject->parentNode())->name().c_str());
                PrototypeLogger::log(__FILE__, __LINE__, buf);
            }
        }
    }
}

void
PrototypePhysxEventsCallback::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{}