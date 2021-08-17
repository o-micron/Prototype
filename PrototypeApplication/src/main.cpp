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

#include <PrototypeCommon/Definitions.h>

#if defined(PROTOTYPE_PLATFORM_WINDOWS)
#ifndef PROTOTYPE_TARGET_RELEASE_BUILD
#define _CRTDBG_MAP_ALLOC
#endif
#endif

#include "Application.h"

#include <PrototypeCommon/Logger.h>
#include <PrototypeEngine/PrototypeEngineApplication.h>

#include <thread>

int
main(int argc, char const* argv[])
{
    {
        PrototypeEngineApplication application;
        application.name         = "PrototypeApplication";
        application.versionMajor = 1;
        application.versionMinor = 0;
        application.onStartFn    = nullptr;
        application.onRender3DFn = nullptr;
        application.onRender2DFn = nullptr;
        application.onUpdateFn   = nullptr;
        application.onEndFn      = nullptr;

        if (PrototypeEngineInit(application)) {
            PrototypeEngineLoop();
            PrototypeEngineDeInit();
        }
    }
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
#ifndef PROTOTYPE_TARGET_RELEASE_BUILD
    _CrtDumpMemoryLeaks();
#endif
#endif
    return 0;
}
