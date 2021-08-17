#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 _Texcoord;
layout(location = 1) in vec3 _Color;

layout(set = 1, binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = vec4(_Color * texture(tex, _Texcoord).rgb, 1.0);
}