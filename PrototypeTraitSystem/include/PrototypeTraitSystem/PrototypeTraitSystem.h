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

#include <optional>
#include <unordered_map>
#include <vector>

#include <PrototypeCommon/Bitflag.h>
#include <PrototypeCommon/MemoryPool.h>

#include <nlohmann/json.hpp>

#include "PrototypeTraitSystemTypes.h"

#define _Bool bool

#include "Camera.h"
#include "Collider.h"
#include "MeshRenderer.h"
#include "Rigidbody.h"
#include "Script.h"
#include "Transform.h"
#include "VehicleChasis.h"

struct PrototypeObject;
struct PrototypeTraitSystem;

struct CameraTrait
{
    std::optional<glm::mat4x4> _viewMatrix;
    std::optional<glm::mat4x4> _projectionMatrix;
    std::optional<glm::quat>   _rotationQuat;
    std::optional<glm::vec3>   _position;
    std::optional<glm::vec3>   _linearVelocity;
    std::optional<glm::vec2>   _linearRotation;
    std::optional<f32>         _interpolationTime;
    std::optional<glm::vec2>   _rotation;
    std::optional<f32>         _lookSensitivity;
    std::optional<f32>         _moveSensitivity;
    std::optional<glm::vec2>   _resolution;
    std::optional<f32>         _fov;
    std::optional<f32>         _aspectRatio;
    std::optional<f32>         _near;
    std::optional<f32>         _far;
    std::optional<_Bool>       _orbital;
};
struct ColliderTrait
{};
struct MeshRendererTrait
{};
struct RigidbodyTrait
{};
struct ScriptTrait
{
    std::optional<std::unordered_map<std::string, ScriptCodeLink>> codeLinks;
};
struct TransformTrait
{};
struct VehicleChasisTrait
{};

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* addCameraFn)(PrototypeObject*, Camera*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* addColliderFn)(PrototypeObject*, Collider*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* addMeshRendererFn)(PrototypeObject*, MeshRenderer*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* addRigidbodyFn)(PrototypeObject*, Rigidbody*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* addScriptFn)(PrototypeObject*, Script*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* addTransformFn)(PrototypeObject*, Transform*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* addVehicleChasisFn)(PrototypeObject*, VehicleChasis*);

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* reuseCameraFn)(PrototypeObject*, Camera*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* reuseColliderFn)(PrototypeObject*, Collider*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* reuseMeshRendererFn)(PrototypeObject*, MeshRenderer*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* reuseRigidbodyFn)(PrototypeObject*, Rigidbody*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* reuseScriptFn)(PrototypeObject*, Script*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* reuseTransformFn)(PrototypeObject*, Transform*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* reuseVehicleChasisFn)(PrototypeObject*, VehicleChasis*);

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* removeCameraFn)(PrototypeObject*, Camera*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* removeColliderFn)(PrototypeObject*, Collider*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* removeMeshRendererFn)(PrototypeObject*, MeshRenderer*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* removeRigidbodyFn)(PrototypeObject*, Rigidbody*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* removeScriptFn)(PrototypeObject*, Script*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* removeTransformFn)(PrototypeObject*, Transform*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* removeVehicleChasisFn)(PrototypeObject*, VehicleChasis*);

typedef void(PROTOTYPE_DYNAMIC_FN_CALL* logCameraFn)(PrototypeObject*, Camera*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* logColliderFn)(PrototypeObject*, Collider*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* logMeshRendererFn)(PrototypeObject*, MeshRenderer*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* logRigidbodyFn)(PrototypeObject*, Rigidbody*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* logScriptFn)(PrototypeObject*, Script*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* logTransformFn)(PrototypeObject*, Transform*);
typedef void(PROTOTYPE_DYNAMIC_FN_CALL* logVehicleChasisFn)(PrototypeObject*, VehicleChasis*);

static const MASK_TYPE PrototypeTraitTypeCount = 7;

static const MASK_TYPE PrototypeTraitTypeIndexCamera        = 0;
static const MASK_TYPE PrototypeTraitTypeIndexCollider      = 1;
static const MASK_TYPE PrototypeTraitTypeIndexMeshRenderer  = 2;
static const MASK_TYPE PrototypeTraitTypeIndexRigidbody     = 3;
static const MASK_TYPE PrototypeTraitTypeIndexScript        = 4;
static const MASK_TYPE PrototypeTraitTypeIndexTransform     = 5;
static const MASK_TYPE PrototypeTraitTypeIndexVehicleChasis = 6;

