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

struct Attachable(Trait) VehicleChasis
{
    void setVehicleRef(void* vehicleRef);
    void setWheelFRObject(PrototypeObject * wheelFRObject);
    void setWheelFLObject(PrototypeObject * wheelFLObject);
    void setWheelBRObject(PrototypeObject * wheelBRObject);
    void setWheelBLObject(PrototypeObject * wheelBLObject);
    void setVehicleIndex(size_t vehicleIndex);
    void setAccelerationPedal(bool state);
    void setBrakePedal(bool state);
    void setRightSteer(bool state);
    void setLeftSteer(bool state);

    void*            vehicleRef();
    PrototypeObject* wheelFRObject();
    PrototypeObject* wheelFLObject();
    PrototypeObject* wheelBRObject();
    PrototypeObject* wheelBLObject();
    size_t           vehicleIndex() const;
    bool             accelerationPedal() const;
    bool             brakePedal() const;
    bool             rightSteer() const;
    bool             leftSteer() const;

    PrototypeObject* object();
    static void      setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler);
    static void      onEditDispatch(PrototypeObject * o);
    static void      to_json(nlohmann::json & j, const VehicleChasis& vch);
    static void      from_json(const nlohmann::json& j, VehicleChasis& vch, PrototypeObject* o);

  private:
    friend struct PrototypeObject;
    PrototypeObject*               _object;
    static onEditDispatchHandlerFn _onEditDispatchHandler;
    void*                          _vehicleRef;
    PrototypeObject*               _wheelFRObject;
    PrototypeObject*               _wheelFLObject;
    PrototypeObject*               _wheelBRObject;
    PrototypeObject*               _wheelBLObject;
    size_t                         _vehicleIndex;
    bool                           _accelerationPedal;
    bool                           _brakePedal;
    bool                           _rightSteer;
    bool                           _leftSteer;
};
