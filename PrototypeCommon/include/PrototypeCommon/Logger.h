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

#include "Definitions.h"

#include <deque>
#include <map>
#include <string>

struct PrototypeLoggerReferencedLog
{
    PrototypeLoggerReferencedLog(std::string filepath, int line, std::string text)
      : filepath(std::move(filepath))
      , line(line)
      , text(std::move(text))
    {}
    std::string filepath;
    int         line;
    std::string text;
};

struct PrototypeLoggerData
{
    PrototypeLoggerData()
      : maxLogsCount(1024)
    {}
    std::deque<PrototypeLoggerReferencedLog> logs;
    size_t                                   maxLogsCount;
};

struct PrototypeLogger
{
    PrototypeLogger()  = delete;
    ~PrototypeLogger() = delete;

    static void trace(const char* message, ...);
    static void log(const char* file, int line, const char* message);
    static void warn(const char* message, ...);
    static void error(const char* message, ...);
    static void fatal(const char* message, ...);
    static void dump(const char* path, const char* data);

    static PrototypeLoggerData* data();
    static void                 setData(PrototypeLoggerData* data);

  private:
    static void print(const char* status, const char* message, va_list args);

    static PrototypeLoggerData* _data;
};