static MASK_TYPE PrototypeTraitTypeMaskCamera        = 1;
static MASK_TYPE PrototypeTraitTypeMaskCollider      = 2;
static MASK_TYPE PrototypeTraitTypeMaskMeshRenderer  = 4;
static MASK_TYPE PrototypeTraitTypeMaskRigidbody     = 8;
static MASK_TYPE PrototypeTraitTypeMaskScript        = 16;
static MASK_TYPE PrototypeTraitTypeMaskTransform     = 32;
static MASK_TYPE PrototypeTraitTypeMaskVehicleChasis = 64;

#define PrototypeTraitTypeAbsoluteStringCamera        "Camera"
#define PrototypeTraitTypeAbsoluteStringCollider      "Collider"
#define PrototypeTraitTypeAbsoluteStringMeshRenderer  "MeshRenderer"
#define PrototypeTraitTypeAbsoluteStringRigidbody     "Rigidbody"
#define PrototypeTraitTypeAbsoluteStringScript        "Script"
#define PrototypeTraitTypeAbsoluteStringTransform     "Transform"
#define PrototypeTraitTypeAbsoluteStringVehicleChasis "VehicleChasis"

#define PrototypeTraitTypeStringCamera        "PrototypeTraitTypeStringCamera"
#define PrototypeTraitTypeStringCollider      "PrototypeTraitTypeStringCollider"
#define PrototypeTraitTypeStringMeshRenderer  "PrototypeTraitTypeStringMeshRenderer"
#define PrototypeTraitTypeStringRigidbody     "PrototypeTraitTypeStringRigidbody"
#define PrototypeTraitTypeStringScript        "PrototypeTraitTypeStringScript"
#define PrototypeTraitTypeStringTransform     "PrototypeTraitTypeStringTransform"
#define PrototypeTraitTypeStringVehicleChasis "PrototypeTraitTypeStringVehicleChasis"

static const std::string PrototypeTraitTypeAbsoluteStringArray[] = { "Camera", "Collider",  "MeshRenderer", "Rigidbody",
                                                                     "Script", "Transform", "VehicleChasis"

};

static const std::unordered_map<std::string, MASK_TYPE> PrototypeTraitTypeMaskFromAbsoluteString = {
    { "Camera", PrototypeTraitTypeMaskCamera },
    { "Collider", PrototypeTraitTypeMaskCollider },
    { "MeshRenderer", PrototypeTraitTypeMaskMeshRenderer },
    { "Rigidbody", PrototypeTraitTypeMaskRigidbody },
    { "Script", PrototypeTraitTypeMaskScript },
    { "Transform", PrototypeTraitTypeMaskTransform },
    { "VehicleChasis", PrototypeTraitTypeMaskVehicleChasis }

};

struct PrototypeObject
{
    void      destroy();
    u32       id() const;
    MASK_TYPE traits() const;
    void      log();
    void      add(MASK_TYPE traitMask);
    bool      has(MASK_TYPE traitMask);
    void      remove(MASK_TYPE traitName);
    void      setParentNode(void* parentNode);
    void*     parentNode() const;
    void      onEdit();

    void addCameraTrait();
    void addColliderTrait();
    void addMeshRendererTrait();
    void addRigidbodyTrait();
    void addScriptTrait();
    void addTransformTrait();
    void addVehicleChasisTrait();

    void removeCameraTrait();
    void removeColliderTrait();
    void removeMeshRendererTrait();
    void removeRigidbodyTrait();
    void removeScriptTrait();
    void removeTransformTrait();
    void removeVehicleChasisTrait();

    void garbageCollectCameraTraitMemory();
    void garbageCollectColliderTraitMemory();
    void garbageCollectMeshRendererTraitMemory();
    void garbageCollectRigidbodyTraitMemory();
    void garbageCollectScriptTraitMemory();
    void garbageCollectTransformTraitMemory();
    void garbageCollectVehicleChasisTraitMemory();

    bool hasCameraTrait() const;
    bool hasColliderTrait() const;
    bool hasMeshRendererTrait() const;
    bool hasRigidbodyTrait() const;
    bool hasScriptTrait() const;
    bool hasTransformTrait() const;
    bool hasVehicleChasisTrait() const;

