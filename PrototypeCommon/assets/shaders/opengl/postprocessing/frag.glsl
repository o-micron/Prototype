#version 410 core

layout(location = 0) in vec2   _Texcoord;
layout(location = 1) in float  _BarrelPower;
layout(location = 2) in float  _Iterations;
layout(location = 3) in float  _MixRatio;

uniform sampler2D tex0;

layout(location = 0) out vec4 FragColor;

vec2
barrelDistortion(vec2 coord, float amt)
{
    vec2  cc   = coord - 0.5;
    float dist = dot(cc, cc);
    return coord + cc * dist * amt;
}

float
sat(float t)
{
    return clamp(t, 0.0, 1.0);
}

float
linterp(float t)
{
    return sat(1.0 - abs(2.0 * t - 1.0));
}

float
remap(float t, float a, float b)
{
    return sat((t - a) / (b - a));
}

vec3
spectrum_offset(float t)
{
    vec3  ret;
    float lo = step(t, 0.5);
    float hi = 1.0 - lo;
    float w  = linterp(remap(t, 1.0 / 6.0, 5.0 / 6.0));
    ret      = vec3(lo, 1.0, hi) * vec3(1.0 - w, w, 1.0 - w);

    return pow(ret, vec3(1.0 / 2.2));
}

vec4
barrelBlurChroma()
{
    float reci_num_iter_f = 1.0 / (3.0 + _Iterations);
    vec3  sumcol          = vec3(0.0);
    vec3  sumw            = vec3(0.0);
    for (float i = 0.0; i < (3.0 + _Iterations); i += 1.0) {
        float t = float(i) * reci_num_iter_f;
        vec3  w = spectrum_offset(t);
        sumw += w;
        sumcol += w * texture(tex0, barrelDistortion(_Texcoord, _BarrelPower * t)).rgb;
    }
    return vec4(sumcol.rgb / sumw, 1.0);
}

float
convolve(mat3 kernel, mat3 image)
{
    float result = 0.0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result += kernel[i][j] * image[i][j];
        }
    }
    return result;
}

float
convolveComponent(mat3 kernelX, mat3 kernelY, mat3 image)
{
    vec2 result;
    result.x = convolve(kernelX, image);
    result.y = convolve(kernelY, image);
    return clamp(length(result), 0.0, 255.0);
}

vec4
colorEdge(float stepx, float stepy, vec2 center, mat3 kernelX, mat3 kernelY)
{
    vec4 colors[9];
    colors[0] = texture(tex0, center + vec2(-stepx, stepy));
    colors[1] = texture(tex0, center + vec2(0, stepy));
    colors[2] = texture(tex0, center + vec2(stepx, stepy));
    colors[3] = texture(tex0, center + vec2(-stepx, 0));
    colors[4] = texture(tex0, center);
    colors[5] = texture(tex0, center + vec2(stepx, 0));
    colors[6] = texture(tex0, center + vec2(-stepx, -stepy));
    colors[7] = texture(tex0, center + vec2(0, -stepy));
    colors[8] = texture(tex0, center + vec2(stepx, -stepy));

    mat3 imageR, imageG, imageB, imageA;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            imageR[i][j] = colors[i * 3 + j].r;
            imageG[i][j] = colors[i * 3 + j].g;
            imageB[i][j] = colors[i * 3 + j].b;
            imageA[i][j] = colors[i * 3 + j].a;
        }
    }

    vec4 color;
    color.r = convolveComponent(kernelX, kernelY, imageR);
    color.g = convolveComponent(kernelX, kernelY, imageG);
    color.b = convolveComponent(kernelX, kernelY, imageB);
    color.a = convolveComponent(kernelX, kernelY, imageA);

    return color;
}

vec4
edge(float stepx, float stepy, vec2 center, mat3 kernelX, mat3 kernelY)
{
    // get samples around pixel
    mat3 image = mat3(length(texture(tex0, center + vec2(-stepx, stepy)).rgb),
                      length(texture(tex0, center + vec2(0, stepy)).rgb),
                      length(texture(tex0, center + vec2(stepx, stepy)).rgb),
                      length(texture(tex0, center + vec2(-stepx, 0)).rgb),
                      length(texture(tex0, center).rgb),
                      length(texture(tex0, center + vec2(stepx, 0)).rgb),
                      length(texture(tex0, center + vec2(-stepx, -stepy)).rgb),
                      length(texture(tex0, center + vec2(0, -stepy)).rgb),
                      length(texture(tex0, center + vec2(stepx, -stepy)).rgb));
    vec2 result;
    result.x = convolve(kernelX, image);
    result.y = convolve(kernelY, image);

    float color = clamp(length(result), 0.0, 255.0);
    return vec4(color);
}

vec4
trueColorEdge(float stepx, float stepy, vec2 center, mat3 kernelX, mat3 kernelY)
{
    vec4 edgeVal = edge(stepx, stepy, center, kernelX, kernelY);
    return edgeVal * texture(tex0, center);
}

vec4
edgeFilter()
{
    const mat3 sobelKernelX = mat3(1.0, 0.0, -1.0, 2.0, 0.0, -2.0, 1.0, 0.0, -1.0);

    const mat3 sobelKernelY = mat3(-1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0);

    const float STEP = 0.002;
    return edge(STEP, STEP, _Texcoord, sobelKernelX, sobelKernelY);
}

void
main()
{
    FragColor = mix(barrelBlurChroma(), edgeFilter(), _MixRatio);
}