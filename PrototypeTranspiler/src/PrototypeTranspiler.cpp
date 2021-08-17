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

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/CharUnits.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <nlohmann/json.hpp>

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace llvm;

// ------------------------------------------------------------------------------------------------
// MACROS SETTINGS
// ------------------------------------------------------------------------------------------------
#if defined(PROTOTYPE_TRANSPILER_OUTPUT_DIR)
#define PROTOTYPE_TRANSPILER_OUTPUT_PATH PROTOTYPE_TRANSPILER_OUTPUT_DIR
#else
#define PROTOTYPE_TRANSPILER_OUTPUT_PATH
#endif
#if defined(PROTOTYPE_META_DIR)
#define PROTOTYPE_META_PATH PROTOTYPE_META_DIR
#else
#define PROTOTYPE_META_PATH
#endif
// ------------------------------------------------------------------------------------------------

std::string
lowercased(std::string text)
{
    for (char& c : text) { c = tolower(c); }
    return text;
}

std::string
uppercased(std::string text)
{
    for (char& c : text) { c = toupper(c); }
    return text;
}

std::string
camelCasedAsVariable(std::string text)
{
    text[0] = tolower(text[0]);
    return text;
}

std::string
camelCasedAsFunction(std::string text)
{
    text[0] = toupper(text[0]);
    return text;
}

struct TraitStructureField
{
    std::string name;
    std::string attr;
    std::string type;
    std::string optType;
};

static void
to_json(nlohmann::json& j, const TraitStructureField& d)
{
    j = nlohmann::json{ { "name", d.name }, { "attr", d.attr }, { "type", d.type }, { "optType", d.optType } };
}

struct TraitStructure
{
    unsigned int                     line;
    unsigned int                     col;
    std::string                      filepath;
    std::string                      fileStem;
    std::string                      fileExtension;
    std::string                      name;
    std::string                      suffix;
    std::vector<TraitStructureField> fields;

    bool operator==(const TraitStructure& other) const { return name == other.name; };
    bool operator<(const TraitStructure& other) const { return name < other.name; };
};

static void
to_json(nlohmann::json& j, const TraitStructure& d)
{
    std::vector<nlohmann::json> jfields;
    for (const auto& field : d.fields) {
        nlohmann::json jj;
        to_json(jj, field);
        jfields.push_back(jj);
    }
    j = nlohmann::json{ { "line", d.line },
                        { "col", d.col },
                        { "filePath", d.filepath },
                        { "fileStem", d.fileStem },
                        { "fileExtension", d.fileExtension },
                        { "name",
                          { { "text", d.name },
                            { "functionName", camelCasedAsFunction(d.name) },
                            { "variableName", camelCasedAsVariable(d.name) } } },
                        { "suffix", d.suffix },
                        { "fields", jfields } };
}

namespace std {
template<>
struct hash<TraitStructure>
{
    std::size_t operator()(const TraitStructure& s) const noexcept
    {
        std::hash<std::string> hasher;
        return hasher(s.name);
    }
};
}

struct PrototypeTraitSystemData
{
    std::string                                     inputFilepath;
    std::string                                     inputFileStemName;
    std::string                                     inputFileExtension;
    std::string                                     outputFilepath;
    std::unordered_map<std::string, TraitStructure> traits;
};

static void
to_json(nlohmann::json& j, const PrototypeTraitSystemData& d)
{
    std::vector<nlohmann::json> jtraits;
    for (const auto& pair : d.traits) {
        const auto&    trait = pair.second;
        nlohmann::json jj;
        to_json(jj, trait);
        jtraits.push_back(jj);
    }
    j = nlohmann::json{ { "traits", jtraits } };
}

static PrototypeTraitSystemData prototypeTraitSystemData = {};

static std::unordered_set<std::string> TraitMarkers     = { "Trait" };
static std::unordered_set<std::string> AttributeMarkers = { "Attr" };

class DeclVisitor : public clang::RecursiveASTVisitor<DeclVisitor>
{
    clang::SourceManager& SourceManager;

