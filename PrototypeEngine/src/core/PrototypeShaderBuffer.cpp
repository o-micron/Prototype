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

#include "PrototypeShaderBuffer.h"
#include "PrototypeDatabase.h"
#include "PrototypeEngine.h"
#include "PrototypeRenderer.h"
#include "PrototypeStaticInitializer.h"
#include "PrototypeUI.h"

#include <PrototypeCommon/IO.h>
#include <PrototypeCommon/Logger.h>

#include <algorithm>
#include <regex>

void
parseBindingSource(std::vector<std::shared_ptr<PrototypeShaderBufferSource>>& sources)
{
    for (size_t i = 0; i < sources.size(); ++i) {
        if (sources[i]->type == PrototypeShaderBufferSourceType_VertexShader) {
            std::vector<std::pair<std::string, f32>> oldFloatData = sources[i]->bindingSource.floatData;
            sources[i]->bindingSource.floatData.clear();
            std::vector<std::pair<std::string, glm::vec2>> oldVec2Data = sources[i]->bindingSource.vec2Data;
            sources[i]->bindingSource.vec2Data.clear();
            std::vector<std::pair<std::string, glm::vec3>> oldVec3Data = sources[i]->bindingSource.vec3Data;
            sources[i]->bindingSource.vec3Data.clear();
            std::vector<std::pair<std::string, glm::vec4>> oldVec4Data = sources[i]->bindingSource.vec4Data;
            sources[i]->bindingSource.vec4Data.clear();

            std::stringstream ss(sources[i]->code);
            std::string       to;
            bool              isGui       = false;
            std::string       isGuiBuffer = "";
            while (std::getline(ss, to, '\n')) {
                std::smatch pieces_match;
                if (std::regex_search(to, pieces_match, std::regex(R"~([ ]*#pragma[ ]+gui[ ]*((.*)))~"))) {
                    isGui = true;
                    if (pieces_match.size() >= 2) {
                        isGuiBuffer = pieces_match[1].str().substr(1);
                        isGuiBuffer = isGuiBuffer.substr(0, isGuiBuffer.size() - 1);
                    }
                } else if (isGui && std::regex_search(
                                      to, pieces_match, std::regex(R"~([ ]*uniform[ ]+float[ ]+([a-z-A-Z-0-9-_]+)[ ]*)~"))) {
                    if (pieces_match.size() == 2) {
                        auto varName = pieces_match[1].str();
                        auto it      = std::find_if(
                          oldFloatData.begin(), oldFloatData.end(), [&](const auto& item) { return item.first == varName; });
                        if (it == oldFloatData.end()) {
                            std::stringstream isGuiBufferStream(isGuiBuffer);
                            float             v0;
                            isGuiBufferStream >> v0;
                            if (!isGuiBufferStream.fail()) {
                                sources[i]->bindingSource.floatData.push_back({ varName, v0 });
                            } else {
                                sources[i]->bindingSource.floatData.push_back({ varName, 0.0f });
                            }
                            isGui       = false;
                            isGuiBuffer = "";
                        } else {
                            sources[i]->bindingSource.floatData.push_back({ varName, it->second });
                            isGui       = false;
                            isGuiBuffer = "";
                        }
                    }
                } else if (isGui && std::regex_search(
                                      to, pieces_match, std::regex(R"~([ ]*uniform[ ]+vec2[ ]+([a-z-A-Z-0-9-_]+)[ ]*)~"))) {
                    if (pieces_match.size() == 2) {
                        auto varName = pieces_match[1].str();
                        auto it      = std::find_if(
                          oldVec2Data.begin(), oldVec2Data.end(), [&](const auto& item) { return item.first == varName; });
                        if (it == oldVec2Data.end()) {
                            std::stringstream isGuiBufferStream(isGuiBuffer);
                            float             v0, v1;
                            isGuiBufferStream >> v0;
                            if (!isGuiBufferStream.fail()) {
                                isGuiBufferStream >> v1;
                                if (!isGuiBufferStream.fail()) {
                                    sources[i]->bindingSource.vec2Data.push_back({ varName, { v0, v1 } });
                                } else {
                                    sources[i]->bindingSource.vec2Data.push_back({ varName, { 0.0f, 0.0f } });
                                }
                            } else {
                                sources[i]->bindingSource.vec2Data.push_back({ varName, { 0.0f, 0.0f } });
                            }
                            isGui       = false;
                            isGuiBuffer = "";
                        } else {
                            sources[i]->bindingSource.vec2Data.push_back({ varName, it->second });
                            isGui       = false;
                            isGuiBuffer = "";
                        }
                    }
                } else if (isGui && std::regex_search(
                                      to, pieces_match, std::regex(R"~([ ]*uniform[ ]+vec3[ ]+([a-z-A-Z-0-9-_]+)[ ]*)~"))) {
                    if (pieces_match.size() == 2) {
                        auto varName = pieces_match[1].str();
                        auto it      = std::find_if(
                          oldVec3Data.begin(), oldVec3Data.end(), [&](const auto& item) { return item.first == varName; });
                        if (it == oldVec3Data.end()) {
                            std::stringstream isGuiBufferStream(isGuiBuffer);
                            float             v0, v1, v2;
                            isGuiBufferStream >> v0;
                            if (!isGuiBufferStream.fail()) {
                                isGuiBufferStream >> v1;
                                if (!isGuiBufferStream.fail()) {
                                    isGuiBufferStream >> v2;
                                    if (!isGuiBufferStream.fail()) {
                                        sources[i]->bindingSource.vec3Data.push_back({ varName, { v0, v1, v2 } });
                                    } else {
                                        sources[i]->bindingSource.vec3Data.push_back({ varName, { 0.0f, 0.0f, 0.0f } });
                                    }
                                } else {
                                    sources[i]->bindingSource.vec3Data.push_back({ varName, { 0.0f, 0.0f, 0.0f } });
                                }
                            } else {
                                sources[i]->bindingSource.vec3Data.push_back({ varName, { 0.0f, 0.0f, 0.0f } });
                            }
                            isGui       = false;
                            isGuiBuffer = "";
                        } else {
                            sources[i]->bindingSource.vec3Data.push_back({ varName, it->second });
                            isGui       = false;
                            isGuiBuffer = "";
                        }
                    }
                } else if (isGui && std::regex_search(
                                      to, pieces_match, std::regex(R"~([ ]*uniform[ ]+vec4[ ]+([a-z-A-Z-0-9-_]+)[ ]*)~"))) {
                    if (pieces_match.size() == 2) {
                        auto varName = pieces_match[1].str();
                        auto it      = std::find_if(
                          oldVec4Data.begin(), oldVec4Data.end(), [&](const auto& item) { return item.first == varName; });
                        if (it == oldVec4Data.end()) {
                            std::stringstream isGuiBufferStream(isGuiBuffer);
                            float             v0, v1, v2, v3;
                            isGuiBufferStream >> v0;
                            if (!isGuiBufferStream.fail()) {
                                isGuiBufferStream >> v1;
                                if (!isGuiBufferStream.fail()) {
                                    isGuiBufferStream >> v2;
                                    if (!isGuiBufferStream.fail()) {
                                        isGuiBufferStream >> v3;
                                        if (!isGuiBufferStream.fail()) {
                                            sources[i]->bindingSource.vec4Data.push_back({ varName, { v0, v1, v2, v3 } });
                                        } else {
                                            sources[i]->bindingSource.vec4Data.push_back({ varName, { 0.0f, 0.0f, 0.0f, 0.0f } });
                                        }
                                    } else {
                                        sources[i]->bindingSource.vec4Data.push_back({ varName, { 0.0f, 0.0f, 0.0f, 0.0f } });
                                    }
                                } else {
                                    sources[i]->bindingSource.vec4Data.push_back({ varName, { 0.0f, 0.0f, 0.0f, 0.0f } });
                                }
                            } else {
                                sources[i]->bindingSource.vec4Data.push_back({ varName, { 0.0f, 0.0f, 0.0f, 0.0f } });
                            }
                            isGui       = false;
                            isGuiBuffer = "";
                        } else {
                            sources[i]->bindingSource.vec4Data.push_back({ varName, it->second });
                            isGui       = false;
                            isGuiBuffer = "";
                        }
                    }
                }
            }
        } else if (sources[i]->type == PrototypeShaderBufferSourceType_FragmentShader) {
            std::vector<std::string> oldTextureData = sources[i]->bindingSource.textureData;
            sources[i]->bindingSource.textureData.clear();

            std::stringstream ss(sources[i]->code);
            std::string       to;
            bool              isGui = true;
            while (std::getline(ss, to, '\n')) {
                std::smatch pieces_match;
                if (std::regex_search(to, pieces_match, std::regex(R"~([ ]*#pragma[ ]+gui[ ]*)~"))) {
                    isGui = true;
                } else if (isGui && std::regex_search(
                                      to, pieces_match, std::regex(R"~([ ]*uniform[ ]+sampler2D[ ]+([a-z-A-Z-0-9-_]+)[ ]*)~"))) {
                    if (pieces_match.size() == 2) {
                        auto varName = pieces_match[1].str();
                        auto it      = std::find_if(
                          oldTextureData.begin(), oldTextureData.end(), [&](const auto& item) { return item == varName; });
                        if (it == oldTextureData.end()) {
                            isGui = false;
                            sources[i]->bindingSource.textureData.push_back(varName);
                        } else {
                            isGui = false;
                            sources[i]->bindingSource.textureData.push_back(*it);
                        }
                    }
                }
            }
        }
    }
}

