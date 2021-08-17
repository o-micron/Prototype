cmake_minimum_required(VERSION 3.1)
project(PrototypeTraitSystem VERSION 1.0 DESCRIPTION "PrototypeTraitSystem" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
# ----------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------
# TARGET & THIRDPARTY
# ----------------------------------------------------------------------------------
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/include/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/src/*.cpp)
add_library(PrototypeTraitSystem ${HEADERS} ${SOURCES})
target_include_directories(PrototypeTraitSystem 
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeCommon/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/glm/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/nlohmann/include
)
target_link_directories(PrototypeTraitSystem
        PRIVATE
)
target_link_libraries(PrototypeTraitSystem
        PRIVATE
)
target_compile_definitions(PrototypeTraitSystem 
    PRIVATE PROTOTYPE_ASSETS_PATH=${PROTOTYPE_CMAKE_ASSETS_DIR}
    PRIVATE PROTOTYPE_PLUGINS_PATH=${PROTOTYPE_PLUGINS_DIR}
)
# ----------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------
# CODEGEN
# ----------------------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/cpdb/compile_commands.json")
    add_custom_target(PrototypeCompiler
            COMMAND ${CMAKE_SOURCE_DIR}/PrototypeCompiler/transpile.cmd ${CMAKE_BUILD_TYPE} > ${CMAKE_BINARY_DIR}/PrototypeCompilerLogs.txt  || (exit 0)
            COMMAND ${CMAKE_SOURCE_DIR}/PrototypeCompiler/generate.cmd ${CMAKE_BUILD_TYPE} >> ${CMAKE_BINARY_DIR}/PrototypeCompilerLogs.txt  || (exit 0)
            COMMAND ${CMAKE_SOURCE_DIR}/PrototypeCompiler/format.cmd ${CMAKE_BUILD_TYPE} >> ${CMAKE_BINARY_DIR}/PrototypeCompilerLogs.txt  || (exit 0)
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
    )
endif()
# ----------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------
# Generator #                                                                                          
# ----------------------------------------------------------------------------------
# message("running prototype transpiler")
# FILE(GLOB_RECURSE TRAIT_HEADERS ${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/include/PrototypeTraitSystem ${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/include/PrototypeTraitSystem/*.h ${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/include/PrototypeTraitSystem/*.hpp)
# message("Expected scan of traits ${TRAIT_HEADERS}")
# FOREACH(FILE_HEADER ${TRAIT_HEADERS})
#     add_custom_command(
#         TARGET PrototypeTraitSystem
#         PRE_BUILD
#         COMMAND ${CMAKE_SOURCE_DIR}/build/PrototypeTranspiler/cmake/windows/${CMAKE_BUILD_TYPE_STR}/PrototypeTranspiler.exe -p=${CMAKE_SOURCE_DIR}/build/compile_commands.json ${FILE_HEADER}
#         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/PrototypeCompiler/llvm-project/build/${CMAKE_BUILD_TYPE_STR}/bin/
#     )
# ENDFOREACH()

# message("running prototype generator")
# add_custom_command(
#     TARGET PrototypeTraitSystem
#     PRE_BUILD
#     COMMAND ${CMAKE_SOURCE_DIR}/build/PrototypeGenerator/cmake/windows/${CMAKE_BUILD_TYPE_STR}/PrototypeGenerator.exe
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )

# message("running clang-format")
# add_custom_command(
#     TARGET PrototypeTraitSystem
#     PRE_BUILD
#     COMMAND ${CMAKE_SOURCE_DIR}/PrototypeCompiler/llvm-project/build/${CMAKE_BUILD_TYPE_STR}/bin/clang-format.exe -style=file -i ${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/include/PrototypeTraitSystem/PrototypeTraitSystem.h
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )
# add_custom_command(
#     TARGET PrototypeTraitSystem
#     PRE_BUILD
#     COMMAND ${CMAKE_SOURCE_DIR}/PrototypeCompiler/llvm-project/build/${CMAKE_BUILD_TYPE_STR}/bin/clang-format.exe -style=file -i ${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/src/PrototypeTraitSystem.cpp
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )

# find_package (Python3 COMPONENTS Interpreter Development)
# if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
#     set(PYTHON_INTERPRETER python3)
# elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#     set(PYTHON_INTERPRETER python3)
# elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#     set(PYTHON_INTERPRETER Python3::Interpreter)
# endif()
# message("installing generator: ${PYTHON_INTERPRETER} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/install.py")
# add_custom_command(
#     TARGET PrototypeTraitSystem
#     PRE_BUILD
#     COMMAND ${PYTHON_INTERPRETER} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/install.py
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../
# )
# message("running generator: ${PYTHON_INTERPRETER} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/generate.py ${CMAKE_CURRENT_SOURCE_DIR}/../../")
# add_custom_command(
#     TARGET PrototypeTraitSystem
#     PRE_BUILD
#     COMMAND ${PYTHON_INTERPRETER} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/generate.py ${CMAKE_CURRENT_SOURCE_DIR}/../../
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../
# )
# ----------------------------------------------------------------------------------

# MESSAGES
MESSAGE(STATUS "PrototypeTraitSystem Build type: ${CMAKE_BUILD_TYPE}")