  public:
    explicit DeclVisitor(clang::SourceManager& SourceManager)
      : SourceManager(SourceManager)
    {}

    void TraverseDecl(clang::NamedDecl* NamedDecl)
    {
        auto D = (clang::TagDecl*)NamedDecl;
        if (D && D->isStruct() && D->isCompleteDefinition()) {
            bool           isTraitStructure = false;
            TraitStructure traitStructure   = {};
            auto           RD               = (clang::RecordDecl*)(D);
            if (!RD || !RD->isCanonicalDecl()) { return; }
            std::string structureName = RD->getNameAsString();
            std::cout << structureName << "\n";
            if (prototypeTraitSystemData.traits.find(structureName) != prototypeTraitSystemData.traits.end()) { return; }
            traitStructure.name = structureName;
            for (const clang::Attr* attr : RD->attrs()) {
                auto annotateAttr = (const clang::AnnotateAttr*)(attr);
                if (annotateAttr) {
                    auto it = TraitMarkers.find(annotateAttr->getAnnotation().str());
                    if (it != TraitMarkers.end()) {
                        isTraitStructure             = true;
                        traitStructure.suffix        = *it;
                        traitStructure.filepath      = prototypeTraitSystemData.inputFilepath;
                        traitStructure.fileStem      = prototypeTraitSystemData.inputFileStemName;
                        traitStructure.fileExtension = prototypeTraitSystemData.inputFileExtension;
                        SourceLocation location      = RD->getLocation();
                        traitStructure.line          = SourceManager.getSpellingLineNumber(location);
                        traitStructure.col           = SourceManager.getSpellingColumnNumber(location);
                    }
                }
            }
            if (isTraitStructure) {
                for (const clang::FieldDecl* field : RD->fields()) {
                    std::vector<std::string> annotations;
                    for (const clang::Attr* attr : field->attrs()) {
                        auto annotateAttr = (const clang::AnnotateAttr*)(attr);
                        if (annotateAttr) { annotations.push_back(annotateAttr->getAnnotation().str()); }
                    }
                    if (field->getAccess() == clang::AccessSpecifier::AS_private || field->isAnonymousStructOrUnion()) continue;
                    TraitStructureField traitField = {};
                    std::string         theType    = field->getType().getAsString();
                    const std::string   stdoptstr  = "std::optional<";
                    const std::string   optstr     = "optional<";
                    std::string         innerType  = field->getCanonicalDecl()->getType().getAsString();
                    if (theType.substr(0, stdoptstr.size()) == stdoptstr || theType.substr(0, optstr.size()) == optstr) {
                        traitField.type    = innerType;
                        traitField.optType = innerType;
                    } else {
                        traitField.type    = innerType;
                        traitField.optType = "std::optional<" + innerType + ">";
                    }
                    if (!annotations.empty() && AttributeMarkers.find(annotations[0]) != AttributeMarkers.end()) {
                        traitField.attr = annotations[1];
                    } else {
                        traitField.attr = field->getNameAsString();
                    }
                    traitField.name = field->getNameAsString();
                    traitStructure.fields.push_back(traitField);
                }
                prototypeTraitSystemData.traits.insert({ structureName, traitStructure });
            }
        }
    }
};

class DeclFinder : public clang::ASTConsumer
{
  public:
    explicit DeclFinder(clang::SourceManager& SM)
      : SourceManager(SM)
      , Visitor(SM)
    {}

    void HandleTagDeclDefinition(clang::TagDecl* D) override {}

    void HandleTranslationUnit(ASTContext& Context) override
    {
        // Run the matchers when we have the whole TU parsed.
        auto Decls = Context.getTranslationUnitDecl()->decls();
        for (auto& Decl : Decls) {
            const auto& FileID = SourceManager.getFileID(Decl->getLocation());
            if (FileID != SourceManager.getMainFileID()) continue;
            Visitor.TraverseDecl((NamedDecl*)Decl);
        }
    }

  private:
    clang::SourceManager& SourceManager;
    DeclVisitor           Visitor;
};

