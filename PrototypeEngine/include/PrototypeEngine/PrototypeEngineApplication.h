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

#include "PrototypeEngineApi.h"

#include <string>

struct PrototypeWindow;

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* OnStartFn)();
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* OnUpdateFn)();
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* OnRender3DFn)();
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* OnRender2DFn)();
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* OnEndFn)();

PROTOTYPE_EXTERN struct PROTOTYPE_ENGINE_API PrototypeEngineApplication
{
    std::string  name;
    u8           versionMajor;
    u8           versionMinor;
    OnStartFn    onStartFn;
    OnUpdateFn   onUpdateFn;
    OnRender3DFn onRender3DFn;
    OnRender2DFn onRender2DFn;
    OnEndFn      onEndFn;
};

PROTOTYPE_EXTERN PROTOTYPE_ENGINE_API bool
PrototypeEngineInit(const PrototypeEngineApplication& application);

PROTOTYPE_EXTERN PROTOTYPE_ENGINE_API void
PrototypeEngineLoop();

PROTOTYPE_EXTERN PROTOTYPE_ENGINE_API void
PrototypeEngineDeInit();