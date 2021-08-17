cmake_minimum_required(VERSION 3.1)
project(PrototypeCommon VERSION 1.0 DESCRIPTION "PrototypeCommon" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
# ----------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------
# TARGET & THIRDPARTY
# ----------------------------------------------------------------------------------
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../../include/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/../../include/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.c)
add_library(PrototypeCommon ${HEADERS} ${SOURCES})
target_include_directories(PrototypeCommon
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/fmt/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/glm/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/nlohmann/include
)
target_link_directories(PrototypeCommon
    PRIVATE 
)
target_link_libraries(PrototypeCommon 
    PRIVATE
)
# ----------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------
# MACROS
# ----------------------------------------------------------------------------------
target_compile_definitions(PrototypeCommon
    PRIVATE PROTOTYPE_ASSETS_PATH=${PROTOTYPE_CMAKE_ASSETS_DIR}
    PRIVATE PROTOTYPE_PLUGINS_PATH=${PROTOTYPE_PLUGINS_DIR}
)

# VULKAN SHADERS
set(VULKAN_SHADER_SRC_DIR ${CMAKE_SOURCE_DIR}/PrototypeCommon/assets/shaders/vulkan)
set(VULKAN_SHADER_OUT_DIR ${CMAKE_SOURCE_DIR}/PrototypeCommon/assets/shaders/vulkan)
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()
SUBDIRLIST(VULKAN_SHADERS_LIST ${CMAKE_SOURCE_DIR}/PrototypeCommon/assets/shaders/vulkan)
list(LENGTH VULKAN_SHADERS_LIST VULKAN_SHADERS_LIST_LEN)
math(EXPR VULKAN_SHADERS_LIST_LEN_RANGE "${VULKAN_SHADERS_LIST_LEN} - 1")
foreach(VULKAN_SHADER_INDEX RANGE ${VULKAN_SHADERS_LIST_LEN_RANGE})
    list(GET VULKAN_SHADERS_LIST ${VULKAN_SHADER_INDEX} VULKAN_SHADER_NAME)
    configure_file(${VULKAN_SHADER_SRC_DIR}/${VULKAN_SHADER_NAME}/vert.glsl ${VULKAN_SHADER_OUT_DIR}/${VULKAN_SHADER_NAME}/vert.spv)
    configure_file(${VULKAN_SHADER_SRC_DIR}/${VULKAN_SHADER_NAME}/frag.glsl ${VULKAN_SHADER_OUT_DIR}/${VULKAN_SHADER_NAME}/frag.spv)
    add_custom_command(TARGET PrototypeCommon PRE_BUILD 
        COMMAND cmd /c ${CMAKE_SOURCE_DIR}/PrototypeEngine/scripts/compile_shaders.bat ${VULKAN_SHADER_SRC_DIR}/${VULKAN_SHADER_NAME} ${VULKAN_SHADER_OUT_DIR}/${VULKAN_SHADER_NAME}
    )
endforeach()

add_custom_command(
        TARGET PrototypeCommon PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/PrototypeCommon/assets ${CMAKE_BINARY_DIR}/bin/assets
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/assets/plugins
)
# ----------------------------------------------------------------------------------

# MESSAGES
MESSAGE(STATUS "PrototypeCommon Build type: ${CMAKE_BUILD_TYPE}")