#version 410 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;

out vec2      _texcoord;

void
main()
{
    _texcoord    = inTexcoord;
    gl_Position  = vec4(inPosition.xy, 0.0, 1.0);
}