cmake_minimum_required(VERSION 3.1)
project(PrototypeInterface VERSION 1.0 DESCRIPTION "PrototypeInterface" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)
# ----------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------
# TARGET & THIRDPARTY
# ----------------------------------------------------------------------------------
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../../include/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/../../include/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.c)
add_library(PrototypeInterface SHARED ${HEADERS} ${SOURCES})
target_include_directories(PrototypeInterface
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeCommon/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeTraitSystem/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeEngine/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/fmt/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/glm/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/nlohmann/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/stb/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/Physx/physx/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/Physx/pxshared/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/assimp/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/assimp/build/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/glfw/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/core/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/calib3d/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/features/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/features2d/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/flann/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/dnn/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/highgui/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/imgcodecs/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/videoio/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/imgproc/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/ml/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/objdetect/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/photo/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/stitching/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/modules/video/include
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/build
)
target_link_directories(PrototypeInterface
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/PhysX/physx/bin/win.x86_64.vc142.md/${CMAKE_BUILD_TYPE_STR_TOLOWER}
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/assimp/build/lib/${CMAKE_BUILD_TYPE_STR}
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/glfw/build/src/${CMAKE_BUILD_TYPE_STR}
        PRIVATE ${CMAKE_SOURCE_DIR}/PrototypeDependencies/opencv/build/lib/${CMAKE_BUILD_TYPE_STR}
)
target_link_libraries(PrototypeInterface
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
target_compile_definitions(PrototypeInterface
        PRIVATE PROTOTYPE_INTERFACE_EXPORT_DYNAMIC
        PRIVATE PROTOTYPE_ASSETS_PATH=${PROTOTYPE_CMAKE_ASSETS_DIR}
        PRIVATE PROTOTYPE_PLUGINS_PATH=${PROTOTYPE_PLUGINS_DIR}
)
set_target_properties(PrototypeInterface
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
#add_custom_command(
#        TARGET PrototypeInterface POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/PrototypeInterface.dll ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/PrototypeInterface.dll
#)
# ----------------------------------------------------------------------------------

# MESSAGES
MESSAGE(STATUS "PrototypeInterface Build type: ${CMAKE_BUILD_TYPE}")