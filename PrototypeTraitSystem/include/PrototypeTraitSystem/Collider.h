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
#include <PrototypeCommon/Types.h>

#include <nlohmann/json.hpp>

struct PrototypeObject;

enum ColliderShape_
{
    ColliderShape_Plane,
    ColliderShape_Box,
    ColliderShape_Sphere,
    ColliderShape_Capsule,
    ColliderShape_ConvexMesh,
    ColliderShape_TriangleMesh,

    ColliderShape_Count
};

PROTOTYPE_FOR_EACH_X(PROTOTYPE_STRINGIFY_ENUM_EXTENDED,
                     PROTOTYPE_STRINGIFY_ENUM,
                     ColliderShape_Plane,
                     ColliderShape_Box,
                     ColliderShape_Sphere,
                     ColliderShape_Capsule,
                     ColliderShape_ConvexMesh,
                     ColliderShape_TriangleMesh);

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* onEditDispatchHandlerFn)(PrototypeObject* o);

struct Attachable(Trait) Collider
{
    void setShapeRef(void* shapeRef);
    void setShapeType(ColliderShape_ shapeType);
    void setWidth(f32 width);
    void setRadius(f32 radius);
    void setHeight(f32 height);
    void setDepth(f32 depth);
    void setDensity(f32 density);
    void setNameRef(std::string nameRef);

    void*                 shapeRef();
    const ColliderShape_& shapeType() const;
    const f32&            width() const;
    const f32&            radius() const;
    const f32&            height() const;
    const f32&            depth() const;
    const f32&            density() const;
    const std::string&    nameRef() const;

    PrototypeObject* object();
    static void      setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler);
    static void      onEditDispatch(PrototypeObject * o);
    static void      to_json(nlohmann::json & j, const Collider& c);
    static void      from_json(const nlohmann::json& j, Collider& c, PrototypeObject* o);

  private:
    friend struct PrototypeObject;
    PrototypeObject*               _object;
    static onEditDispatchHandlerFn _onEditDispatchHandler;
    void*                          _shapeRef;
    ColliderShape_                 _shapeType;
    union
    {
        struct
        {
            f32 _width;
            f32 _height;
            f32 _depth;
        };
        struct
        {
            f32 _radius;
            f32 __sphere_pad1;
            f32 __sphere_pad2;
        };
        struct
        {
            f32 __capsule_pad0;
            f32 __capsule_pad1;
            f32 _density;
        };
    };
    std::string _nameRef;
};
