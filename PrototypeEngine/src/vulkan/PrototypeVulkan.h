/// Copyright 2021 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#pragma once

#include <PrototypeCommon/Definitions.h>

#include "../../include/PrototypeEngine/PrototypeEngineApi.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <PrototypeCommon/Maths.h>

#include <array>
#include <vector>

#define MAX_FRAMES_IN_FLIGHT 3
#define MAX_NUM_OBJECTS      1000

struct PrototypeSceneNode;
struct PrototypeObject;

struct PtvSwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentationModes;
};

struct PtvQueueFamilyIndices
{
    i32 graphics;
    i32 presentation;
};
struct PtvQueueFamilyQueues
{
    VkQueue graphics;
    VkQueue presentation;
};
struct PtvQueueFamilies
{
    PtvQueueFamilyIndices indices;
    PtvQueueFamilyQueues  queues;
};
struct PtvShaderModule
{
    VkShaderModule vertex;
    VkShaderModule fragment;
};
struct PtvShaderStage
{
    VkPipelineShaderStageCreateInfo vertex;
    VkPipelineShaderStageCreateInfo fragment;
};
struct PtvShader
{
    PtvShaderStage  stages;
    PtvShaderModule module;
    std::string     name;
};
struct PtvSwapchain
{
    VkSurfaceFormatKHR         format;
    VkPresentModeKHR           presentMode;
    VkExtent2D                 extent;
    VkSwapchainKHR             swapchain;
    std::vector<VkImage>       images;
    std::vector<VkImageView>   imageViews;
    std::vector<VkFramebuffer> framebuffers;
};
struct PtvGraphicsPipeline
{
    VkPipelineLayout layout;
    VkPipeline       pipeline;
};
struct PtvCommand
{
    VkCommandPool                pool;
    std::vector<VkCommandBuffer> buffers;
};
struct PtvCommandPools
{
    PtvCommand standard;
    PtvCommand transient;
    PtvCommand reset;
};
struct PtvDescriptorLayout
{
    VkDescriptorSetLayout        layout;
    std::vector<VkDescriptorSet> sets;
    std::string                  name;
};
struct PtvDescriptor
{
    VkDescriptorPool    pool;
    PtvDescriptorLayout matrices;
    PtvDescriptorLayout materials;
};
struct PtvSemaphores
{
    VkSemaphore imageAvailable;
    VkSemaphore renderFinished;
};
struct PtvSynchronization
{
    std::array<PtvSemaphores, MAX_FRAMES_IN_FLIGHT> semaphores;
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT>       inFlightFences;
    std::vector<VkFence>                            imagesInFlightFences;
};
struct PtvBuffer
{
    VkBuffer       buffer;
    VkDeviceMemory memory;
};
struct PtvGeometryBuffer
{
    PtvBuffer   vertex;
    PtvBuffer   index;
    u32         indexCount;
    std::string name;
};
struct PtvTexture
{
    VkImage        image;
    VkDeviceMemory memory;
    VkImageView    imageView;
    VkSampler      sampler;
};
struct PtvUniformBufferObjectCamera
{
    glm::mat4 view;
    glm::mat4 projection;
};
struct PtvUniformBufferObjectTransform
{
    glm::mat4 model;
};
struct PtvUniformBufferObject
{
    PtvUniformBufferObjectCamera                                 camera;
    std::array<PtvUniformBufferObjectTransform, MAX_NUM_OBJECTS> transforms;
};
struct PtvPushConstantData
{
    u32 transformIndex;
    u32 materialIndex;
};

struct PvtCamera
{
    PrototypeSceneNode* node;
    PrototypeObject*    object;
    const float*        viewMatrix;
    const float*        projectionMatrix;
};

static inline void
VK_CHECK(VkResult result)
{
    assert(result == VK_SUCCESS);
}