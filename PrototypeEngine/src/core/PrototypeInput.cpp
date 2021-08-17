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

#include "PrototypeInput.h"

#include "PrototypeEngine.h"
#include "PrototypeRenderer.h"
#include "PrototypeUI.h"
#include "PrototypeWindow.h"

#include <GLFW/glfw3.h>

PrototypeInput::PrototypeInput() {}

PrototypeInput::~PrototypeInput() {}

void
PrototypeInput::update()
{
    for (auto& pair : _keyValues) {
        if (!pair.second.first) {
            if (pair.second.second > 0.0f) {
                pair.second.second -= pair.second.second * 10.0f * PrototypeEngineInternalApplication::window->deltaTime();
            } else {
                pair.second.second = 0.0f;
            }
        } else {
            if (pair.second.second < 1.0f) {
                pair.second.second +=
                  0.01f + (1.0f - pair.second.second) * 10.0f * PrototypeEngineInternalApplication::window->deltaTime();
            } else {
                pair.second.second = 1.0f;
            }
        }
    }
}

void
PrototypeInput::releaseAllKeyboardKeys()
{
    for (auto& pair : _keyValues) { pair.second.first = false; }
}

void
PrototypeInput::onKeyboard(i32 key, i32 scancode, i32 action, i32 mods)
{
    bool blocked = PrototypeEngineInternalApplication::window->isShiftDown();

    for (auto& pair : _keyValues) {
        if (key == pair.first) {
            if (blocked || action == GLFW_RELEASE) {
                pair.second.first = false;
            } else if (action == GLFW_PRESS) {
                pair.second.first = true;
            }
        }
    }
}

void
PrototypeInput::subscribeKeyForStreams(int32_t key)
{
    _keyValues.insert({ key, { false, 0.0f } });
}

void
PrototypeInput::unsubscribeKeyForStreams(int32_t key)
{
    _keyValues.erase(key);
}

f32
PrototypeInput::fetchKeyNormalizedValue(int32_t key) const
{
    auto it = _keyValues.find(key);
    if (it != _keyValues.end()) { return it->second.second; }
    return 0.0f;
}

bool
PrototypeInput::fetchKeyPressed(int32_t key) const
{
    auto it = _keyValues.find(key);
    if (it != _keyValues.end()) { return static_cast<f32>(it->second.first); }
    return false;
}