#version 410 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;

layout(location = 0) out vec2 _Texcoord;

void
main()
{
    _Texcoord    = inTexcoord;
    gl_Position  = vec4(inPosition.xy, 0.0, 1.0);
}