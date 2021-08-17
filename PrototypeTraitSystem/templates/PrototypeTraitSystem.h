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

#include <unordered_map>
#include <vector>
#include <optional>

#include <PrototypeCommon/Bitflag.h>
#include <PrototypeCommon/MemoryPool.h>

#include <nlohmann/json.hpp>

#include "PrototypeTraitSystemTypes.h"

#define _Bool bool

{% for trait in data.traits -%}
    #include "{{ trait.fileStem }}{{ trait.fileExtension }}"
{% endfor %}

struct PrototypeObject;
struct PrototypeTraitSystem;

{% for trait in data.traits -%}
    struct {{ trait.name.text }}{{ trait.suffix }} {
    {% for field in trait.fields -%}
        {{ field.optType }} {{ field.name }};
    {% endfor %}
};
{% endfor %}

{% for trait in data.traits -%}
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* add{{ trait.name.functionName }}Fn)(PrototypeObject*, {{ trait.name.text }}*);
{% endfor %}

{% for trait in data.traits -%}
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* reuse{{ trait.name.functionName }}Fn)(PrototypeObject*, {{ trait.name.text }}*);
{% endfor %}

{% for trait in data.traits -%}
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* remove{{ trait.name.functionName }}Fn)(PrototypeObject*, {{ trait.name.text }}*);
{% endfor %}

{% for trait in data.traits -%}
    typedef void(PROTOTYPE_DYNAMIC_FN_CALL* log{{ trait.name.functionName }}Fn)(PrototypeObject*, {{ trait.name.text }}*);
{% endfor %}

static const MASK_TYPE PrototypeTraitTypeCount = {{ data.count }};

{% for trait in data.traits -%}
    static const MASK_TYPE PrototypeTraitTypeIndex{{ trait.name.text }} = {{ loop.index-1 }};
{% endfor %}

{% for trait in data.traits -%}
    static MASK_TYPE PrototypeTraitTypeMask{{ trait.name.text }} = {{ 2 ** (loop.index-1) }};
{% endfor %}

{% for trait in data.traits -%}
    #define PrototypeTraitTypeAbsoluteString{{ trait.name.text }} "{{ trait.name.text }}"
{% endfor %}

{% for trait in data.traits -%}
    #define PrototypeTraitTypeString{{ trait.name.text }} "PrototypeTraitTypeString{{ trait.name.text }}"
{% endfor %}

static const std::string PrototypeTraitTypeAbsoluteStringArray[] = {
    {% for trait in data.traits -%}
        "{{ trait.name.text }}"{{ ", " if not loop.last }}
    {% endfor %}
};

static const std::unordered_map<std::string, MASK_TYPE> PrototypeTraitTypeMaskFromAbsoluteString = {
    {% for trait in data.traits -%}
        { "{{ trait.name.text }}", PrototypeTraitTypeMask{{ trait.name.text }} }{{ ", " if not loop.last }}
    {% endfor %}
};

struct PrototypeObject
{
    void                    destroy();
    u32                     id() const;
    MASK_TYPE               traits() const;
    void                    log();
    void                    add(MASK_TYPE traitMask);
    bool                    has(MASK_TYPE traitMask);
    void                    remove(MASK_TYPE traitName);
    void                    setParentNode(void* parentNode);
    void*                   parentNode() const;
    void                    onEdit();
    
    {% for trait in data.traits -%}
        void add{{ trait.name.functionName }}Trait();
    {% endfor %}
    
    {% for trait in data.traits -%}
        void remove{{ trait.name.functionName }}Trait();
    {% endfor %}

    {% for trait in data.traits -%}
        void garbageCollect{{ trait.name.functionName }}TraitMemory();
    {% endfor %}

    {% for trait in data.traits -%}
        bool has{{ trait.name.functionName }}Trait() const;
    {% endfor %}

    {% for trait in data.traits -%}
        {{ trait.name.text }}* get{{ trait.name.functionName }}Trait() const;
    {% endfor %}

    static void to_json(nlohmann::json& j, const PrototypeObject& o);
    static void from_json(const nlohmann::json& j, PrototypeObject& o);

