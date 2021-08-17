#version 410 core

in vec3 _WorldPosition;

uniform sampler2D texHdr;

layout(location = 0) out vec4 outFragColor;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2
SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
void
main()
{
    vec2 uv         = SampleSphericalMap(normalize(_WorldPosition));
    vec3 color      = texture(texHdr, uv).rgb;
    outFragColor    = vec4(color, 1.0);
}