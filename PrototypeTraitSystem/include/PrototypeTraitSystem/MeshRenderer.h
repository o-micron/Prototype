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

#include <PrototypeCommon/Definitions.h>

#include <PrototypeCommon/Maths.h>
#include <PrototypeCommon/Types.h>

#include <array>

#include <nlohmann/json.hpp>

struct PrototypeObject;

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onEditDispatchHandlerFn)(PrototypeObject* o);

enum MeshRendererPolygonMode_
{
    MeshRendererPolygonMode_POINT = 0,
    MeshRendererPolygonMode_LINE,
    MeshRendererPolygonMode_FILL,

    MeshRendererPolygonMode_COUNT
};

struct MeshRendererMeshMaterialTuple
{
    MeshRendererPolygonMode_ polygonMode;
    std::string              mesh;
    std::string              material;
};

struct Attachable(Trait) MeshRenderer
{
    void setData(std::vector<MeshRendererMeshMaterialTuple> & data);
    void setMeshAtIndex(size_t index, std::string mesh);
    void setMaterialAtIndex(size_t index, std::string material);

    std::vector<MeshRendererMeshMaterialTuple>& data();

    PrototypeObject* object();
    static void      setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler);
    static void      onEditDispatch(PrototypeObject * o);
    static void      to_json(nlohmann::json & j, const MeshRenderer& mr);
    static void      from_json(const nlohmann::json& j, MeshRenderer& mr, PrototypeObject* o);

  private:
    friend struct PrototypeObject;
    PrototypeObject*                           _object;
    static onEditDispatchHandlerFn             _onEditDispatchHandler;
    std::vector<MeshRendererMeshMaterialTuple> _data;
};
