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

#include "PrototypeTextureBuffer.h"
#include "PrototypeDatabase.h"
#include "PrototypeEngine.h"
#include "PrototypeRenderer.h"
#include "PrototypeStaticInitializer.h"
#include "PrototypeUI.h"

#include <PrototypeCommon/IO.h>
#include <PrototypeCommon/Logger.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void
loadSourceFromFile(PrototypeTextureBufferSource* textureBufferSource)
{
    u8* textureData = stbi_load(textureBufferSource->fullpath.c_str(),
                                &textureBufferSource->width,
                                &textureBufferSource->height,
                                &textureBufferSource->components,
                                STBI_rgb_alpha);
    if (!textureData) {
        PrototypeLogger::warn("Texture file not found %s", textureBufferSource->fullpath.c_str());
        return;
    }
    textureBufferSource->components = STBI_rgb_alpha;
    textureBufferSource->data       = std::vector<u8>(
      textureData, textureData + (textureBufferSource->width * textureBufferSource->height * textureBufferSource->components));
    stbi_image_free(textureData);
}

PrototypeTextureBuffer::PrototypeTextureBuffer(const std::string name)
  : _id(++PrototypeStaticInitializer::_textureBufferUUID)
  , _name(name)
  , userData(nullptr)
  , _needsUpload(false)
{}

PrototypeTextureBuffer::~PrototypeTextureBuffer() { unsetData(); }

const u32&
PrototypeTextureBuffer::id() const
{
    return _id;
}

const std::string&
PrototypeTextureBuffer::name() const
{
    return _name;
}

const PrototypeTextureBufferSource&
PrototypeTextureBuffer::source() const
{
    if (_source->data.empty()) { loadSourceFromFile(_source.get()); }
    return *_source.get();
}

const bool&
PrototypeTextureBuffer::needsUpload() const
{
    return _needsUpload;
}

void
PrototypeTextureBuffer::setSource(std::shared_ptr<PrototypeTextureBufferSource> source)
{
    _source = std::move(source);
}

void
PrototypeTextureBuffer::stageChange()
{
    _source->timestamp = PrototypeIo::filestamp(_source->fullpath);
    loadSourceFromFile(_source.get());
    _needsUpload = true;
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    PrototypeEngineInternalApplication::renderer->ui()->signalBuffersChanged(true);
#endif
}

void
PrototypeTextureBuffer::commitChange()
{
    if (_needsUpload) {
        _needsUpload = false;
        PrototypeEngineInternalApplication::renderer->onTextureBufferGpuUpload(this);
    }
}

void
PrototypeTextureBuffer::unsetData()
{
    _source->data.clear();
    _source->data.shrink_to_fit();
    _source->width      = 0;
    _source->height     = 0;
    _source->components = 0;
}

void
PrototypeTextureBuffer::to_json(nlohmann::json& j, const PrototypeTextureBuffer& textureBuffer)
{
    j["id"]   = textureBuffer.id();
    j["name"] = textureBuffer.name();
}

void
PrototypeTextureBuffer::from_json(const nlohmann::json& j)
{
    const std::string texturePath     = j.get<std::string>();
    const std::string textureFullPath = PROTOTYPE_TEXTURE_PATH("") + texturePath;

    if (texturePath.empty()) { return; }

    if (PrototypeEngineInternalApplication::database->textureBuffers.find(texturePath) !=
        PrototypeEngineInternalApplication::database->textureBuffers.end()) {
        return;
    }
    auto textureBufferSource = std::make_shared<PrototypeTextureBufferSource>(
      textureFullPath, std::vector<u8>(), 0, 0, 0, PrototypeIo::filestamp(textureFullPath));
    loadSourceFromFile(textureBufferSource.get());
    if (!textureBufferSource->data.empty()) {
        PrototypeTextureBuffer* textureBuffer = PrototypeEngineInternalApplication::database->allocateTextureBuffer(texturePath);
        textureBuffer->setSource(std::move(textureBufferSource));
        PrototypeEngineInternalApplication::database->textureBuffers.insert({ texturePath, textureBuffer });
    }
}