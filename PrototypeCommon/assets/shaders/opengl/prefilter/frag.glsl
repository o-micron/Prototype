#version 410 core

in vec3             _WorldPosition;
in float            _Roughness;

uniform samplerCube texPrefilter;

layout(location = 0) out vec4 outFragColor;

const float PI           = 3.14159265359;
const uint  SAMPLE_COUNT = 1024u;

float
DistributionGGX(in vec3 N, in vec3 H, in float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
    return nom / denom;
}

float
VanDerCorpus(uint n, uint base)
{
    float invBase = 1.0 / float(base);
    float denom   = 1.0;
    float result  = 0.0;

    for (uint i = 0u; i < 32u; ++i) {
        if (n > 0u) {
            denom = mod(float(n), 2.0);
            result += denom * invBase;
            invBase = invBase / 2.0;
            n       = uint(float(n) / 2.0);
        }
    }

    return result;
}

vec2
Hammersley(in uint i, in uint N)
{
    return vec2(float(i) / float(N), VanDerCorpus(i, 2u));
}

vec3
OptimizeGGX(in vec2 Xi, in vec3 N, in float roughness)
{
    float a        = roughness * roughness;
    float phi      = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    vec3  H;
    H.x            = cos(phi) * sinTheta;
    H.y            = sin(phi) * sinTheta;
    H.z            = cosTheta;
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

#define MEDIUMP_FLT_MAX 65504.0
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)

float
D_GGX(float roughness, float NoH, const vec3 n, const vec3 h)
{
    vec3  NxH = cross(n, h);
    float a   = NoH * roughness;
    float k   = roughness / (dot(NxH, NxH) + a * a);
    float d   = k * k * (1.0 / PI);
    return saturateMediump(d);
}

void
main()
{
    vec3  N                = normalize(_WorldPosition);
    vec3  R                = N;
    vec3  V                = R;
    vec3  prefilteredColor = vec3(0.0);
    float totalWeight      = 0.0;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec2  Xi    = Hammersley(i, SAMPLE_COUNT);
        vec3  H     = OptimizeGGX(Xi, N, _Roughness);
        vec3  L     = normalize(2.0 * dot(V, H) * H - V);
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            // float D = DistributionGGX(N, H, _Roughness);
            float D          = D_GGX(_Roughness, dot(N, H), R, H);
            float NdotH      = max(dot(N, H), 0.0);
            float HdotV      = max(dot(H, V), 0.0);
            float pdf        = D * NdotH / (4.0 * HdotV) + 0.0001;
            float resolution = 512.0;
            float saTexel    = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample   = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
            float mipLevel   = _Roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
            prefilteredColor += textureLod(texPrefilter, L, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;
    outFragColor     = vec4(prefilteredColor, 1.0);
}