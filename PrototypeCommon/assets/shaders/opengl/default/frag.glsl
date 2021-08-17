#version 410 core

layout(location = 0) in vec4        _Position;
layout(location = 1) in vec3        _Normal;
layout(location = 2) flat in float  _ObjectId;
layout(location = 3) in vec2        _Texcoord;

#pragma gui
uniform sampler2D tex0;

layout(location = 0) out vec4 PositionFragColor;
layout(location = 1) out vec4 NormalFragColor;
layout(location = 2) out vec4 AlbedoFragColor;
layout(location = 3) out vec4 MetallicRoughnessAmbientBloomFragColor;
layout(location = 4) out vec4 UVObjectIDFragColor;

void
main()
{
    PositionFragColor       = vec4(_Position.xyz, 1.0);
    NormalFragColor         = vec4(_Normal, 1.0);
    AlbedoFragColor         = vec4(texture(tex0, _Texcoord).xyz, 1.0);
    UVObjectIDFragColor     = vec4(_Texcoord, _ObjectId, 1.0);
    MetallicRoughnessAmbientBloomFragColor = vec4(0.0, 0.0, 0.0, 0.0);
}