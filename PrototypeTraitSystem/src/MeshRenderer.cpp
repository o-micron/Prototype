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

#include "../include/PrototypeTraitSystem/MeshRenderer.h"

#include <utility>

onEditDispatchHandlerFn MeshRenderer::_onEditDispatchHandler = nullptr;

void
MeshRenderer::setData(std::vector<MeshRendererMeshMaterialTuple>& data)
{
    _data = std::move(data);
}

void
MeshRenderer::setMeshAtIndex(size_t index, std::string mesh)
{
    if (index >= 0 && index < _data.size()) { _data[index].mesh = mesh; }
}

void
MeshRenderer::setMaterialAtIndex(size_t index, std::string material)
{
    if (index >= 0 && index < _data.size()) { _data[index].material = material; }
}

std::vector<MeshRendererMeshMaterialTuple>&
MeshRenderer::data()
{
    return _data;
}

PrototypeObject*
MeshRenderer::object()
{
    return _object;
}

void
MeshRenderer::setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler)
{
    _onEditDispatchHandler = onEditDispatchHandler;
}

void
MeshRenderer::onEditDispatch(PrototypeObject* o)
{
    if (_onEditDispatchHandler) { _onEditDispatchHandler(o); }
}

void
MeshRenderer::to_json(nlohmann::json& j, const MeshRenderer& mr)
{
    const char* field_name = "name";
    const char* field_data = "data";

    j[field_name] = PROTOTYPE_STRINGIFY(MeshRenderer);
    std::vector<nlohmann::json> data;
    for (const auto& d : mr._data) { data.push_back({ { "mesh", d.mesh }, { "material", d.material } }); }
    j[field_data] = data;
}

void
MeshRenderer::from_json(const nlohmann::json& j, MeshRenderer& mr, PrototypeObject* o)
{
    const char* field_data = "data";

    mr._data.clear();
    for (const auto& d : j.at(field_data)) {
        MeshRendererMeshMaterialTuple mrmmt = {};
        mrmmt.mesh                          = d.at("mesh").get<std::string>();
        mrmmt.material                      = d.at("material").get<std::string>();
        mrmmt.polygonMode                   = MeshRendererPolygonMode_FILL;
        mr._data.emplace_back(mrmmt);
    }

    mr._object = o;
}