class PrototypeTranspilerFrontendAction : public ASTFrontendAction
{
  public:
    PrototypeTranspilerFrontendAction() = default;

    void EndSourceFileAction() override
    {
        std::string          json_file_path = prototypeTraitSystemData.outputFilepath;
        StringRef            jsonFile(json_file_path.c_str());
        std::error_code      EC;
        llvm::raw_fd_ostream JOS(jsonFile, EC, llvm::sys::fs::F_None);
        if (EC) {
            llvm::errs() << "while opening '" << jsonFile << "': " << EC.message() << '\n';
            exit(1);
        }

        nlohmann::json j;
        to_json(j, prototypeTraitSystemData);
        std::stringstream ss;
        ss << std::setw(4) << j << std::endl;
        JOS << ss.str() << "\n";
    }

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(CompilerInstance& CI, StringRef file) override
    {
        return std::make_unique<DeclFinder>(CI.getSourceManager());
    }
};

int
main(int argc, const char** argv)
{
    //    for (int i = 0; i < argc; ++i) { std::cout << "arg[" << i << "] = " << argv[i] << "\n"; }
    {
        std::filesystem::path p = std::string(argv[2]);
        if (!std::filesystem::is_regular_file(p)) return 0;
        std::string stem      = p.filename().stem().string();
        std::string extension = p.filename().extension().string();
        if (stem == "PrototypeTraitSystem" || stem == "PrototypeTraitSystemTypes" ||
            !(extension == ".h" || extension == ".hh" || extension == ".hpp") || stem == "main")
            return 0;
        prototypeTraitSystemData.inputFileStemName  = stem;
        prototypeTraitSystemData.inputFileExtension = extension;
        prototypeTraitSystemData.inputFilepath      = p.string();
        prototypeTraitSystemData.outputFilepath     = std::string(PROTOTYPE_TRANSPILER_OUTPUT_PATH).append(stem).append(".json");
        if (!std::filesystem::exists(std::string(PROTOTYPE_TRANSPILER_OUTPUT_PATH))) {
            std::filesystem::create_directories(std::string(PROTOTYPE_TRANSPILER_OUTPUT_PATH));
        }
    }

    // fetch current mtime value of the file
    std::filesystem::create_directories(std::string(PROTOTYPE_META_PATH));
    int64_t currentMtime = std::filesystem::last_write_time(prototypeTraitSystemData.inputFilepath).time_since_epoch().count();
    // check if meta file exists
    std::filesystem::path jsonFile(std::string(PROTOTYPE_META_PATH).append("meta.json"));
    bool                  needsUpdate = false;
    nlohmann::json        jsonj;
    if (std::filesystem::exists(jsonFile)) {
        {
            std::ifstream jsonfs(jsonFile);
            jsonfs >> jsonj;
            jsonfs.close();
        }

        if (jsonj.find(prototypeTraitSystemData.inputFilepath) != jsonj.end()) {
            // field exitsts
            int64_t jsonFileMtime = jsonj[prototypeTraitSystemData.inputFilepath];
            if (jsonFileMtime != currentMtime) { needsUpdate = true; }
        } else {
            // field doesn't exist
            needsUpdate = true;
        }
    } else {
        needsUpdate = true;
    }
    jsonj[prototypeTraitSystemData.inputFilepath] = currentMtime;
    if (!needsUpdate) {
        std::cout << "[HOT] " << prototypeTraitSystemData.inputFilepath << "\n";
        return 0;
    }

    llvm::cl::OptionCategory MyToolCategory("PrototypeTranspiler options");
    CommonOptionsParser      optionsParser(argc, argv, MyToolCategory);
    ClangTool                Tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());
    int                      result = Tool.run(newFrontendActionFactory<PrototypeTranspilerFrontendAction>().get());

    {
        std::ofstream jsonfs(std::string(PROTOTYPE_META_PATH).append("meta.json"));
        jsonfs << jsonj;
        jsonfs.close();
    }

    std::cout << "[PROCESSED] " << prototypeTraitSystemData.inputFilepath << "\n";

    return result;
}