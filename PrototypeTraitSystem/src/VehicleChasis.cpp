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

#include "../include/PrototypeTraitSystem/VehicleChasis.h"

onEditDispatchHandlerFn VehicleChasis::_onEditDispatchHandler = nullptr;

void
VehicleChasis::setVehicleRef(void* vehicleRef)
{
    _vehicleRef = vehicleRef;
}

void
VehicleChasis::setWheelFRObject(PrototypeObject* wheelFRObject)
{
    _wheelFRObject = wheelFRObject;
}

void
VehicleChasis::setWheelFLObject(PrototypeObject* wheelFLObject)
{
    _wheelFLObject = wheelFLObject;
}

void
VehicleChasis::setWheelBRObject(PrototypeObject* wheelBRObject)
{
    _wheelBRObject = wheelBRObject;
}

void
VehicleChasis::setWheelBLObject(PrototypeObject* wheelBLObject)
{
    _wheelBLObject = wheelBLObject;
}

void
VehicleChasis::setVehicleIndex(size_t vehicleIndex)
{
    _vehicleIndex = vehicleIndex;
}

void
VehicleChasis::setAccelerationPedal(bool state)
{
    _accelerationPedal = state;
}

void
VehicleChasis::setBrakePedal(bool state)
{
    _brakePedal = state;
}

void
VehicleChasis::setRightSteer(bool state)
{
    _rightSteer = state;
}

void
VehicleChasis::setLeftSteer(bool state)
{
    _leftSteer = state;
}

void*
VehicleChasis::vehicleRef()
{
    return _vehicleRef;
}

PrototypeObject*
VehicleChasis::wheelFRObject()
{
    return _wheelFRObject;
}

PrototypeObject*
VehicleChasis::wheelFLObject()
{
    return _wheelFLObject;
}

PrototypeObject*
VehicleChasis::wheelBRObject()
{
    return _wheelBRObject;
}

PrototypeObject*
VehicleChasis::wheelBLObject()
{
    return _wheelBLObject;
}

size_t
VehicleChasis::vehicleIndex() const
{
    return _vehicleIndex;
}

bool
VehicleChasis::accelerationPedal() const
{
    return _accelerationPedal;
}

bool
VehicleChasis::brakePedal() const
{
    return _brakePedal;
}

bool
VehicleChasis::rightSteer() const
{
    return _rightSteer;
}

bool
VehicleChasis::leftSteer() const
{
    return _leftSteer;
}

PrototypeObject*
VehicleChasis::object()
{
    return _object;
}

void
VehicleChasis::setOnEditDispatchHandler(onEditDispatchHandlerFn onEditDispatchHandler)
{
    _onEditDispatchHandler = onEditDispatchHandler;
}

void
VehicleChasis::onEditDispatch(PrototypeObject* o)
{
    if (_onEditDispatchHandler) { _onEditDispatchHandler(o); }
}

void
VehicleChasis::to_json(nlohmann::json& j, const VehicleChasis& vch)
{
    const char* field_name = "name";

    j[field_name] = PROTOTYPE_STRINGIFY(VehicleChasis);
}

void
VehicleChasis::from_json(const nlohmann::json& j, VehicleChasis& vch, PrototypeObject* o)
{
    vch._accelerationPedal = false;
    vch._brakePedal        = false;
    vch._rightSteer        = false;
    vch._leftSteer         = false;
    vch._object            = o;
}