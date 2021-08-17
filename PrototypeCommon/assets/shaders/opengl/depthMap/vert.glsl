#version 410 core

layout(location = 0) in vec4 inPositionU;
layout(location = 1) in vec4 inNormalV;
layout(location = 2) in vec4 inColor;

uniform mat4 Model;
uniform mat4 LightMatrix;

void
main()
{
    gl_Position = LightMatrix * Model * vec4(inPositionU.xyz, 1.0);
}