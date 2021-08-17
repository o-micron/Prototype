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
#include "Types.h"

#include <limits>
#include <string>
#include <time.h>
#include <vector>

class PrototypeIo
{
  private:
    PrototypeIo()  = delete;
    ~PrototypeIo() = delete;

  public:
    static void                     createDirectory(const char* name);
    static void                     createNewCppPluginProject(const char* name);
    static void                     createNewRustPluginProject(const char* name);
    static bool                     checkIfPluginProjectExists(const char* name);
    static bool                     isFile(const char* path);
    static bool                     isDirectory(const char* path);
    static bool                     readFileLines(const char* path, std::vector<std::string>& lines);
    static bool                     readFileText(const char* path, std::string& text);
    static bool                     readFileBlock(const char* path, std::string& text);
    static void                     writeFileBlock(const char* path, std::string& text);
    static void                     copyFile(const char* fromPath, const char* toPath);
    static time_t                   filestamp(const std::string& filepath);
    static std::vector<std::string> listFiles(const std::string& filepath, u8 levelLimit);
    static std::vector<std::string> listFolders(const std::string& filepath, u8 levelLimit);
};