  private:
    friend struct MemoryPool<PrototypeObject, 100>;

    ~PrototypeObject();
    PrototypeObject();
    
    u32                                     _id;
    void*                                   _parentNode;
    PrototypeBitflag                        _traits;
};

struct PrototypeTraitSystemData
{
    PrototypeTraitSystemData();
    ~PrototypeTraitSystemData();

    u32                                  _gid;
    MemoryPool<PrototypeObject, 100>     _pool;
    std::vector<PrototypeObject*>        _objects;
    
    {% for trait in data.traits -%}
        MemoryPool<{{ trait.name.text }}, 100> _{{ trait.name.variableName }}Pool;
        std::vector<{{ trait.name.text }}*> _{{ trait.name.variableName }}Vector;
    {% endfor %}

    {% for trait in data.traits -%}
        std::unordered_map<u32, size_t> _{{ trait.name.variableName }}Map;
    {% endfor %}

    {% for trait in data.traits -%}
        std::unordered_map<u32, size_t> _{{ trait.name.variableName }}GarbageMap;
    {% endfor %}

    {% for trait in data.traits -%}
        add{{ trait.name.functionName }}Fn _add{{ trait.name.functionName }}CbFn;
    {% endfor %}

    {% for trait in data.traits -%}
        reuse{{ trait.name.functionName }}Fn _reuse{{ trait.name.functionName }}CbFn;
    {% endfor %}

    {% for trait in data.traits -%}
        remove{{ trait.name.functionName }}Fn _remove{{ trait.name.functionName }}CbFn;
    {% endfor %}

    {% for trait in data.traits -%}
        log{{ trait.name.functionName }}Fn _log{{ trait.name.functionName }}CbFn;
    {% endfor %}
};

struct PrototypeTraitSystem
{
    static PrototypeObject*                     createObject();
    static void                                 clearObjects();
    static const std::vector<PrototypeObject*>& objects();

    {% for trait in data.traits -%}
        static const std::vector<{{ trait.name.text }}*>& {{ trait.name.variableName }}Vector();
    {% endfor %}

    {% for trait in data.traits -%}
        static const std::unordered_map<u32, size_t>& {{ trait.name.variableName }}Map();
    {% endfor %}

    {% for trait in data.traits -%}
        static const std::unordered_map<u32, size_t>& {{ trait.name.variableName }}GarbageMap();
    {% endfor %}

    {% for trait in data.traits -%}
        static void set{{ trait.name.functionName }}TraitAddCbFnPtr(add{{ trait.name.functionName }}Fn cbfn);
    {% endfor %}

    {% for trait in data.traits -%}
        static void set{{ trait.name.functionName }}TraitReuseCbFnPtr(reuse{{ trait.name.functionName }}Fn cbfn);
    {% endfor %}

    {% for trait in data.traits -%}
        static void set{{ trait.name.functionName }}TraitRemoveCbFnPtr(remove{{ trait.name.functionName }}Fn cbfn);
    {% endfor %}

    {% for trait in data.traits -%}
        static void set{{ trait.name.functionName }}TraitLogCbFnPtr(log{{ trait.name.functionName }}Fn cbfn);
    {% endfor %}
    
    static PrototypeTraitSystemData* data();
    static void                      setData(PrototypeTraitSystemData* data);

private:
    friend struct MemoryPool<PrototypeObject, 100>;
    friend struct PrototypeObject;
    friend bool   PrototypeTraitSystemInit();
    friend void** PrototypeTraitSystemGetDataInternal();
    friend void   PrototypeTraitSystemSetDataInternal(void** data);
    friend void   PrototypeTraitSystemDeinit();
    
    static PrototypeTraitSystemData* _data;
};

extern void**
PrototypeTraitSystemGetDataInternal();

extern void
PrototypeTraitSystemSetDataInternal(void** data);

extern bool
PrototypeTraitSystemInit();

extern "C" void**
PrototypeTraitSystemGetData();

extern "C" void
PrototypeTraitSystemSetData(void** data);

extern void
PrototypeTraitSystemDeinit();