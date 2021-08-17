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

#include "../include/PrototypeCommon/Bitflag.h"

extern PrototypeBitflag
PrototypeBitflagNew()
{
    return PrototypeBitflag{ 0 };
}

extern PrototypeBitflag
PrototypeBitflagFrom(u64 features)
{
    return PrototypeBitflag{ features };
}

extern bool
PrototypeBitflagIs(const PrototypeBitflag& self, u64 features)
{
    return self.features == features;
}

extern bool
PrototypeBitflagHas(const PrototypeBitflag& self, u64 features)
{
    return (self.features & features) == features;
}

extern bool
PrototypeBitflagHasAny(const PrototypeBitflag& self, u64 features)
{
    return (self.features & features) > 0;
}

extern void
PrototypeBitflagAdd(PrototypeBitflag& self, u64 features)
{
    self.features |= features;
}

extern void
PrototypeBitflagRemove(PrototypeBitflag& self, u64 features)
{
    self.features &= ~features;
}