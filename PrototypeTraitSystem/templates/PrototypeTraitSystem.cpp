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

#include "../include/PrototypeTraitSystem/PrototypeTraitSystem.h"

#include <algorithm>


PrototypeTraitSystemData* PrototypeTraitSystem::_data = nullptr;

PrototypeTraitSystemData::PrototypeTraitSystemData()
    : _gid(0)
    {% for trait in data.traits -%}
        , _add{{ trait.name.functionName }}CbFn(nullptr)
        , _reuse{{ trait.name.functionName }}CbFn(nullptr)
        , _remove{{ trait.name.functionName }}CbFn(nullptr)
        , _log{{ trait.name.functionName }}CbFn(nullptr)
    {% endfor %}
{}

PrototypeTraitSystemData::~PrototypeTraitSystemData() {}

PrototypeObject::PrototypeObject()
  : _id(++PrototypeTraitSystem::_data->_gid)
  , _parentNode(nullptr)
  , _traits({ 0 })
{}

PrototypeObject::~PrototypeObject()
{
    {% for trait in data.traits -%}
        remove{{ trait.name.functionName }}Trait();
    {% endfor %}
    {% for trait in data.traits -%}
        garbageCollect{{ trait.name.functionName }}TraitMemory();
    {% endfor %}
    _id = 0;
}

void
PrototypeObject::destroy()
{
    auto it = std::find(PrototypeTraitSystem::_data->_objects.begin(), PrototypeTraitSystem::_data->_objects.end(), this);
    if (it != PrototypeTraitSystem::_data->_objects.end()) {
        PrototypeObject* et = *it;
        PrototypeTraitSystem::_data->_objects.erase(it);
        PrototypeTraitSystem::_data->_pool.deleteElement(et);
    }
}

u32
PrototypeObject::id() const
{
    return _id;
}

MASK_TYPE 
PrototypeObject::traits() const
{
    return _traits.features;
}

void
PrototypeObject::log()
{
    {% for trait in data.traits -%}
        if (has{{ trait.name.functionName }}Trait()) {
            if (PrototypeTraitSystem::_data->_log{{ trait.name.functionName }}CbFn) {
                if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMask{{ trait.name.text }})) {
                    {{ trait.name.text }}* cmp = PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Vector[PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Map[_id]];
                    PrototypeTraitSystem::_data->_log{{ trait.name.functionName }}CbFn(this, cmp);
                }
            }
        }
    {% endfor %}
}

void
PrototypeObject::add(MASK_TYPE traitMask)
{
    PrototypeBitflag bf = PrototypeBitflagFrom(traitMask);
    {% for trait in data.traits -%}
        if (PrototypeBitflagHas(bf, PrototypeTraitTypeMask{{ trait.name.text }}) && !has(PrototypeTraitTypeMask{{ trait.name.text }})) { 
            PrototypeObject::add{{ trait.name.functionName }}Trait();
        }
    {% endfor %} 
}

bool
PrototypeObject::has(MASK_TYPE traitMask)
{ 
    return PrototypeBitflagHas(_traits, traitMask);
}

void
PrototypeObject::remove(MASK_TYPE traitMask)
{
    auto bf = PrototypeBitflagFrom(traitMask);
    {% for trait in data.traits -%}
        if (PrototypeBitflagHas(bf, PrototypeTraitTypeMask{{ trait.name.text }})) { 
            PrototypeObject::remove{{ trait.name.functionName }}Trait();
        }
    {% endfor %}
}

void
PrototypeObject::setParentNode(void* parentNode)
{
    _parentNode = parentNode;
}

void*
PrototypeObject::parentNode() const
{
    if(_parentNode) {
        return static_cast<void*>(_parentNode);
    }
    return nullptr;
}

void
PrototypeObject::onEdit()
{
    {% for trait in data.traits -%}
        if (PrototypeObject::has{{ trait.name.functionName }}Trait()) { 
            {{ trait.name.text }}::onEditDispatch(this);
        }
    {% endfor %}
}

