#version 410 core

layout(location = 0) in vec4       _Position;
layout(location = 1) in vec3       _Normal;
layout(location = 2) in float      _Metallic;
layout(location = 3) in vec3       _BaseColor;
layout(location = 4) in float      _Roughness;
layout(location = 5) in vec2       _Texcoord;
layout(location = 6) flat in float _ObjectId;


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
    PositionFragColor       = texture(tex0, _Texcoord).rgba * vec4(_Position.xyz, 1.0);
    NormalFragColor         = vec4(_Normal, 1.0);
    AlbedoFragColor         = vec4(_BaseColor, 1.0);
    UVObjectIDFragColor     = vec4(_Texcoord, _ObjectId, 1.0);
    MetallicRoughnessAmbientBloomFragColor = vec4(_Metallic, _Roughness, 1.0, 0.0);
}