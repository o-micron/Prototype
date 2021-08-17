#version 410 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 _Color;

void
main()
{
    _Color = inColor;
    gl_Position = vec4(inPosition, 1.0);   
}