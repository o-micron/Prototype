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

#include "../../include/PrototypeEngine/PrototypeEngineApplication.h"

struct PrototypeDatabase;
struct PrototypeRenderer;
struct PrototypePhysics;
struct PrototypeScene;
struct PrototypeProfiler;
struct PrototypeLogger;

enum PROTOTYPE_ENGINE_API PrototypeEngineERenderingApi_
{
    PrototypeEngineERenderingApi_OPENGL4_1 = 0,
    PrototypeEngineERenderingApi_OPENGLES_3_0,
    PrototypeEngineERenderingApi_VULKAN_1,

    PrototypeEngineERenderingApi_COUNT
};

PROTOTYPE_FOR_EACH_X(PROTOTYPE_STRINGIFY_ENUM_EXTENDED,
                     PROTOTYPE_STRINGIFY_ENUM,
                     PrototypeEngineERenderingApi_,
                     PrototypeEngineERenderingApi_OPENGL4_1,
                     PrototypeEngineERenderingApi_OPENGLES_3_0,
                     PrototypeEngineERenderingApi_VULKAN_1);

enum PROTOTYPE_ENGINE_API PrototypeEngineEPhysicsApi_
{
    PrototypeEngineEPhysicsApi_PHYSX = 0,
    PrototypeEngineEPhysicsApi_BULLET,

    PrototypeEngineEPhysicsApi_COUNT
};

PROTOTYPE_FOR_EACH_X(PROTOTYPE_STRINGIFY_ENUM_EXTENDED,
                     PROTOTYPE_STRINGIFY_ENUM,
                     PrototypeEngineEPhysicsApi_,
                     PrototypeEngineEPhysicsApi_PHYSX,
                     PrototypeEngineEPhysicsApi_BULLET);

struct PrototypeEngineInternalApplication
{
    PrototypeEngineInternalApplication()  = delete;
    ~PrototypeEngineInternalApplication() = delete;

    static PrototypeEngineApplication    application;
    static PrototypeEngineERenderingApi_ renderingApi;
    static PrototypeEngineEPhysicsApi_   physicsApi;
    static bool                          shouldQuit;
    static PrototypeDatabase*            database;
    static PrototypeWindow*              window;
    static PrototypeRenderer*            renderer;
    static PrototypePhysics*             physics;
    static PrototypeScene*               scene;
#if defined(PROTOTYPE_ENABLE_PROFILER)
    static PrototypeProfiler* profiler;
#endif
    static void** traitSystemData;
};

struct PrototypeEngineContext
{
    PrototypeEngineApplication    application;
    PrototypeEngineERenderingApi_ renderingApi;
    PrototypeEngineEPhysicsApi_   physicsApi;
    bool                          shouldQuit;
    PrototypeDatabase*            database;
    PrototypeWindow*              window;
    PrototypeRenderer*            renderer;
    PrototypePhysics*             physics;
    PrototypeScene*               scene;
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PrototypeProfiler* profiler;
#endif
    void** traitSystemData;
};