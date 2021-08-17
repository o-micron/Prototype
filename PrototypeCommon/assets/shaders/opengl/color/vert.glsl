#version 410 core

layout(location = 0) in vec4 inPositionU;
layout(location = 1) in vec4 inNormalV;
layout(location = 2) in vec4 inColor;

layout (std140) uniform Common
{
                                    // base alignment   // aligned offset
    mat4 View;                      // 16               // 0     (column 0)
                                    // 16               // 16    (column 1)
                                    // 16               // 32    (column 2)
                                    // 16               // 48    (column 3)  
    mat4 Projection;                // 16               // 64    (column 0)
                                    // 16               // 80    (column 1)
                                    // 16               // 96    (column 2)
                                    // 16               // 112   (column 3)
    float Near;                     // 4                // 128
    float Far;                      // 4                // 132
    vec2 MouseXY;                   // 8                // 136
    vec4 LightPosition;             // 16               // 144
    vec4 LightColor;                // 16               // 160
};
uniform mat4 Model;
uniform uint ObjectId;
uniform vec3 BaseColor;
uniform float Metallic;
uniform float Roughness;

layout(location = 0) out vec4       _Position;
layout(location = 1) out vec3       _Normal;
layout(location = 2) out float      _Metallic;
layout(location = 3) out vec3       _BaseColor;
layout(location = 4) out float      _Roughness;
layout(location = 5) out vec2       _Texcoord;
layout(location = 6) flat out float _ObjectId;

void
main()
{
    _Position       = Model * vec4(inPositionU.xyz, 1.0);
    _Normal         = mat3(Model) * inNormalV.xyz;
    _Metallic       = Metallic;
    _BaseColor      = BaseColor;
    _Roughness      = Roughness;
    _Texcoord       = vec2(inPositionU.w, inNormalV.w);
    _ObjectId       = float(ObjectId);
    gl_Position     = Projection * View * _Position;   
}