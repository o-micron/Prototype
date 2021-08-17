cmake_minimum_required(VERSION 3.1)
project(PrototypeGenerator VERSION 1.0 DESCRIPTION "PrototypeGenerator" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)

set(PROTOTYPE_CMAKE_ASSETS_LOCAL_DIR "\"${CMAKE_BINARY_DIR}/bin/assets/\"")
set(PROTOTYPE_CMAKE_ASSETS_DIR "\"assets/\"")
set(PROTOTYPE_PLUGINS_DIR "\"../../../PrototypePlugins/\"")

# ----------------------------------------------------------------------------------
# GENERATOR
# ----------------------------------------------------------------------------------
add_executable(PrototypeGenerator ${CMAKE_CURRENT_SOURCE_DIR}/../../src/PrototypeGenerator.cpp)

set(PROTOTYPE_GENERATOR_DESCRIPTORS_PATH "\"${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/descriptors/\"")
set(PROTOTYPE_GENERATOR_TEMPLATES_PATH "\"${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/templates/\"")
set(PROTOTYPE_GENERATOR_OUTPUT_HEADER_PATH "\"${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/include/PrototypeTraitSystem/\"")
set(PROTOTYPE_GENERATOR_OUTPUT_SRC_PATH "\"${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/src/\"")
set(PROTOTYPE_META_PATH "\"${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/meta/\"")
target_compile_definitions(PrototypeGenerator 
    PRIVATE PROTOTYPE_GENERATOR_DESCRIPTORS_DIR=${PROTOTYPE_GENERATOR_DESCRIPTORS_PATH}    
    PRIVATE PROTOTYPE_GENERATOR_TEMPLATES_DIR=${PROTOTYPE_GENERATOR_TEMPLATES_PATH}    
    PRIVATE PROTOTYPE_GENERATOR_OUTPUT_HEADER_DIR=${PROTOTYPE_GENERATOR_OUTPUT_HEADER_PATH}
    PRIVATE PROTOTYPE_GENERATOR_OUTPUT_SRC_DIR=${PROTOTYPE_GENERATOR_OUTPUT_SRC_PATH}
    PRIVATE PROTOTYPE_META_DIR=${PROTOTYPE_META_PATH}
    PRIVATE PROTOTYPE_ASSETS_PATH=${PROTOTYPE_CMAKE_ASSETS_DIR}
    PRIVATE PROTOTYPE_PLUGINS_PATH=${PROTOTYPE_PLUGINS_DIR}
)

target_include_directories(PrototypeGenerator
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/nlohmann/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/jinja2cpp/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/jinja2cpp/thirdparty/fmt/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/jinja2cpp/thirdparty/nlohmann/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/jinja2cpp/thirdparty/rapidjson/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/jinja2cpp/thirdparty/nonstd/expected-lite/include
)
target_link_directories(PrototypeGenerator
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/jinja2cpp/build/${CMAKE_BUILD_TYPE}
)
target_link_libraries(PrototypeGenerator
    PRIVATE Jinja2Cpp
)
# ----------------------------------------------------------------------------------

# MESSAGES
MESSAGE(STATUS "PrototypeGenerator Build type: ${CMAKE_BUILD_TYPE}")