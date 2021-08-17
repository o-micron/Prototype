#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_NUM_OBJECTS 1000

struct PtvUniformBufferObjectCamera {
    mat4 view;
    mat4 projection;
};

struct PtvUniformBufferObjectTransform {
    mat4 model;
};

layout(set = 0, binding = 0) uniform PtvUniformBufferObject {
    PtvUniformBufferObjectCamera    camera;
    PtvUniformBufferObjectTransform transforms[MAX_NUM_OBJECTS];
} ubo;

layout(push_constant) uniform PtvConstantData {
	uint transformIndex;
	uint materialIndex;
} pc;

layout(location = 0) in vec4 inPositionU;
layout(location = 1) in vec4 inNormalV;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec2   _Texcoord;
layout(location = 1) out vec3   _Color;

void
main()
{
    gl_Position     = ubo.camera.projection * ubo.camera.view * ubo.transforms[pc.transformIndex].model * vec4(inPositionU.xyz, 1.0);   
    _Texcoord       = vec2(inPositionU.w, inNormalV.w);
    _Color          = inColor.xyz;
}