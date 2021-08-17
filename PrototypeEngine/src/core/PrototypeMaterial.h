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

#include <PrototypeCommon/Maths.h>

#include <nlohmann/json.hpp>

#include <memory>
#include <string>
#include <vector>

struct PrototypeTextureBuffer;
struct PrototypeShaderBuffer;

struct PrototypeMaterial
{
    PrototypeMaterial(const std::string name);
    ~PrototypeMaterial();

    const u32&                                  id() const;
    const std::string&                          name() const;
    const PrototypeShaderBuffer*                shader() const;
    const std::vector<PrototypeTextureBuffer*>& textures() const;
    const glm::vec3&                            baseColor() const;
    const f32&                                  metallic() const;
    const f32&                                  roughness() const;

    static void to_json(nlohmann::json& j, const PrototypeMaterial& material);
    static void from_json(const nlohmann::json& j);

  private:
    const u32                            _id;
    const std::string                    _name;
    PrototypeShaderBuffer*               _shader;
    std::vector<PrototypeTextureBuffer*> _textures;
    glm::vec3                            _baseColor;
    f32                                  _metallic;
    f32                                  _roughness;
};
