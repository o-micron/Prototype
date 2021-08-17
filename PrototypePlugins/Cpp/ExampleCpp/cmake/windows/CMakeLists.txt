
cmake_minimum_required(VERSION 3.1)
project(ExampleCpp VERSION 1.0 DESCRIPTION "ExampleCpp" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# ----------------------------------------------------------------------------------

set(PREFERRED_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/PrototypePlugins/Cpp/ExampleCpp/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PREFERRED_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PREFERRED_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PREFERRED_OUTPUT_DIRECTORY})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PREFERRED_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PREFERRED_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PREFERRED_OUTPUT_DIRECTORY})

# ----------------------------------------------------------------------------------
# TARGET & THIRDPARTY
# ----------------------------------------------------------------------------------
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.c)
add_library(ExampleCpp SHARED ${SOURCES})
target_include_directories(ExampleCpp
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeInterface/include
)
target_link_directories(ExampleCpp
        PRIVATE ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(ExampleCpp
        PRIVATE PrototypeInterface
)
# ----------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------
# MACROS
# ----------------------------------------------------------------------------------
target_compile_definitions(ExampleCpp
        PRIVATE PROTOTYPE_PLUGIN_EXPORT_DYNAMIC
        PRIVATE PROTOTYPE_INTERFACE_IMPORT_DYNAMIC
        PRIVATE PROTOTYPE_ASSETS_PATH=${PROTOTYPE_CMAKE_ASSETS_DIR}
        PRIVATE PROTOTYPE_PLUGINS_PATH=${PROTOTYPE_PLUGINS_DIR}
)
add_custom_command(TARGET ExampleCpp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${PREFERRED_OUTPUT_DIRECTORY}ExampleCpp.dll ${CMAKE_BINARY_DIR}/bin/assets/plugins/
)
# ----------------------------------------------------------------------------------

# MESSAGES
MESSAGE(STATUS "ExampleCpp Build type: ${CMAKE_BUILD_TYPE}")
