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

#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include <nlohmann/json.hpp>

struct PrototypeShaderBuffer;

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onShaderBufferReloadFn)(PrototypeShaderBuffer* shaderBuffer);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onShaderBufferGpuUploadFn)(PrototypeShaderBuffer* shaderBuffer);

enum PrototypeShaderBufferSourceType_
{
    PrototypeShaderBufferSourceType_VertexShader = 0,
    PrototypeShaderBufferSourceType_FragmentShader,

    PrototypeShaderBufferSourceType_Count
};

struct PrototypeShaderBufferBindingSource
{
    std::vector<std::string>                       textureData;
    std::vector<std::pair<std::string, f32>>       floatData;
    std::vector<std::pair<std::string, glm::vec2>> vec2Data;
    std::vector<std::pair<std::string, glm::vec3>> vec3Data;
    std::vector<std::pair<std::string, glm::vec4>> vec4Data;
};

struct PrototypeShaderBufferSource
{
    PrototypeShaderBufferSource()
      : fullpath("")
      , code("")
      , type(PrototypeShaderBufferSourceType_Count)
      , timestamp(0)
    {}

    PrototypeShaderBufferSource(std::string fullpath, std::string code, PrototypeShaderBufferSourceType_ type, time_t timestamp)
      : fullpath(fullpath)
      , code(code)
      , type(type)
      , timestamp(timestamp)
    {}
    time_t                             timestamp;
    PrototypeShaderBufferSourceType_   type;
    std::string                        fullpath;
    std::string                        code;
    PrototypeShaderBufferBindingSource bindingSource;
};

struct PrototypeShaderBuffer
{
    PrototypeShaderBuffer(const std::string name);
    ~PrototypeShaderBuffer();

    const u32&                                                       id() const;
    const std::string&                                               name() const;
    const bool&                                                      needsUpload() const;
    const std::vector<std::shared_ptr<PrototypeShaderBufferSource>>& sources() const;

    void setSources(std::vector<std::shared_ptr<PrototypeShaderBufferSource>>& sources);
    void stageChange();
    void commitChange();
    void unsetData();

    static void to_json(nlohmann::json& j, const PrototypeShaderBuffer& shaderBuffer);
    static void from_json(const nlohmann::json& j);

    void* userData;

  private:
    const u32                                                         _id;
    const std::string                                                 _name;
    bool                                                              _needsUpload;
    mutable std::vector<std::shared_ptr<PrototypeShaderBufferSource>> _sources;
};