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
#include <PrototypeCommon/Types.h>

#if defined(PROTOTYPE_PLATFORM_WEBKIT) && defined(PROTOTYPE_ENGINE_EXPORT_DYNAMIC)
#define PROTOTYPE_ENGINE_API __attribute__((visibility("default")))
#elif defined(PROTOTYPE_PLATFORM_WINDOWS) && defined(PROTOTYPE_ENGINE_EXPORT_DYNAMIC)
#define PROTOTYPE_ENGINE_API __declspec(dllexport)
#elif defined(PROTOTYPE_PLATFORM_WINDOWS) && defined(PROTOTYPE_ENGINE_IMPORT_DYNAMIC)
#define PROTOTYPE_ENGINE_API __declspec(dllimport)
#elif defined(PROTOTYPE_PLATFORM_DARWIN) && defined(PROTOTYPE_ENGINE_EXPORT_DYNAMIC)
#define PROTOTYPE_ENGINE_API __attribute__((visibility("default")))
#elif defined(PROTOTYPE_PLATFORM_LINUX) && defined(PROTOTYPE_ENGINE_EXPORT_DYNAMIC)
#define PROTOTYPE_ENGINE_API __attribute__((visibility("default")))
#else
#define PROTOTYPE_ENGINE_API
#endif