void
loadSourceFromFile(std::vector<std::shared_ptr<PrototypeShaderBufferSource>>& sources)
{
    sources[0]->code = "";
    sources[1]->code = "";
    if (PrototypeIo::readFileBlock(sources[0]->fullpath.c_str(), sources[0]->code)) {
        if (PrototypeIo::readFileBlock(sources[1]->fullpath.c_str(), sources[1]->code)) {
            parseBindingSource(sources);
        } else {
            PrototypeLogger::warn("Fragment shader file not found %s", sources[1]->fullpath.c_str());
            return;
        }
    } else {
        PrototypeLogger::warn("Vertex shader file not found %s", sources[0]->fullpath.c_str());
        return;
    }
}

PrototypeShaderBuffer::PrototypeShaderBuffer(const std::string name)
  : _id(++PrototypeStaticInitializer::_shaderBufferUUID)
  , _name(name)
  , userData(nullptr)
  , _needsUpload(false)
{}

PrototypeShaderBuffer::~PrototypeShaderBuffer() { unsetData(); }

const u32&
PrototypeShaderBuffer::id() const
{
    return _id;
}

const std::string&
PrototypeShaderBuffer::name() const
{
    return _name;
}

const bool&
PrototypeShaderBuffer::needsUpload() const
{
    return _needsUpload;
}

