cmake_minimum_required(VERSION 3.1)
project(PrototypeTranspiler VERSION 1.0 DESCRIPTION "PrototypeTranspiler" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)

set(PROTOTYPE_CMAKE_ASSETS_LOCAL_DIR "\"${CMAKE_BINARY_DIR}/bin/assets/\"")
set(PROTOTYPE_CMAKE_ASSETS_DIR "\"assets/\"")
set(PROTOTYPE_PLUGINS_DIR "\"../../../PrototypePlugins/\"")

# FIND PACKAGES
set(LLVM_MAIN_DIR ${CMAKE_SOURCE_DIR}/PrototypeCompiler/llvm-project)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# VARIABLES
set(TARGET_INCLUDE_DIRS
    ${LLVM_MAIN_DIR}/llvm/include
    ${LLVM_MAIN_DIR}/build/include
    ${LLVM_MAIN_DIR}/clang/include
    ${LLVM_MAIN_DIR}/build/tools/clang/include
    ${CMAKE_SOURCE_DIR}/PrototypeDependencies/nlohmann/include
)
set(TARGET_LINK_DIRS
    ${LLVM_MAIN_DIR}/build/${CMAKE_BUILD_TYPE}/lib
)

set(TARGET_LINK_LIBS
    LLVMX86AsmParser        # MC, MCParser, Support, X86Desc, X86Info
    LLVMX86Desc             # MC, Support, X86AsmPrinter, X86Info
    LLVMX86Info             # MC, Support, Target
    LLVMipo
    LLVMScalarOpts
    LLVMInstCombine
    LLVMTransformUtils
    LLVMAnalysis
    LLVMTarget
    LLVMOption              # Support
    LLVMMCParser            # MC, Support
    LLVMMC                  # Object, Support
    LLVMObject              # BitReader, Core, Support
    LLVMBitReader           # Core, Support
    LLVMCore                # Support
    LLVMSupport
    version
    clangFrontend
    clangSerialization
    clangDriver
    clangParse
    clangRewriteFrontend
    clangStaticAnalyzerFrontend
    clangSema
    clangAnalysis
    clangEdit
    clangAST
    clangLex
    clangBasic
    clangTooling
    clangRewrite
    clangASTMatchers
    clangToolingCore
)
    
add_executable(PrototypeTranspiler ${CMAKE_CURRENT_SOURCE_DIR}/../../src/PrototypeTranspiler.cpp)

set(PROTOTYPE_TRANSPILER_OUTPUT_PATH "\"${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/descriptors/\"")
set(PROTOTYPE_META_PATH "\"${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/meta/\"")
target_compile_definitions(PrototypeTranspiler 
    PRIVATE PROTOTYPE_TRANSPILER_OUTPUT_DIR=${PROTOTYPE_TRANSPILER_OUTPUT_PATH}
    PRIVATE PROTOTYPE_META_DIR=${PROTOTYPE_META_PATH}
    PRIVATE PROTOTYPE_ASSETS_PATH=${PROTOTYPE_CMAKE_ASSETS_DIR}
    PRIVATE PROTOTYPE_PLUGINS_PATH=${PROTOTYPE_PLUGINS_DIR}
    PRIVATE -D__STDC_LIMIT_MACROS
    PRIVATE -D__STDC_CONSTANT_MACROS
)

target_include_directories(PrototypeTranspiler PRIVATE ${TARGET_INCLUDE_DIRS})
target_link_directories(PrototypeTranspiler PRIVATE ${TARGET_LINK_DIRS})
target_link_libraries(PrototypeTranspiler PRIVATE ${TARGET_LINK_LIBS})

# MESSAGES
MESSAGE(STATUS "PrototypeTranspiler Build type: ${CMAKE_BUILD_TYPE}")