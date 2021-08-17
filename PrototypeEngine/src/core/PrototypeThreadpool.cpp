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

#include "PrototypeThreadpool.h"

PrototypeThreadpool::PrototypeThreadpool(u8 numThreads)
{
    _numThreads = numThreads;
    _numBusyThreads.store(0);
    _stopped.store(false);
    startAll();
}

PrototypeThreadpool::~PrototypeThreadpool() { stopAll(); }

void
PrototypeThreadpool::submit(ThreadPoolTask task) noexcept
{
    {
        std::unique_lock<std::mutex> lock(_eventMutex);
        _tasks.emplace(std::move(task));
    }
    _eventVar.notify_one();
}

bool
PrototypeThreadpool::hasWork() noexcept
{
    std::unique_lock<std::mutex> lock(_eventMutex);
    return !_tasks.empty() || _numBusyThreads > 0;
}

u8
PrototypeThreadpool::remainingTasks() noexcept
{
    std::unique_lock<std::mutex> lock(_eventMutex);
    return static_cast<u8>(_tasks.size());
}

void
PrototypeThreadpool::waitForWork() noexcept
{
    while (hasWork()) {}
}

void
PrototypeThreadpool::startAll()
{
    for (size_t i = 0; i < _numThreads; ++i) {
        _threads.emplace_back([=]() {
            while (true) {
                ThreadPoolTask task;

                {
                    std::unique_lock<std::mutex> lock(_eventMutex);
                    _eventVar.wait(lock, [=]() { return _stopped.load() || !_tasks.empty(); });

                    if (_stopped.load()) { break; }

                    task = std::move(_tasks.front());
                    _tasks.pop();
                    _numBusyThreads.store(_numBusyThreads.load() + 1);
                }

                task();

                {
                    std::unique_lock<std::mutex> lock(_eventMutex);
                    _numBusyThreads.store(_numBusyThreads.load() - 1);
                }
            }
        });
    }
}

void
PrototypeThreadpool::stopAll() noexcept
{
    {
        std::unique_lock<std::mutex> lock(_eventMutex);
        _stopped.store(true);
    }

    _eventVar.notify_all();

    for (std::thread& thread : _threads) { thread.join(); }
}