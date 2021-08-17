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

#include "PrototypeStaticInitializer.h"
#include "PrototypeEngine.h"
#include "PrototypeMeshBuffer.h"

u32 PrototypeStaticInitializer::_meshBufferUUID    = 0;
u32 PrototypeStaticInitializer::_shaderBufferUUID  = 0;
u32 PrototypeStaticInitializer::_textureBufferUUID = 0;
u32 PrototypeStaticInitializer::_materialUUID      = 0;
u32 PrototypeStaticInitializer::_sceneUUID         = 0;
u32 PrototypeStaticInitializer::_sceneLayerUUID    = 0;
u32 PrototypeStaticInitializer::_sceneNodeUUID     = 0;
u32 PrototypeStaticInitializer::_framebufferUUID   = 0;

void
PrototypeStaticInitializer::reset()
{
    PrototypeStaticInitializer::_meshBufferUUID    = 0;
    PrototypeStaticInitializer::_shaderBufferUUID  = 0;
    PrototypeStaticInitializer::_textureBufferUUID = 0;
    PrototypeStaticInitializer::_materialUUID      = 0;
    PrototypeStaticInitializer::_sceneUUID         = 0;
    PrototypeStaticInitializer::_sceneLayerUUID    = 0;
    PrototypeStaticInitializer::_sceneNodeUUID     = 0;
    PrototypeStaticInitializer::_framebufferUUID   = 0;

    PrototypeEngineInternalApplication::window   = nullptr;
    PrototypeEngineInternalApplication::renderer = nullptr;
    PrototypeEngineInternalApplication::physics  = nullptr;
    PrototypeEngineInternalApplication::scene    = nullptr;
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PrototypeEngineInternalApplication::profiler = nullptr;
#endif
}
