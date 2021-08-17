#version 410 core

layout(location = 0) in vec4 inPositionU;
layout(location = 1) in vec4 inNormalV;
layout(location = 2) in vec4 inColor;

uniform mat4  View;
uniform mat4  Projection;

uniform float Roughness;

out vec3      _WorldPosition;
out float     _Roughness;

void
main()
{
    _WorldPosition  = inPositionU.xyz;
    _Roughness      = Roughness;
    gl_Position     = Projection * View * vec4(inPositionU.xyz, 1.0);
}