{% for trait in data.traits -%}
    void PrototypeObject::add{{ trait.name.functionName }}Trait()
    {
        if (!PrototypeBitflagHas(_traits, PrototypeTraitTypeMask{{ trait.name.text }})) {
            PrototypeBitflagAdd(_traits, PrototypeTraitTypeMask{{ trait.name.text }});
            if (PrototypeTraitSystem::_data->_{{ trait.name.variableName }}GarbageMap.find(_id) == PrototypeTraitSystem::_data->_{{ trait.name.variableName }}GarbageMap.end()) {
                PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Map[_id] = PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Vector.size();
                auto newcmp = PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Pool.newElement();
                newcmp->_object = this;
                PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Vector.emplace_back(newcmp);
                if (PrototypeTraitSystem::_data->_add{{ trait.name.functionName }}CbFn) {
                    PrototypeTraitSystem::_data->_add{{ trait.name.functionName }}CbFn(this, newcmp);
                }
            } else {
                PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Map[_id] = PrototypeTraitSystem::_data->_{{ trait.name.variableName }}GarbageMap[_id];
                PrototypeTraitSystem::_data->_{{ trait.name.variableName }}GarbageMap.erase(_id);
                if (PrototypeTraitSystem::_data->_reuse{{ trait.name.functionName }}CbFn) {
                    PrototypeTraitSystem::_data->_reuse{{ trait.name.functionName }}CbFn(this, PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Vector[PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Map[_id]]);
                }
            }
        }
    }

    void PrototypeObject::remove{{ trait.name.functionName }}Trait()
    {
        if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMask{{ trait.name.text }})) {
            if (PrototypeTraitSystem::_data->_remove{{ trait.name.functionName }}CbFn) {
                PrototypeTraitSystem::_data->_remove{{ trait.name.functionName }}CbFn(this, this->get{{ trait.name.functionName }}Trait());
            }
            PrototypeBitflagRemove(_traits, PrototypeTraitTypeMask{{ trait.name.text }});
            PrototypeTraitSystem::_data->_{{ trait.name.variableName }}GarbageMap[_id] = PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Map[_id];
            PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Map.erase(_id);
        }
    }

    void PrototypeObject::garbageCollect{{ trait.name.functionName }}TraitMemory()
    {
        for (auto pair : PrototypeTraitSystem::_data->_{{ trait.name.variableName }}GarbageMap) {
            for (auto& pair2 : PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Map) {
                if (pair2.second > pair.second) { --pair2.second; }
            }
            PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Pool.deleteElement(PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Vector[pair.second]);
            PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Vector.erase(PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Vector.begin() + pair.second);
        }
        PrototypeTraitSystem::_data->_{{ trait.name.variableName }}GarbageMap.clear();
    }

    bool PrototypeObject::has{{ trait.name.functionName }}Trait() const 
    { 
        return PrototypeBitflagHas(_traits, PrototypeTraitTypeMask{{ trait.name.text }});
    }

    {{ trait.name.text }}* PrototypeObject::get{{ trait.name.functionName }}Trait() const
    {
        if (PrototypeBitflagHas(_traits, PrototypeTraitTypeMask{{ trait.name.text }})) {
            return PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Vector[PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Map[_id]];
        }
        return nullptr;
    }
{% endfor %}

void
PrototypeObject::to_json(nlohmann::json& j, const PrototypeObject& o)
{
    {% for trait in data.traits -%}
    {
        if(o.has{{ trait.name.functionName }}Trait()) {
            nlohmann::json jj;
            {{ trait.name.text }}::to_json(jj, (const {{ trait.name.text }})*(o.get{{ trait.name.functionName }}Trait()));
            j.push_back(jj);
        }
    }
    {% endfor %}
}

