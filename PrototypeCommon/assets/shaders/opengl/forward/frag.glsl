#version 410 core

in vec2           _texcoord;

uniform sampler2D tex;
layout(location = 0) out vec4 frag_color;

void
main()
{
    frag_color = vec4(texture(tex, _texcoord).xyz, 1.0);
}