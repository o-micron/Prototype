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

#include "../include/PrototypeCommon/IO.h"
#include "../include/PrototypeCommon/Definitions.h"
#include "../include/PrototypeCommon/Logger.h"

#include <filesystem>
namespace stdfs = std::filesystem;
#include <fstream>
#include <regex>

#if defined(_WIN32)
#include <Windows.h>
#endif

static inline bool
templateReplace(std::string& str, const std::string& from, const std::string& to)
{
    size_t index = 0;
    while (true) {
        index = str.find(from, index);
        if (index == std::string::npos) break;
        str.replace(index, from.length(), to);
        index += from.length();
    }
    return true;
}

void
PrototypeIo::createDirectory(const char* name)
{
    stdfs::create_directories(name);
}

void
PrototypeIo::createNewCppPluginProject(const char* name)
{
    std::string cmakeFile =
#include "../../PrototypeEngine/templates/plugins/cpp/cmake/windows/CMakeLists.txt"
      ;

    std::string Code =
#include "../../PrototypeEngine/templates/plugins/cpp/src/lib.cpp"
      ;

    const std::string templateKeyword = "{{ PrototypePluginProjectName }}";

    std::vector<std::tuple<std::string, std::string, std::string>> files = { { "/cmake/windows", "/CMakeLists.txt", cmakeFile },
                                                                             { "/src", "/lib.cpp", Code } };

    for (auto& t : files) {
        std::string dir = std::string(PROTOTYPE_CPP_PLUGINS_PATH("")).append(name).append(std::get<0>(t));
        std::filesystem::create_directories(dir);
        std::string filepath = dir.append(std::get<1>(t));
        templateReplace(std::get<2>(t), templateKeyword, name);
        writeFileBlock(filepath.c_str(), std::get<2>(t));
    }
}

void
PrototypeIo::createNewRustPluginProject(const char* name)
{
    std::string cmakeFile =
#include "../../PrototypeEngine/templates/plugins/rust/CMakeLists.txt"
      ;
    std::string lib =
#include "../../PrototypeEngine/templates/plugins/rust/src/lib.rs"
      ;
    std::string gitignore =
#include "../../PrototypeEngine/templates/plugins/rust/.gitignore"
      ;
    std::string buildrs =
#include "../../PrototypeEngine/templates/plugins/rust/build.rs"
      ;
    std::string cargoLock =
#include "../../PrototypeEngine/templates/plugins/rust/Cargo.lock"
      ;
    std::string cargoToml =
#include "../../PrototypeEngine/templates/plugins/rust/Cargo.toml"
      ;
    std::string wrapper =
#include "../../PrototypeEngine/templates/plugins/rust/wrapper.h"
      ;

    const std::string                                              templateKeyword = "{{ PrototypePluginProjectName }}";
    std::vector<std::tuple<std::string, std::string, std::string>> files           = {
        { "", "/CMakeLists.txt", cmakeFile }, { "/src", "/lib.rs", lib },       { "", "/.gitignore", gitignore },
        { "", "/build.rs", buildrs },         { "", "/cargo.lock", cargoLock }, { "", "/cargo.toml", cargoToml },
        { "", "/wrapper.h", wrapper }
    };

    for (auto& t : files) {
        std::string dir = std::string(PROTOTYPE_RUST_PLUGINS_PATH("")).append(name).append(std::get<0>(t));
        std::filesystem::create_directories(dir);
        std::string filepath = dir.append(std::get<1>(t));
        templateReplace(std::get<2>(t), templateKeyword, name);
        writeFileBlock(filepath.c_str(), std::get<2>(t));
    }
}

bool
PrototypeIo::checkIfPluginProjectExists(const char* name)
{
    std::string cppDir = std::string(PROTOTYPE_CPP_PLUGINS_PATH("")).append(name);
    if (std::filesystem::exists(cppDir)) { return true; }
    std::string rustDir = std::string(PROTOTYPE_RUST_PLUGINS_PATH("")).append(name);
    if (std::filesystem::exists(rustDir)) { return true; }
    return false;
}

bool
PrototypeIo::isFile(const char* path)
{
    return stdfs::is_regular_file(stdfs::path(path));
}

bool
PrototypeIo::isDirectory(const char* path)
{
    return stdfs::is_directory(stdfs::path(path));
}

bool
PrototypeIo::readFileLines(const char* path, std::vector<std::string>& lines)
{
    std::ifstream file(path);
    if (!file.is_open()) { return false; }
    for (std::string line; std::getline(file, line);) lines.push_back(line);
    file.close();
    return true;
}

bool
PrototypeIo::readFileText(const char* path, std::string& text)
{
    std::ifstream file(path);
    if (!file.is_open()) { return false; }
    text = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return true;
}

bool
PrototypeIo::readFileBlock(const char* path, std::string& text)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) { return false; }
    size_t fileSize = (size_t)file.tellg();
    text.resize(fileSize);
    file.seekg(0);
    file.read((char*)text.data(), fileSize);
    file.close();
    return true;
}

void
PrototypeIo::writeFileBlock(const char* path, std::string& text)
{
    std::ofstream file(path, std::ios::ate | std::ios::binary);
    file << text;
    file.close();
}

void
PrototypeIo::copyFile(const char* fromPath, const char* toPath)
{
    std::error_code ec;
    std::filesystem::copy(fromPath, toPath, std::filesystem::copy_options::overwrite_existing, ec);
    PrototypeLogger::error("Copying %s to %s :[%s]", fromPath, toPath, ec.message().c_str());
}

time_t
PrototypeIo::filestamp(const std::string& filepath)
{
    const stdfs::path p(filepath);
    if (!stdfs::exists(p)) return 0;
    struct _stat result = {};
    _stat(filepath.c_str(), &result);
    return result.st_mtime;
}

std::vector<std::string>
PrototypeIo::listFiles(const std::string& filepath, u8 levelLimit)
{
    if (!stdfs::exists(filepath)) { return {}; }
    std::vector<std::string> files;
    for (const auto& entry : stdfs::directory_iterator(filepath)) {
        if (stdfs::is_regular_file(entry)) {
            files.push_back(entry.path().string());
        } else if (stdfs::is_directory(entry)) {
            if (levelLimit > 0) {
                auto children = listFiles(entry.path().string(), levelLimit - 1);
                files.insert(files.end(), children.begin(), children.end());
            }
        }
    }
    return files;
}

std::vector<std::string>
PrototypeIo::listFolders(const std::string& filepath, u8 levelLimit)
{
    std::vector<std::string> folders;
    for (const auto& entry : stdfs::directory_iterator(filepath)) {
        if (stdfs::is_directory(entry)) {
            folders.push_back(entry.path().string());
            if (levelLimit > 0) {
                auto children = listFolders(entry.path().string(), levelLimit - 1);
                folders.insert(folders.end(), children.begin(), children.end());
            }
        }
    }
    return folders;
}