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

#include <nlohmann/json.hpp>

struct PrototypeFrameBuffer
{
    PrototypeFrameBuffer(std::string name, std::string shader, u32 numColorAttachments, bool withDepthAttachment);
    ~PrototypeFrameBuffer() = default;

    const u32&         id() const;
    const std::string& name() const;
    const std::string& shader() const;
    const u32&         numColorAttachments() const;
    const bool&        withDepthAttachment() const;

    static void to_json(nlohmann::json& j, const PrototypeFrameBuffer& frameBuffer);
    static void from_json(const nlohmann::json& j);

  private:
    const u32         _id;
    const std::string _name;
    const std::string _shader;
    const u32         _numColorAttachments;
    const bool        _withDepthAttachment;
};
