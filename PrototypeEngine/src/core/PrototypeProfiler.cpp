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

#if defined(PROTOTYPE_ENABLE_PROFILER)

#include "PrototypeProfiler.h"

#include <numeric>

PrototypeProfiler::PrototypeProfiler()
{
    std::iota(_xaxis.begin(), _xaxis.end(), 0);
    _timeline.time = 0;
}

void
PrototypeProfiler::addTimelineItem(const std::string tag)
{
    if (_timeline.items.find(tag) != _timeline.items.end()) { return; }

    PrototypeProfilerTimelineItem item = {};
    item.color                         = { 255.0f, 255.0f, 255.0f, 255.0f };
    item.isVisisble                    = true;
    item.isOpen                        = false;
    item.name                          = tag;
    std::fill(item.values.begin(), item.values.end(), 0);

    _timeline.items.insert({ tag, std::move(item) });
}

void
PrototypeProfiler::advanceTimeline()
{
    ++_timeline.time;
    _timeline.time %= PROTOTYPE_PROFILER_BUFFER_LENGTH;
    for (auto& pair : _timeline.items) { pair.second.values[_timeline.time] = 0; }
}

void
PrototypeProfiler::markTimelineItem(const std::string tag, u32 value)
{
    _timeline.items[tag].values[_timeline.time] = value;
}

void
PrototypeProfiler::setTimelineItemColor(const std::string tag, PrototypeVec4 color)
{
    _timeline.items[tag].color = color;
}

void
PrototypeProfiler::setTimelineItemVisible(const std::string tag, bool status)
{
    _timeline.items[tag].isVisisble = status;
}

std::array<u32, PROTOTYPE_PROFILER_BUFFER_LENGTH>&
PrototypeProfiler::xaxis()
{
    return _xaxis;
}

std::unordered_map<std::string, PrototypeProfilerTimelineItem>&
PrototypeProfiler::getTimelineItems()
{
    return _timeline.items;
}

#endif // #if defined(PROTOTYPE_ENABLE_PROFILER)