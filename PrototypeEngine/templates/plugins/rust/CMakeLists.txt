R"~~#%#~~(
cmake_minimum_required(VERSION 3.1)
project({{ PrototypePluginProjectName }})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/PrototypeCompiler/cmake/")

enable_language(Rust)
include(CMakeCargo)

cargo_build(NAME {{ PrototypePluginProjectName }})
)~~#%#~~"