    Camera*        getCameraTrait() const;
    Collider*      getColliderTrait() const;
    MeshRenderer*  getMeshRendererTrait() const;
    Rigidbody*     getRigidbodyTrait() const;
    Script*        getScriptTrait() const;
    Transform*     getTransformTrait() const;
    VehicleChasis* getVehicleChasisTrait() const;

    static void to_json(nlohmann::json& j, const PrototypeObject& o);
    static void from_json(const nlohmann::json& j, PrototypeObject& o);

  private:
    friend struct MemoryPool<PrototypeObject, 100>;

    ~PrototypeObject();
    PrototypeObject();

    u32              _id;
    void*            _parentNode;
    PrototypeBitflag _traits;
};

struct PrototypeTraitSystemData
{
    PrototypeTraitSystemData();
    ~PrototypeTraitSystemData();

    u32                              _gid;
    MemoryPool<PrototypeObject, 100> _pool;
    std::vector<PrototypeObject*>    _objects;

    MemoryPool<Camera, 100>        _cameraPool;
    std::vector<Camera*>           _cameraVector;
    MemoryPool<Collider, 100>      _colliderPool;
    std::vector<Collider*>         _colliderVector;
    MemoryPool<MeshRenderer, 100>  _meshRendererPool;
    std::vector<MeshRenderer*>     _meshRendererVector;
    MemoryPool<Rigidbody, 100>     _rigidbodyPool;
    std::vector<Rigidbody*>        _rigidbodyVector;
    MemoryPool<Script, 100>        _scriptPool;
    std::vector<Script*>           _scriptVector;
    MemoryPool<Transform, 100>     _transformPool;
    std::vector<Transform*>        _transformVector;
    MemoryPool<VehicleChasis, 100> _vehicleChasisPool;
    std::vector<VehicleChasis*>    _vehicleChasisVector;

    std::unordered_map<u32, size_t> _cameraMap;
    std::unordered_map<u32, size_t> _colliderMap;
    std::unordered_map<u32, size_t> _meshRendererMap;
    std::unordered_map<u32, size_t> _rigidbodyMap;
    std::unordered_map<u32, size_t> _scriptMap;
    std::unordered_map<u32, size_t> _transformMap;
    std::unordered_map<u32, size_t> _vehicleChasisMap;

    std::unordered_map<u32, size_t> _cameraGarbageMap;
    std::unordered_map<u32, size_t> _colliderGarbageMap;
    std::unordered_map<u32, size_t> _meshRendererGarbageMap;
    std::unordered_map<u32, size_t> _rigidbodyGarbageMap;
    std::unordered_map<u32, size_t> _scriptGarbageMap;
    std::unordered_map<u32, size_t> _transformGarbageMap;
    std::unordered_map<u32, size_t> _vehicleChasisGarbageMap;

    addCameraFn        _addCameraCbFn;
    addColliderFn      _addColliderCbFn;
    addMeshRendererFn  _addMeshRendererCbFn;
    addRigidbodyFn     _addRigidbodyCbFn;
    addScriptFn        _addScriptCbFn;
    addTransformFn     _addTransformCbFn;
    addVehicleChasisFn _addVehicleChasisCbFn;

    reuseCameraFn        _reuseCameraCbFn;
    reuseColliderFn      _reuseColliderCbFn;
    reuseMeshRendererFn  _reuseMeshRendererCbFn;
    reuseRigidbodyFn     _reuseRigidbodyCbFn;
    reuseScriptFn        _reuseScriptCbFn;
    reuseTransformFn     _reuseTransformCbFn;
    reuseVehicleChasisFn _reuseVehicleChasisCbFn;

    removeCameraFn        _removeCameraCbFn;
    removeColliderFn      _removeColliderCbFn;
    removeMeshRendererFn  _removeMeshRendererCbFn;
    removeRigidbodyFn     _removeRigidbodyCbFn;
    removeScriptFn        _removeScriptCbFn;
    removeTransformFn     _removeTransformCbFn;
    removeVehicleChasisFn _removeVehicleChasisCbFn;

    logCameraFn        _logCameraCbFn;
    logColliderFn      _logColliderCbFn;
    logMeshRendererFn  _logMeshRendererCbFn;
    logRigidbodyFn     _logRigidbodyCbFn;
    logScriptFn        _logScriptCbFn;
    logTransformFn     _logTransformCbFn;
    logVehicleChasisFn _logVehicleChasisCbFn;
};

