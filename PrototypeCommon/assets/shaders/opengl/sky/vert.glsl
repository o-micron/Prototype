#version 410 core

layout(location = 0) in vec4 inPositionU;
layout(location = 1) in vec4 inNormalV;
layout(location = 2) in vec4 inColor;

uniform mat4  View;
uniform mat4  Projection;

out vec3     _WorldPosition;

void
main()
{
    _WorldPosition  = inPositionU.xyz;
    mat4 rotView    = mat4(mat3(View));
    vec4 clipPos    = Projection * rotView * vec4(_WorldPosition, 1.0);
    gl_Position     = clipPos;
}