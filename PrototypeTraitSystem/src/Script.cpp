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

#include "../include/PrototypeTraitSystem/Script.h"

#include <PrototypeCommon/Logger.h>

#include <setjmp.h>

onEditDispatchHandlerFn Script::_onEditDispatchHandler = nullptr;

PrototypeObject*
Script::object()
{
    return _object;
}

void
Script::setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler)
{
    _onEditDispatchHandler = onEditDispatchHandler;
}

void
Script::onEditDispatch(PrototypeObject* o)
{
    if (_onEditDispatchHandler) { _onEditDispatchHandler(o); }
}

void
Script::to_json(nlohmann::json& j, const Script& s)
{
    const char* field_name = "name";

    j[field_name] = PROTOTYPE_STRINGIFY(Collider);
}

void
Script::from_json(const nlohmann::json& j, Script& s, PrototypeObject* o)
{
    s._object = o;
}

bool
ScriptCodeLink::operator==(const ScriptCodeLink& other) const
{
    return filepath == other.filepath;
}
bool
ScriptCodeLink::operator<(const ScriptCodeLink& other) const
{
    return filepath < other.filepath;
}