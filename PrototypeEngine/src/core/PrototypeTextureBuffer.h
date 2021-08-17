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

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

struct PrototypeTextureBuffer;

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onTextureBufferReloadFn)(PrototypeTextureBuffer* textureBuffer);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onTextureBufferGpuUploadFn)(PrototypeTextureBuffer* textureBuffer);

struct PrototypeTextureBufferSource
{
    PrototypeTextureBufferSource(std::string                fullpath,
                                 std::vector<unsigned char> data,
                                 i32                        width,
                                 i32                        height,
                                 i32                        components,
                                 time_t                     timestamp)
      : fullpath(fullpath)
      , data(data)
      , width(width)
      , height(height)
      , components(components)
      , timestamp(timestamp)
    {}

    std::string                fullpath;
    std::vector<unsigned char> data;
    i32                        width;
    i32                        height;
    i32                        components;
    time_t                     timestamp;
};

struct PrototypeTextureBuffer
{
    PrototypeTextureBuffer(const std::string name);
    ~PrototypeTextureBuffer();

    const u32&                          id() const;
    const std::string&                  name() const;
    const PrototypeTextureBufferSource& source() const;
    const bool&                         needsUpload() const;

    void setSource(std::shared_ptr<PrototypeTextureBufferSource> source);
    void stageChange();
    void commitChange();
    void unsetData();

    static void to_json(nlohmann::json& j, const PrototypeTextureBuffer& textureBuffer);
    static void from_json(const nlohmann::json& j);

    void* userData;

  private:
    const u32                                             _id;
    const std::string                                     _name;
    bool                                                  _needsUpload;
    mutable std::shared_ptr<PrototypeTextureBufferSource> _source;
};