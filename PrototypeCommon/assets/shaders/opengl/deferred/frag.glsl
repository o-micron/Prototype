#version 410 core

layout(location = 0) in vec3   _CamPosition;
layout(location = 1) in float  _CamNear;
layout(location = 2) in vec2   _Texcoord;
layout(location = 3) in float  _CamFar;

uniform sampler2D   tex0;                 // FragPosition
uniform sampler2D   tex1;                 // FragNormal
uniform sampler2D   tex2;                 // FragAlbedo
uniform sampler2D   tex3;                 // MetallicRoughnessAmbientBloomFragColor
uniform sampler2D   tex4;                 // UVObjectIDFragColor
uniform samplerCube texSky;
uniform samplerCube texIrradiance;
uniform samplerCube texPrefilter;
uniform sampler2D   texBrdf;
uniform sampler2D   texDepth;

layout(location = 0) out vec4 FragColor;

const float PI       = 3.14159265359;
const float LOD      = 4.0;
const float exposure = 3.0;

const vec3 ColorIndices[9] = vec3[9](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, 0.0, 1.0),
    vec3(0.0, 1.0, 1.0),

    vec3(1.0, 0.5, 0.5),
    vec3(0.5, 1.0, 0.5),
    vec3(0.5, 0.5, 1.0)
);

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  
// ----------------------------------------------------------------------------

float LinearizeDepth(in float depth) {
  float z = depth;
  return (2.0 * _CamNear) / (_CamFar + _CamNear - z * (_CamFar - _CamNear));
}

void
main()
{
    float depth = LinearizeDepth(texture(texDepth, _Texcoord).r);
    vec3 Position = texture(tex0, _Texcoord).rgb;
    if(depth < 0.99) {
        vec3 Albedo = texture(tex2, _Texcoord).rgb;
        vec4 MetallicRoughnessAmbientBloomFragColor = texture(tex3, _Texcoord);
        vec4 UVObjectIDFragColor = texture(tex4, _Texcoord);
        float Metallic = MetallicRoughnessAmbientBloomFragColor.r;
        float Roughness = MetallicRoughnessAmbientBloomFragColor.g;
        if(Metallic == 0.0 && Roughness == 0.0) {
            FragColor = vec4(Albedo, 1.0);
            return;
        } else {
            vec3 Normal = texture(tex1, _Texcoord).rgb;

            vec3 V = normalize(_CamPosition - Position);
            vec3 R = reflect(-V, Normal);

            vec3 F0             = vec3(0.04);
            F0                  = mix(F0, Albedo, Metallic);
            vec3 F              = fresnelSchlickRoughness(max(dot(Normal, V), 0.0), F0, Roughness);
            vec3 kS             = F;
            vec3 kD             = 1.0 - kS;
            kD                 *= 1.0 - Metallic;
            vec3 irradiance     = texture(texIrradiance, Normal).rgb;
            vec3 diffuse        = irradiance * Albedo;

            // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
            vec3 prefilteredColor   = textureLod(texPrefilter, R,  Roughness * LOD).rgb;
            vec2 brdf               = texture(texBrdf, vec2(max(dot(Normal, V), 0.0), Roughness)).rg;
            vec3 specular           = prefilteredColor * (F * brdf.x + brdf.y);
            vec3 ambient            = (kD * diffuse + specular); //* ao;

            vec3 color = ambient;

            // HDR tonemapping
            color = color / (color + vec3(1.0));
            // gamma correct
            color = pow(color, vec3(1.0/2.2));

            // ColorIndices[uint(mod(UVObjectIDFragColor.b, 9.0))]
            FragColor = vec4(color, 1.0);
        }
    } else {
        discard;
    }
}