@echo off

set SHADER_SRC_PATH=%1
set SHADER_OUT_PATH=%2

%VULKAN_SDK%/Bin/glslc.exe -c -fshader-stage=vertex %SHADER_SRC_PATH%/vert.glsl -o %SHADER_OUT_PATH%/vert.spv
%VULKAN_SDK%/Bin/glslc.exe -c -fshader-stage=fragment %SHADER_SRC_PATH%/frag.glsl -o %SHADER_OUT_PATH%/frag.spv