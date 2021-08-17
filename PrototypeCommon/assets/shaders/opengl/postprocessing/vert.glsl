#version 410 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;

#pragma gui (0.0)
uniform float barrelPower;
#pragma gui (0.0)
uniform float iterations;
#pragma gui (0.0)
uniform float mixRatio;

layout(location = 0) out vec2   _Texcoord;
layout(location = 1) out float  _BarrelPower;
layout(location = 2) out float  _Iterations;
layout(location = 3) out float  _MixRatio;

void
main()
{
    _Texcoord       = inTexcoord;
    _BarrelPower    = barrelPower;
    _Iterations     = iterations;
    _MixRatio       = mixRatio;
    gl_Position     = vec4(inPosition.xy, 0.0, 1.0);

}