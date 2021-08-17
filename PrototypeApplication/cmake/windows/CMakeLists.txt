cmake_minimum_required(VERSION 3.1)
project(PrototypeApplication VERSION 1.0 DESCRIPTION "PrototypeApplication" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
# ----------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------
# TARGET & THIRDPARTY
# ---------------------------------------------------------------------------------- 
find_package(Vulkan REQUIRED)

set(PhysxOutputDir ${CMAKE_SOURCE_DIR}/PrototypeDependencies/PhysX/physx/bin/win.x86_64.vc142.md/${CMAKE_BUILD_TYPE_STR_TOLOWER})

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.c)
add_executable(PrototypeApplication ${HEADERS} ${SOURCES})
target_include_directories(PrototypeApplication 
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeCommon/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeEngine/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/fmt/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/glm/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/nlohmann/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/stb/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/Physx/physx/include
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/Physx/pxshared/include
)
target_link_directories(PrototypeApplication
    PRIVATE ${PhysxOutputDir}
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/assimp/build/lib/${CMAKE_BUILD_TYPE_STR}
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/glfw/build/src/${CMAKE_BUILD_TYPE_STR}
    PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/build/lib/${CMAKE_BUILD_TYPE_STR}
)
target_link_libraries(PrototypeApplication 
    PRIVATE PrototypeCommon
    PRIVATE PrototypeTraitSystem
    PRIVATE PrototypeEngine
    PRIVATE Vulkan::Vulkan
    PRIVATE glfw3
    PRIVATE assimp-vc142-mt
    PRIVATE draco
    PRIVATE PhysX_64 
    PRIVATE LowLevel_static_64
    PRIVATE LowLevelAABB_static_64
    PRIVATE LowLevelDynamics_static_64
    PRIVATE PhysXCharacterKinematic_static_64
    PRIVATE PhysXCommon_64
    PRIVATE PhysXCooking_64
    PRIVATE PhysXExtensions_static_64
    PRIVATE PhysXFoundation_64
    PRIVATE PhysXPvdSDK_static_64
    PRIVATE PhysXTask_static_64
    PRIVATE PhysXVehicle_static_64
    PRIVATE SceneQuery_static_64
    PRIVATE SimulationController_static_64
    PRIVATE ade
    PRIVATE opencv_calib3d451
    PRIVATE opencv_core451
    PRIVATE opencv_dnn451
    PRIVATE opencv_features2d451
    PRIVATE opencv_flann451
    PRIVATE opencv_gapi451
    PRIVATE opencv_highgui451
    PRIVATE opencv_imgcodecs451
    PRIVATE opencv_imgproc451
    PRIVATE opencv_ml451
    PRIVATE opencv_objdetect451
    PRIVATE opencv_photo451
    PRIVATE opencv_stitching451
    PRIVATE opencv_video451
    PRIVATE opencv_videoio451
)
# ----------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------
# MACROS
# ----------------------------------------------------------------------------------
target_compile_definitions(PrototypeApplication 
    PRIVATE PROTOTYPE_ASSETS_PATH=${PROTOTYPE_CMAKE_ASSETS_DIR}
    PRIVATE PROTOTYPE_PLUGINS_PATH=${PROTOTYPE_PLUGINS_DIR}
)
# ----------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------
# COPY DEPENDENCIES (DLLs)
# ----------------------------------------------------------------------------------
set(PhysxDlls PhysX_64.dll PhysXCommon_64.dll PhysXCooking_64.dll PhysXFoundation_64.dll)
if (MSVC)
    set(BinaryLocation ${CMAKE_BINARY_DIR}/bin)
else()
    set(BinaryLocation ${CMAKE_BINARY_DIR}/bin)
endif(MSVC)
FOREACH(DllFile ${PhysxDlls})
add_custom_command(
    TARGET PrototypeApplication POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${PhysxOutputDir}/${DllFile} ${BinaryLocation}/${DllFile}
)
ENDFOREACH()

add_custom_command(
    TARGET PrototypeApplication POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/PrototypeDependencies/assimp/build/bin/${CMAKE_BUILD_TYPE_STR}/assimp-vc142-mt.dll ${BinaryLocation}/assimp-vc142-mt.dll
)
add_custom_command(
    TARGET PrototypeApplication POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/PrototypeDependencies/assimp/build/bin/${CMAKE_BUILD_TYPE_STR}/draco.dll ${BinaryLocation}/draco.dll
)
add_custom_command(
    TARGET PrototypeApplication POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/build/bin/${CMAKE_BUILD_TYPE_STR}/opencv_core451.dll ${BinaryLocation}/opencv_core451.dll
)
add_custom_command(
    TARGET PrototypeApplication POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/build/bin/${CMAKE_BUILD_TYPE_STR}/opencv_imgcodecs451.dll ${BinaryLocation}/opencv_imgcodecs451.dll
)
add_custom_command(
    TARGET PrototypeApplication POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/build/bin/${CMAKE_BUILD_TYPE_STR}/opencv_imgproc451.dll ${BinaryLocation}/opencv_imgproc451.dll
)
add_custom_command(
    TARGET PrototypeApplication POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/build/bin/${CMAKE_BUILD_TYPE_STR}/opencv_videoio451.dll ${BinaryLocation}/opencv_videoio451.dll
)
# ----------------------------------------------------------------------------------

# MESSAGES
MESSAGE(STATUS "PrototypeApplication Build type: ${CMAKE_BUILD_TYPE}")