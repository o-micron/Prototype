#version 410 core

layout(location = 0) in vec3 _Color;

layout(location = 0) out vec4 FragColor;

void
main()
{
	FragColor = vec4(_Color, 1.0);
}