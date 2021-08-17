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

#include <filesystem>
#include <fstream>
#include <streambuf>
#include <string>
#include <sstream>

#include <jinja2cpp/binding/nlohmann_json.h>
#include <jinja2cpp/template.h>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

// ------------------------------------------------------------------------------------------------
// MACROS SETTINGS
// ------------------------------------------------------------------------------------------------
#if defined(PROTOTYPE_GENERATOR_DESCRIPTORS_DIR)
#define PROTOTYPE_GENERATOR_DESCRIPTORS_PATH PROTOTYPE_GENERATOR_DESCRIPTORS_DIR
#else
#define PROTOTYPE_GENERATOR_DESCRIPTORS_PATH ""
#endif
#if defined(PROTOTYPE_GENERATOR_TEMPLATES_DIR)
#define PROTOTYPE_GENERATOR_TEMPLATES_PATH PROTOTYPE_GENERATOR_TEMPLATES_DIR
#else
#define PROTOTYPE_GENERATOR_TEMPLATES_PATH ""
#endif
#if defined(PROTOTYPE_GENERATOR_OUTPUT_HEADER_DIR)
#define PROTOTYPE_GENERATOR_OUTPUT_HEADER_PATH PROTOTYPE_GENERATOR_OUTPUT_HEADER_DIR
#else
#define PROTOTYPE_GENERATOR_OUTPUT_HEADER_PATH
#endif
#if defined(PROTOTYPE_GENERATOR_OUTPUT_SRC_DIR)
#define PROTOTYPE_GENERATOR_OUTPUT_SRC_PATH PROTOTYPE_GENERATOR_OUTPUT_SRC_DIR
#else
#define PROTOTYPE_GENERATOR_OUTPUT_SRC_PATH
#endif
#if defined(PROTOTYPE_META_DIR)
#define PROTOTYPE_META_PATH PROTOTYPE_META_DIR
#else
#define PROTOTYPE_META_PATH
#endif
// ------------------------------------------------------------------------------------------------

int
main(int argc, char const* argv[])
{
    nlohmann::json        jsonj;
    std::filesystem::path jsonFile(std::string(PROTOTYPE_META_PATH).append("meta.json"));
    {
        std::ifstream jsonfs(jsonFile);
        jsonfs >> jsonj;
        jsonfs.close();
    }

    nlohmann::json context;

    bool needsUpdate = false;

    std::string path = PROTOTYPE_GENERATOR_DESCRIPTORS_PATH;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        bool localNeedsUpdate = false;
        if (!entry.is_regular_file()) { continue; }
        auto path = entry.path();
        if (!path.has_extension()) { continue; }
        if (path.extension() == ".json") {
            nlohmann::json jfile;
            std::ifstream  ifs(path.string());
            if (!ifs.is_open()) { continue; }
            ifs >> jfile;
            ifs.close();
            context["traits"].push_back(jfile["traits"][0]);
            int64_t currentFileMtime = std::filesystem::last_write_time(path.string()).time_since_epoch().count();
            if (jsonj.find(path.string()) == jsonj.end()) {
                localNeedsUpdate = true;
            } else {
                int64_t jsonFileMtime = jsonj[path.string()];
                if (jsonFileMtime != currentFileMtime) { localNeedsUpdate = true; }
            }
            jsonj[path.string()] = currentFileMtime;
            if (!localNeedsUpdate) {
                std::cout << "[HOT] " << path.string() << "\n";
            } else {
                std::cout << "[PROCESSED] " << path.string() << "\n";
            }
            needsUpdate |= localNeedsUpdate;
        }
    }

    const std::vector<std::string> expectedFiles = {
        std::string(PROTOTYPE_GENERATOR_TEMPLATES_PATH).append("PrototypeTraitSystemTypes.h"),
        std::string(PROTOTYPE_GENERATOR_TEMPLATES_PATH).append("PrototypeTraitSystem.h"),
        std::string(PROTOTYPE_GENERATOR_TEMPLATES_PATH).append("PrototypeTraitSystem.cpp"),
        std::string(PROTOTYPE_GENERATOR_OUTPUT_HEADER_PATH).append("PrototypeTraitSystemTypes.h"),
        std::string(PROTOTYPE_GENERATOR_OUTPUT_HEADER_PATH).append("PrototypeTraitSystem.h"),
        std::string(PROTOTYPE_GENERATOR_OUTPUT_SRC_PATH).append("PrototypeTraitSystem.cpp")
    };

    for (const auto& file : expectedFiles) {
        std::filesystem::path fp(file);
        if (!std::filesystem::exists(fp)) {
            needsUpdate = true;
            std::cout << "[PROCESSED] " << fp.string() << "\n";
            continue;
        }
        int64_t currentMtime = std::filesystem::last_write_time(fp).time_since_epoch().count();
        if (jsonj.find(fp.string()) == jsonj.end()) {
            needsUpdate        = true;
            jsonj[fp.string()] = currentMtime;
            std::cout << "[PROCESSED] " << fp.string() << "\n";
        } else {
            int64_t fpMtime = jsonj[fp.string()];
            if (fpMtime != currentMtime) {
                needsUpdate        = true;
                jsonj[fp.string()] = currentMtime;
                std::cout << "[PROCESSED] " << fp.string() << "\n";
            } else {
                std::cout << "[HOT] " << fp.string() << "\n";
            }
        }
    }

    if (!needsUpdate) {
        {
            std::ofstream jsonfs(std::string(PROTOTYPE_META_PATH).append("meta.json"));
            jsonfs << jsonj;
            jsonfs.close();
        }
        return 0;
    }

    context["count"] = context["traits"].size();

    std::stringstream ss;
    ss << context << "\n";

    jinja2::ValuesMap vmap;
    vmap.insert_or_assign("data", jinja2::Reflect<nlohmann::json>(std::move(context)));

    const std::vector<std::pair<std::string, std::string>> exportedFiles = {
        { PROTOTYPE_GENERATOR_TEMPLATES_PATH "PrototypeTraitSystemTypes.h",
          PROTOTYPE_GENERATOR_OUTPUT_HEADER_PATH "PrototypeTraitSystemTypes.h" },
        { PROTOTYPE_GENERATOR_TEMPLATES_PATH "PrototypeTraitSystem.h",
          PROTOTYPE_GENERATOR_OUTPUT_HEADER_PATH "PrototypeTraitSystem.h" },
        { PROTOTYPE_GENERATOR_TEMPLATES_PATH "PrototypeTraitSystem.cpp",
          PROTOTYPE_GENERATOR_OUTPUT_SRC_PATH "PrototypeTraitSystem.cpp" }
    };

    for (const auto& pair : exportedFiles) {
        jinja2::Template templ;
        templ.LoadFromFile(pair.first);
        std::ofstream ofs(pair.second);
        templ.Render(ofs, vmap);
        ofs.close();
    }

    {
        std::ofstream jsonfs(std::string(PROTOTYPE_META_PATH).append("meta.json"));
        jsonfs << jsonj;
        jsonfs.close();
    }

    return 0;
}