void
PrototypeObject::from_json(const nlohmann::json& j, PrototypeObject& o)
{
    const char* field_name = "name";
    {% for trait in data.traits -%}
    if(j.at(field_name) == "{{ trait.name.text }}") {
        o.add{{ trait.name.functionName }}Trait();
        {{ trait.name.text }}::from_json(j, *(o.get{{ trait.name.functionName }}Trait()), &o);
        return;
    }
    {% endfor %}
}


// TraitSystem
PrototypeObject*
PrototypeTraitSystem::createObject()
{
    PrototypeObject* et = PrototypeTraitSystem::_data->_pool.newElement();
    PrototypeTraitSystem::_data->_objects.push_back(et);
    return et;
}

void
PrototypeTraitSystem::clearObjects()
{
    for (auto it = PrototypeTraitSystem::_data->_objects.begin(); it != PrototypeTraitSystem::_data->_objects.end(); ++it) { 
        PrototypeTraitSystem::_data->_pool.deleteElement(*it);
    }
    PrototypeTraitSystem::_data->_objects.clear();
}

const std::vector<PrototypeObject*>&
PrototypeTraitSystem::objects()
{
    return PrototypeTraitSystem::_data->_objects;
}

{% for trait in data.traits -%}
    const std::vector<{{ trait.name.text }}*>& PrototypeTraitSystem::{{ trait.name.variableName }}Vector()
    {
        return PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Vector;
    }

    const std::unordered_map<u32, size_t>& PrototypeTraitSystem::{{ trait.name.variableName }}Map()
    {
        return PrototypeTraitSystem::_data->_{{ trait.name.variableName }}Map;
    }

    const std::unordered_map<u32, size_t>& PrototypeTraitSystem::{{ trait.name.variableName }}GarbageMap()
    {
        return PrototypeTraitSystem::_data->_{{ trait.name.variableName }}GarbageMap;
    }

    void PrototypeTraitSystem::set{{ trait.name.functionName }}TraitAddCbFnPtr(add{{ trait.name.functionName }}Fn cbfn)
    {
        PrototypeTraitSystem::_data->_add{{ trait.name.functionName }}CbFn = cbfn;
    }

    void PrototypeTraitSystem::set{{ trait.name.functionName }}TraitReuseCbFnPtr(reuse{{ trait.name.functionName }}Fn cbfn)
    {
        PrototypeTraitSystem::_data->_reuse{{ trait.name.functionName }}CbFn = cbfn;
    }

    void PrototypeTraitSystem::set{{ trait.name.functionName }}TraitRemoveCbFnPtr(remove{{ trait.name.functionName }}Fn cbfn)
    {
        PrototypeTraitSystem::_data->_remove{{ trait.name.functionName }}CbFn = cbfn;
    }
    void PrototypeTraitSystem::set{{ trait.name.functionName }}TraitLogCbFnPtr(log{{ trait.name.functionName }}Fn cbfn)
    {
        PrototypeTraitSystem::_data->_log{{ trait.name.functionName }}CbFn = cbfn;
    }
{% endfor %}

extern void**
PrototypeTraitSystemGetDataInternal()
{
    return &((void*)PrototypeTraitSystem::_data);
}

extern void
PrototypeTraitSystemSetDataInternal(void** data)
{
    PrototypeTraitSystem::_data = (PrototypeTraitSystemData*)(*data);
}

extern bool
PrototypeTraitSystemInit()
{
    PrototypeTraitSystem::_data = PROTOTYPE_NEW PrototypeTraitSystemData();
    return true;
}

extern "C" void**
PrototypeTraitSystemGetData()
{
    return PrototypeTraitSystemGetDataInternal();
}

extern "C" void
PrototypeTraitSystemSetData(void** data)
{
    PrototypeTraitSystemSetDataInternal(data);
}

extern void
PrototypeTraitSystemDeinit()
{
    delete PrototypeTraitSystem::_data;
}