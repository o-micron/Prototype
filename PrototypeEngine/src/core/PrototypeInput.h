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

#include <stdint.h>
#include <unordered_map>

#include <GLFW/glfw3.h>

struct PrototypeInput
{
    PrototypeInput();
    ~PrototypeInput();

    void update();
    void releaseAllKeyboardKeys();
    void onKeyboard(i32 key, i32 scancode, i32 action, i32 mods);
    void subscribeKeyForStreams(int32_t key);
    void unsubscribeKeyForStreams(int32_t key);
    f32  fetchKeyNormalizedValue(int32_t key) const;
    bool fetchKeyPressed(int32_t key) const;

  private:
    std::unordered_map<int32_t, std::pair<bool, f32>> _keyValues;
};
