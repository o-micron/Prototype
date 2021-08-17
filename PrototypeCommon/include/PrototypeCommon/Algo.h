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

#include <unordered_map>
#include <unordered_set>
#include <vector>

template<typename TKey, typename TVal>
static inline void
PrototypeAlgoCopyNewValues(const std::unordered_map<TKey, TVal>& from, std::unordered_map<TKey, TVal>& to)
{
    std::unordered_map<TKey, TVal> old = to;
    to.clear();
    for (const auto& pair : from) {
        if (old.find(pair.first) == old.end()) {
            to[pair.first] = pair.second;
        } else {
            to[pair.first] = old[pair.first];
        }
    }
}

template<typename T>
static inline void
PrototypeAlgoCopyNewValues(const std::unordered_set<T>& from, std::unordered_set<T>& to)
{
    to = from;
}

template<typename T>
static inline void
PrototypeAlgoCopyNewValues(const std::vector<T>& from, std::vector<T>& to)
{
    to = from;
}