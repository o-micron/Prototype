#version 410 core

layout(location = 0) in vec2 _Texcoord;

uniform sampler2D tex0;

layout(location = 0) out vec4 FragColor;

void
main()
{
    FragColor = vec4(texture(tex0, _Texcoord).xyz, 1.0);
}