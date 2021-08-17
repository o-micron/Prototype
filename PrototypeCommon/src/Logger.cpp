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

#include "../include/PrototypeCommon/Logger.h"

#include "../include/PrototypeCommon/IO.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <fmt/printf.h>

#include <stdarg.h>
#include <stdio.h>
#include <string>

PrototypeLoggerData* PrototypeLogger::_data = nullptr;

void
PrototypeLogger::print(const char* status, const char* message, va_list args)
{
#ifndef PROTOTYPE_TARGET_RELEASE_BUILD
    vprintf(std::string(status).append(message).append("\n").c_str(), args);
#endif
}

void
PrototypeLogger::trace(const char* message, ...)
{
    va_list args;
    va_start(args, message);
    PrototypeLogger::print("[TRACE] ", message, args);
    va_end(args);
}

void
PrototypeLogger::log(const char* file, int line, const char* message)
{
    std::string f(file);
    std::replace(f.begin(), f.end(), '\\', '/');
    _data->logs.emplace_front(f, line, std::string(message));
    if (_data->logs.size() > _data->maxLogsCount) { _data->logs.pop_back(); }
}

void
PrototypeLogger::warn(const char* message, ...)
{
    va_list args;
    va_start(args, message);
    PrototypeLogger::print("[WARN] ", message, args);
    va_end(args);
}

void
PrototypeLogger::error(const char* message, ...)
{
    va_list args;
    va_start(args, message);
    PrototypeLogger::print("[ERROR] ", message, args);
    va_end(args);
}

void
PrototypeLogger::fatal(const char* message, ...)
{
    va_list args;
    va_start(args, message);
    PrototypeLogger::print("[FATAL] ", message, args);
    va_end(args);

    PROTOTYPE_ASSERT(false)
}

void
PrototypeLogger::dump(const char* path, const char* data)
{
    std::string writeData = std::string(data);
    PrototypeIo::writeFileBlock(path, writeData);
}

PrototypeLoggerData*
PrototypeLogger::data()
{
    return _data;
}

void
PrototypeLogger::setData(PrototypeLoggerData* data)
{
    _data = data;
}