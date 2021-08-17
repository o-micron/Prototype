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

#include "PrototypeMaterial.h"

#include "PrototypeShaderBuffer.h"
#include "PrototypeTextureBuffer.h"

#include "PrototypeDatabase.h"
#include "PrototypeEngine.h"
#include "PrototypeStaticInitializer.h"

PrototypeMaterial::PrototypeMaterial(const std::string name)
  : _id(++PrototypeStaticInitializer::_materialUUID)
  , _name(name)
{}

PrototypeMaterial::~PrototypeMaterial() {}

const u32&
PrototypeMaterial::id() const
{
    return _id;
}

const std::string&
PrototypeMaterial::name() const
{
    return _name;
}

const PrototypeShaderBuffer*
PrototypeMaterial::shader() const
{
    return _shader;
}

const std::vector<PrototypeTextureBuffer*>&
PrototypeMaterial::textures() const
{
    return _textures;
}

const glm::vec3&
PrototypeMaterial::baseColor() const
{
    return _baseColor;
}

const f32&
PrototypeMaterial::metallic() const
{
    return _metallic;
}

const f32&
PrototypeMaterial::roughness() const
{
    return _roughness;
}

void
PrototypeMaterial::to_json(nlohmann::json& j, const PrototypeMaterial& material)
{
    const char* field_id        = "id";
    const char* field_name      = "name";
    const char* field_shader    = "shader";
    const char* field_textures  = "textures";
    const char* field_baseColor = "baseColor";
    const char* field_metallic  = "metallic";
    const char* field_roughness = "roughness";

    j[field_name]   = material._name;
    j[field_shader] = material._shader->name();
    std::vector<std::string> data;
    for (const auto& texture : material._textures) { data.push_back(texture->name()); }
    j[field_textures].push_back(nlohmann::json(data));
    j[field_baseColor] = material._baseColor;
    j[field_metallic]  = material._metallic;
    j[field_roughness] = material._roughness;
}

void
PrototypeMaterial::from_json(const nlohmann::json& j)
{
    if (j.is_null()) { return; }

    const char* field_name      = "name";
    const char* field_shader    = "shader";
    const char* field_textures  = "textures";
    const char* field_baseColor = "baseColor";
    const char* field_metallic  = "metallic";
    const char* field_roughness = "roughness";

    std::string name = j.at(field_name).get<std::string>();
    if (name.empty()) { return; }
    
    PrototypeMaterial* material = PrototypeEngineInternalApplication::database->allocateMaterial(name);

    auto shaderIt = PrototypeEngineInternalApplication::database->shaderBuffers.find(j.at(field_shader).get<std::string>());
    if (shaderIt == PrototypeEngineInternalApplication::database->shaderBuffers.end()) {
        PrototypeEngineInternalApplication::database->deallocateMaterial(material);
        return;
    }
    material->_shader = shaderIt->second;
    for (auto jtexture : j.at(field_textures)) {
        auto textureIt = PrototypeEngineInternalApplication::database->textureBuffers.find(jtexture.get<std::string>());
        if (textureIt == PrototypeEngineInternalApplication::database->textureBuffers.end()) {
            PrototypeEngineInternalApplication::database->deallocateMaterial(material);
            return;
        }
        material->_textures.push_back(textureIt->second);
    }

    if (j.contains(field_baseColor)) {
        material->_baseColor = j.at(field_baseColor).get<glm::vec3>();
    } else {
        material->_baseColor = glm::vec3(1.0f);
    }

    if (j.contains(field_metallic)) {
        material->_metallic = j.at(field_metallic).get<f32>();
    } else {
        material->_metallic = 0.0f;
    }

    if (j.contains(field_roughness)) {
        material->_roughness = j.at(field_roughness).get<f32>();
    } else {
        material->_roughness = 0.0f;
    }

    PrototypeEngineInternalApplication::database->materials.insert({ material->name(), material });
}