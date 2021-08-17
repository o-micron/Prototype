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

#include "../include/PrototypeTraitSystem/Collider.h"

#include <PrototypeCommon/Logger.h>

#include <utility>

onEditDispatchHandlerFn Collider::_onEditDispatchHandler = nullptr;

void
Collider::setShapeRef(void* shape)
{
    _shapeRef = shape;
}

void
Collider::setShapeType(ColliderShape_ shapeType)
{
    _shapeType = shapeType;
}

void
Collider::setWidth(f32 width)
{
    _width = width;
}

void
Collider::setRadius(f32 radius)
{
    _radius = radius;
}

void
Collider::setHeight(f32 height)
{
    _height = height;
}

void
Collider::setDepth(f32 depth)
{
    _depth = depth;
}

void
Collider::setDensity(f32 density)
{
    _density = density;
}

void
Collider::setNameRef(std::string nameRef)
{
    _nameRef = std::move(nameRef);
}

void*
Collider::shapeRef()
{
    return _shapeRef;
}

const ColliderShape_&
Collider::shapeType() const
{
    return _shapeType;
}

const f32&
Collider::width() const
{
    return _width;
}

const f32&
Collider::radius() const
{
    return _radius;
}

const f32&
Collider::height() const
{
    return _height;
}

const f32&
Collider::depth() const
{
    return _depth;
}

const f32&
Collider::density() const
{
    return _density;
}

const std::string&
Collider::nameRef() const
{
    return _nameRef;
}

PrototypeObject*
Collider::object()
{
    return _object;
}

void
Collider::setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler)
{
    _onEditDispatchHandler = onEditDispatchHandler;
}

void
Collider::onEditDispatch(PrototypeObject* o)
{
    if (_onEditDispatchHandler) { _onEditDispatchHandler(o); }
}

void
Collider::to_json(nlohmann::json& j, const Collider& c)
{
    const char* field_name      = "name";
    const char* field_name_ref  = "nameRef";
    const char* field_shapeType = "shapeType";
    const char* field_width     = "width";
    const char* field_height    = "height";
    const char* field_depth     = "depth";
    const char* field_radius    = "radius";
    const char* field_density   = "density";

    j[field_name]      = PROTOTYPE_STRINGIFY(Collider);
    j[field_name_ref]  = c._nameRef;
    j[field_shapeType] = c._shapeType;
    if (j.at(field_shapeType) == ColliderShape_Plane) {
        j[field_width]  = c._width;
        j[field_height] = c._height;
        j[field_depth]  = c._depth;
    } else if (j.at(field_shapeType) == ColliderShape_Box) {
        j[field_width]  = c._width;
        j[field_height] = c._height;
        j[field_depth]  = c._depth;
    } else if (j.at(field_shapeType) == ColliderShape_Sphere) {
        j[field_radius] = c._radius;
    } else if (j.at(field_shapeType) == ColliderShape_Capsule) {
        j[field_radius]  = c._radius;
        j[field_height]  = c._height;
        j[field_density] = c._density;
    } else if (j.at(field_shapeType) == ColliderShape_ConvexMesh) {
        j[field_width]  = c._width;
        j[field_height] = c._height;
        j[field_depth]  = c._depth;
    } else if (j.at(field_shapeType) == ColliderShape_TriangleMesh) {
        j[field_width]  = c._width;
        j[field_height] = c._height;
        j[field_depth]  = c._depth;
    } else {
        PrototypeLogger::fatal("Unhandled collider type");
    }
}

void
Collider::from_json(const nlohmann::json& j, Collider& c, PrototypeObject* o)
{
    const char* field_name_ref  = "nameRef";
    const char* field_shapeType = "shapeType";
    const char* field_width     = "width";
    const char* field_height    = "height";
    const char* field_depth     = "depth";
    const char* field_radius    = "radius";
    const char* field_density   = "density";

    if (j.at(field_shapeType) == ColliderShape_Plane_Str) {
        c._shapeType = ColliderShape_Plane;
        c._width     = j.at(field_width).get<f32>();
        c._height    = j.at(field_height).get<f32>();
        c._depth     = 0.0f;
        c._nameRef   = "PLANE";
    } else if (j.at(field_shapeType) == ColliderShape_Box_Str) {
        c._shapeType = ColliderShape_Box;
        c._width     = j.at(field_width).get<f32>();
        c._height    = j.at(field_height).get<f32>();
        c._depth     = j.at(field_depth).get<f32>();
        c._nameRef   = "CUBE";
    } else if (j.at(field_shapeType) == ColliderShape_Sphere_Str) {
        c._shapeType = ColliderShape_Sphere;
        c._radius    = j.at(field_radius).get<f32>();
        c._nameRef   = "SPHERE";
    } else if (j.at(field_shapeType) == ColliderShape_Capsule_Str) {
        c._shapeType = ColliderShape_Capsule;
        c._radius    = j.at(field_radius).get<f32>();
        c._height    = j.at(field_height).get<f32>();
        c._density   = j.at(field_density).get<f32>();
        c._nameRef   = "CAPSULE";
    } else if (j.at(field_shapeType) == ColliderShape_ConvexMesh_Str) {
        c._shapeType = ColliderShape_ConvexMesh;
        c._width     = 1.0f;
        c._height    = 1.0f;
        c._depth     = 1.0f;
        c._nameRef   = std::string("(CONVEX) ").append(j.at(field_name_ref).get<std::string>());
    } else if (j.at(field_shapeType) == ColliderShape_TriangleMesh_Str) {
        c._shapeType = ColliderShape_TriangleMesh;
        c._width     = 1.0f;
        c._height    = 1.0f;
        c._depth     = 1.0f;
        c._nameRef   = std::string("(TRIMESH) ").append(j.at(field_name_ref).get<std::string>());
    } else {
        PrototypeLogger::fatal("Unhandled collider type");
    }
    c._object = o;
}