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

#if defined(PROTOTYPE_ENABLE_PROFILER)

#include "../../include/PrototypeEngine/PrototypeEngineApi.h"

#include <PrototypeCommon/Maths.h>

#include <array>
#include <unordered_map>
#include <vector>

#define PROTOTYPE_PROFILER_BUFFER_LENGTH 512

struct PrototypeProfilerTimelineItem
{
    std::array<u32, PROTOTYPE_PROFILER_BUFFER_LENGTH> values;
    PrototypeVec4                                     color;
    std::string                                       name;
    bool                                              isVisisble;
    bool                                              isOpen;
};

struct PrototypeProfilerTimeline
{
    void                                                           next();
    void                                                           mark(const std::string tag, u32 value);
    std::unordered_map<std::string, PrototypeProfilerTimelineItem> items;
    size_t                                                         time;
};

struct PrototypeProfiler
{
    PrototypeProfiler();
    void addTimelineItem(const std::string tag);
    void advanceTimeline();
    void markTimelineItem(const std::string tag, u32 value);
    void setTimelineItemColor(const std::string tag, PrototypeVec4 color);
    void setTimelineItemVisible(const std::string tag, bool status);

    std::array<u32, PROTOTYPE_PROFILER_BUFFER_LENGTH>&              xaxis();
    std::unordered_map<std::string, PrototypeProfilerTimelineItem>& getTimelineItems();

  private:
    std::array<u32, PROTOTYPE_PROFILER_BUFFER_LENGTH> _xaxis;
    PrototypeProfilerTimeline                         _timeline;
};

#endif // #if defined(PROTOTYPE_ENABLE_PROFILER)