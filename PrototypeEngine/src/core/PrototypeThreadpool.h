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

#include "../../include/PrototypeEngine/PrototypeEngineApi.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

typedef std::function<void()> ThreadPoolTask;

struct PrototypeThreadpool
{
    explicit PrototypeThreadpool(u8 numThreads);
    ~PrototypeThreadpool();
    void submit(ThreadPoolTask task) noexcept;
    bool hasWork() noexcept;
    u8   remainingTasks() noexcept;
    void waitForWork() noexcept;

  private:
    void startAll();
    void stopAll() noexcept;

    u8                         _numThreads;
    std::atomic_uint8_t        _numBusyThreads;
    std::atomic_bool           _stopped;
    std::condition_variable    _eventVar;
    std::mutex                 _eventMutex;
    std::vector<std::thread>   _threads;
    std::queue<ThreadPoolTask> _tasks;
};
