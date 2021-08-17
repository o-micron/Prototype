#version 410 core

in vec3             _WorldPosition;

uniform samplerCube texSky;

layout(location = 0) out vec4 FragColor;

void
main()
{
    vec3 color = textureLod(texSky, _WorldPosition, 0.0).rgb;
    FragColor = vec4(color, 1.0);
}