const std::vector<std::shared_ptr<PrototypeShaderBufferSource>>&
PrototypeShaderBuffer::sources() const
{
    bool unloaded = false;
    for (const auto& source : _sources) {
        if (source->code.empty()) {
            unloaded = true;
            break;
        }
    }
    if (unloaded) { loadSourceFromFile(_sources); }
    return _sources;
}

void
PrototypeShaderBuffer::setSources(std::vector<std::shared_ptr<PrototypeShaderBufferSource>>& sources)
{
    _sources.resize(sources.size());
    for (size_t i = 0; i < sources.size(); ++i) { _sources[i] = std::move(sources[i]); }
}

void
PrototypeShaderBuffer::stageChange()
{
    for (auto& source : _sources) { source->timestamp = PrototypeIo::filestamp(source->fullpath); }
    loadSourceFromFile(_sources);
    _needsUpload = true;
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    PrototypeEngineInternalApplication::renderer->ui()->signalBuffersChanged(true);
#endif
}

void
PrototypeShaderBuffer::commitChange()
{
    if (_needsUpload) {
        _needsUpload = false;
        PrototypeEngineInternalApplication::renderer->onShaderBufferGpuUpload(this);
    }
}

void
PrototypeShaderBuffer::unsetData()
{
    for (auto& source : _sources) {
        source->code.clear();
        source->code.shrink_to_fit();
    }
}

void
PrototypeShaderBuffer::to_json(nlohmann::json& j, const PrototypeShaderBuffer& shaderBuffer)
{
    j["id"]   = shaderBuffer.id();
    j["name"] = shaderBuffer.name();
}

void
PrototypeShaderBuffer::from_json(const nlohmann::json& j)
{
    if (j.is_null()) { return; }
    const std::string shaderPath         = j.get<std::string>();
    std::string       vertexShaderPath   = "";
    std::string       fragmentShaderPath = "";

    if (shaderPath.empty()) { return; }

    if (PrototypeEngineInternalApplication::database->shaderBuffers.find(shaderPath) !=
        PrototypeEngineInternalApplication::database->shaderBuffers.end()) {
        return;
    }

    // detect rendering api and set path and extension accordingly
    switch (PrototypeEngineInternalApplication::renderingApi) {
        case PrototypeEngineERenderingApi_OPENGL4_1: {
            vertexShaderPath   = PROTOTYPE_OPENGL_SHADER_PATH("") + shaderPath + "/vert.glsl";
            fragmentShaderPath = PROTOTYPE_OPENGL_SHADER_PATH("") + shaderPath + "/frag.glsl";
        } break;
        case PrototypeEngineERenderingApi_OPENGLES_3_0: {
            vertexShaderPath   = PROTOTYPE_OPENGL_SHADER_PATH("") + shaderPath + "/vert.glsl";
            fragmentShaderPath = PROTOTYPE_OPENGL_SHADER_PATH("") + shaderPath + "/frag.glsl";
        } break;
        case PrototypeEngineERenderingApi_VULKAN_1: {
            vertexShaderPath   = PROTOTYPE_VULKAN_SHADER_PATH("") + shaderPath + "/vert.spv";
            fragmentShaderPath = PROTOTYPE_VULKAN_SHADER_PATH("") + shaderPath + "/frag.spv";
        } break;
        default: {
            PrototypeLogger::fatal("renderingApi: Unimplemented!(Unreachable");
        } break;
    }
    std::vector<std::shared_ptr<PrototypeShaderBufferSource>> shaderBufferSources = {
        std::make_unique<PrototypeShaderBufferSource>(
          vertexShaderPath, "", PrototypeShaderBufferSourceType_VertexShader, PrototypeIo::filestamp(vertexShaderPath)),
        std::make_unique<PrototypeShaderBufferSource>(
          fragmentShaderPath, "", PrototypeShaderBufferSourceType_FragmentShader, PrototypeIo::filestamp(fragmentShaderPath))
    };

    loadSourceFromFile(shaderBufferSources);
    if (!shaderBufferSources[0]->code.empty() && !shaderBufferSources[1]->code.empty()) {
        auto shaderBuffer = PrototypeEngineInternalApplication::database->allocateShaderBuffer(shaderPath);
        shaderBuffer->setSources(shaderBufferSources);
        PrototypeEngineInternalApplication::database->shaderBuffers.insert({ shaderPath, shaderBuffer });
    }
}