struct PrototypeTraitSystem
{
    static PrototypeObject*                     createObject();
    static void                                 clearObjects();
    static const std::vector<PrototypeObject*>& objects();

    static const std::vector<Camera*>&        cameraVector();
    static const std::vector<Collider*>&      colliderVector();
    static const std::vector<MeshRenderer*>&  meshRendererVector();
    static const std::vector<Rigidbody*>&     rigidbodyVector();
    static const std::vector<Script*>&        scriptVector();
    static const std::vector<Transform*>&     transformVector();
    static const std::vector<VehicleChasis*>& vehicleChasisVector();

    static const std::unordered_map<u32, size_t>& cameraMap();
    static const std::unordered_map<u32, size_t>& colliderMap();
    static const std::unordered_map<u32, size_t>& meshRendererMap();
    static const std::unordered_map<u32, size_t>& rigidbodyMap();
    static const std::unordered_map<u32, size_t>& scriptMap();
    static const std::unordered_map<u32, size_t>& transformMap();
    static const std::unordered_map<u32, size_t>& vehicleChasisMap();

    static const std::unordered_map<u32, size_t>& cameraGarbageMap();
    static const std::unordered_map<u32, size_t>& colliderGarbageMap();
    static const std::unordered_map<u32, size_t>& meshRendererGarbageMap();
    static const std::unordered_map<u32, size_t>& rigidbodyGarbageMap();
    static const std::unordered_map<u32, size_t>& scriptGarbageMap();
    static const std::unordered_map<u32, size_t>& transformGarbageMap();
    static const std::unordered_map<u32, size_t>& vehicleChasisGarbageMap();

    static void setCameraTraitAddCbFnPtr(addCameraFn cbfn);
    static void setColliderTraitAddCbFnPtr(addColliderFn cbfn);
    static void setMeshRendererTraitAddCbFnPtr(addMeshRendererFn cbfn);
    static void setRigidbodyTraitAddCbFnPtr(addRigidbodyFn cbfn);
    static void setScriptTraitAddCbFnPtr(addScriptFn cbfn);
    static void setTransformTraitAddCbFnPtr(addTransformFn cbfn);
    static void setVehicleChasisTraitAddCbFnPtr(addVehicleChasisFn cbfn);

    static void setCameraTraitReuseCbFnPtr(reuseCameraFn cbfn);
    static void setColliderTraitReuseCbFnPtr(reuseColliderFn cbfn);
    static void setMeshRendererTraitReuseCbFnPtr(reuseMeshRendererFn cbfn);
    static void setRigidbodyTraitReuseCbFnPtr(reuseRigidbodyFn cbfn);
    static void setScriptTraitReuseCbFnPtr(reuseScriptFn cbfn);
    static void setTransformTraitReuseCbFnPtr(reuseTransformFn cbfn);
    static void setVehicleChasisTraitReuseCbFnPtr(reuseVehicleChasisFn cbfn);

    static void setCameraTraitRemoveCbFnPtr(removeCameraFn cbfn);
    static void setColliderTraitRemoveCbFnPtr(removeColliderFn cbfn);
    static void setMeshRendererTraitRemoveCbFnPtr(removeMeshRendererFn cbfn);
    static void setRigidbodyTraitRemoveCbFnPtr(removeRigidbodyFn cbfn);
    static void setScriptTraitRemoveCbFnPtr(removeScriptFn cbfn);
    static void setTransformTraitRemoveCbFnPtr(removeTransformFn cbfn);
    static void setVehicleChasisTraitRemoveCbFnPtr(removeVehicleChasisFn cbfn);

    static void setCameraTraitLogCbFnPtr(logCameraFn cbfn);
    static void setColliderTraitLogCbFnPtr(logColliderFn cbfn);
    static void setMeshRendererTraitLogCbFnPtr(logMeshRendererFn cbfn);
    static void setRigidbodyTraitLogCbFnPtr(logRigidbodyFn cbfn);
    static void setScriptTraitLogCbFnPtr(logScriptFn cbfn);
    static void setTransformTraitLogCbFnPtr(logTransformFn cbfn);
    static void setVehicleChasisTraitLogCbFnPtr(logVehicleChasisFn cbfn);

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