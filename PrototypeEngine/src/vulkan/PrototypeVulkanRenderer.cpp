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

#include "PrototypeVulkanRenderer.h"

#include "../core/PrototypeCameraSystem.h"
#include "../core/PrototypeDatabase.h"
#include "../core/PrototypeEngine.h"
#include "../core/PrototypeInput.h"
#include "../core/PrototypeMaterial.h"
#include "../core/PrototypeMeshBuffer.h"
#include "../core/PrototypePhysics.h"
#include "../core/PrototypeScene.h"
#include "../core/PrototypeSceneLayer.h"
#include "../core/PrototypeSceneNode.h"
#include "../core/PrototypeShaderBuffer.h"
#include "../core/PrototypeShortcuts.h"
#include "../core/PrototypeTextureBuffer.h"
#include "PrototypeVulkanWindow.h"

#include <PrototypeTraitSystem/Camera.h>
#include <PrototypeTraitSystem/PrototypeTraitSystem.h>
#include <PrototypeTraitSystem/Transform.h>

#include <PrototypeCommon/IO.h>
#include <PrototypeCommon/Logger.h>
#include <PrototypeCommon/Patches.h>

#include <algorithm>
#include <assert.h>
#include <chrono>
#include <functional>
#include <iterator>
#include <sstream>
#include <vector>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_vulkan.h"

const std::vector<const char*> PrototypeVulkanRenderer::requiredValidationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> PrototypeVulkanRenderer::requiredExtensions       = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

static VKAPI_ATTR VkBool32
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void*                                       pUserData)
{
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
            PrototypeLogger::error("%s", pCallbackData->pMessage);
        } break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
            PrototypeLogger::warn("%s", pCallbackData->pMessage);
        } break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
        } break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
        } break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: {
            PrototypeLogger::fatal("Unreachable()!");
        } break;
    }

    return VK_FALSE;
}

static VkVertexInputBindingDescription
getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding                         = 0;
    bindingDescription.stride                          = sizeof(PrototypeMeshVertex);
    bindingDescription.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

static std::array<VkVertexInputAttributeDescription, 3>
getAttributeDescriptions()
{
    VkVertexInputAttributeDescription attr0 = {};
    attr0.location                          = 0;
    attr0.format                            = VK_FORMAT_R32G32B32A32_SFLOAT;
    attr0.offset                            = PrototypeOffsetOf(&PrototypeMeshVertex::positionU);

    VkVertexInputAttributeDescription attr1 = {};
    attr1.binding                           = 0;
    attr1.location                          = 1;
    attr1.format                            = VK_FORMAT_R32G32B32A32_SFLOAT;
    attr1.offset                            = PrototypeOffsetOf(&PrototypeMeshVertex::normalV);

    VkVertexInputAttributeDescription attr2 = {};
    attr2.binding                           = 0;
    attr2.location                          = 2;
    attr2.format                            = VK_FORMAT_R32G32B32A32_SFLOAT;
    attr2.offset                            = PrototypeOffsetOf(&PrototypeMeshVertex::color);

    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = { attr0, attr1, attr2 };

    return attributeDescriptions;
}

PtvSwapchainSupportDetails
PrototypeVulkanRenderer::querySwapchainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    PtvSwapchainSupportDetails swapchainSupportDetails;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapchainSupportDetails.capabilities);
    // surface formats
    u32 formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    swapchainSupportDetails.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapchainSupportDetails.formats.data());

    // presentation modes
    u32 presentationModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount, nullptr);
    swapchainSupportDetails.presentationModes.resize(presentationModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
      physicalDevice, surface, &presentationModeCount, swapchainSupportDetails.presentationModes.data());

    return swapchainSupportDetails;
}

VkSurfaceFormatKHR
PrototypeVulkanRenderer::selectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats)
{
    for (const auto& availableFormat : availableSurfaceFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableSurfaceFormats[0];
}

VkPresentModeKHR
PrototypeVulkanRenderer::selectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) { return availablePresentMode; }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
PrototypeVulkanRenderer::selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, PrototypeVulkanWindow* window)
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        window->getFramebufferExtent(&width, &height);

        VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width =
          std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height =
          std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

u32
PrototypeVulkanRenderer::findMemoryType(VkPhysicalDevice physicalDevice, u32 typeFilter, VkMemoryPropertyFlags propertyFlags)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (u32 i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags) {
            return i;
        }
    }

    PrototypeLogger::fatal("Failed to find suitable memory type");
    return 0;
}

VkFormat
PrototypeVulkanRenderer::findSupportedFormat(VkPhysicalDevice             physicalDevice,
                                             const std::vector<VkFormat>& candidates,
                                             VkImageTiling                tiling,
                                             VkFormatFeatureFlags         features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    PrototypeLogger::fatal("Failed to find supported format");
    return candidates.back();
}

VkFormat
PrototypeVulkanRenderer::findSupportedDepthFormat(VkPhysicalDevice physicalDevice)
{
    return findSupportedFormat(physicalDevice,
                               { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool
PrototypeVulkanRenderer::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

PrototypeVulkanRenderer::PrototypeVulkanRenderer(PrototypeVulkanWindow* window)
  : _swapchain({})
  , _commandPools({})
  , _uiState(PrototypeUIState_None)
  , _descriptor({})
  , _window(window)
  , _synchronization({})
  , _queueFamilies({})
  , _depthTexture({})
  , _mainCamera({})
  , _instance(nullptr)
  , _graphicsPipeline({})
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
  , _ui(std::make_unique<PrototypeVulkanUI>())
#endif
  , _debugMessenger(nullptr)
  , _surface(nullptr)
  , _physicalDevice(nullptr)
  , _device(nullptr)
  , _renderPass(nullptr)
  , _currentFrame(0)
  , _framebufferResized(false)
  , _needsRecord(true)
{}

bool
PrototypeVulkanRenderer::init()
{
    if (!createInstance()) { return false; }
#if defined(PROTOTYPE_TARGET_DEBUG)
    if (!createDebuggerUtilsMessenger()) { return false; }
#endif
    if (!createSurface()) { return false; }
    if (!selectPhysicalDevice()) { return false; }
    if (!createLogicalDevice()) { return false; }
    if (!createSwapchain()) { return false; }
    if (!createSwapchainImageViews()) { return false; }
    if (!createRenderPass()) { return false; }
    if (!createDescriptorSetLayout()) { return false; }
    if (!createGraphicsPipeline()) { return false; }
    if (!createCommandPools()) { return false; }
    if (!createDepthResources()) { return false; }
    if (!createFramebuffers()) { return false; }
    if (!loadTexturesFromBuffers()) { return false; }
    if (!loadMeshesFromBuffers()) { return false; }
    if (!createUniformBuffers()) { return false; }
    if (!createDescriptorPool()) { return false; }
    if (!createDescriptorSets()) { return false; }
    if (!createGui()) { return false; }
    if (!createCommandBuffers()) { return false; }
    if (!createSemaphores()) { return false; }
    if (!createFences()) { return false; }

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    _ui->init();
#else
    _input = PROTOTYPE_NEW PrototypeInput();

    _input->subscribeKeyForStreams(GLFW_KEY_W);
    _input->subscribeKeyForStreams(GLFW_KEY_S);
    _input->subscribeKeyForStreams(GLFW_KEY_D);
    _input->subscribeKeyForStreams(GLFW_KEY_A);
    _input->subscribeKeyForStreams(GLFW_KEY_I);
    _input->subscribeKeyForStreams(GLFW_KEY_K);

    _input->subscribeKeyForStreams(GLFW_KEY_UP);
    _input->subscribeKeyForStreams(GLFW_KEY_DOWN);
    _input->subscribeKeyForStreams(GLFW_KEY_RIGHT);
    _input->subscribeKeyForStreams(GLFW_KEY_LEFT);
#endif

    switchScenes(PrototypeEngineInternalApplication::scene);

    return true;
}

void
PrototypeVulkanRenderer::deInit()
{
    vkDeviceWaitIdle(_device);

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    _ui->deInit();
#else
    delete _input;
#endif

    cleanupSwapchain();

    clearMeshes();
    clearTextures();

    for (VkFence& fence : _synchronization.inFlightFences) { vkDestroyFence(_device, fence, nullptr); }

    for (PtvSemaphores& semaphore : _synchronization.semaphores) {
        vkDestroySemaphore(_device, semaphore.renderFinished, nullptr);
        vkDestroySemaphore(_device, semaphore.imageAvailable, nullptr);
    }

    vkDestroyCommandPool(_device, _commandPools.reset.pool, nullptr);
    _commandPools.reset.pool = nullptr;

    vkDestroyCommandPool(_device, _commandPools.transient.pool, nullptr);
    _commandPools.transient.pool = nullptr;

    vkDestroyCommandPool(_device, _commandPools.standard.pool, nullptr);
    _commandPools.standard.pool = nullptr;

    vkDestroyDevice(_device, nullptr);
    _device = nullptr;

    if (_debugMessenger) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");
        PROTOTYPE_ASSERT_MSG(func, "Failed to get address of vkDestroyDebugUtilsMessengerEXT");
        func(_instance, _debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    _surface = nullptr;

    vkDestroyInstance(_instance, nullptr);
    _instance = nullptr;
}

bool
PrototypeVulkanRenderer::update()
{
#if defined(PROTOTYPE_ENGINE_DEVELOPMENT_MODE)
    _ui->sceneView()->onUpdate();
#else
    _input->update();

    f32 vehicleAcceleration = _input->fetchKeyNormalizedValue(GLFW_KEY_UP);
    f32 vehicleBrake        = _input->fetchKeyNormalizedValue(GLFW_KEY_DOWN);
    f32 vehicleRight        = _input->fetchKeyNormalizedValue(GLFW_KEY_RIGHT);
    f32 vehicleLeft         = _input->fetchKeyNormalizedValue(GLFW_KEY_LEFT);
    f32 cameraForward       = _input->fetchKeyNormalizedValue(GLFW_KEY_W);
    f32 cameraBackward      = _input->fetchKeyNormalizedValue(GLFW_KEY_S);
    f32 cameraRight         = _input->fetchKeyNormalizedValue(GLFW_KEY_D);
    f32 cameraLeft          = _input->fetchKeyNormalizedValue(GLFW_KEY_A);
    f32 cameraUp            = _input->fetchKeyNormalizedValue(GLFW_KEY_I);
    f32 cameraDown          = _input->fetchKeyNormalizedValue(GLFW_KEY_K);

    auto vehicleObjets = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskTransform |
                                                                                         PrototypeTraitTypeMaskVehicleChasis);
    for (auto& vehicleObject : vehicleObjets) {
        VehicleChasis* chasis = vehicleObject->getVehicleChasisTrait();
        PrototypeEngineInternalApplication::physics->updateVehicleController(
          vehicleObject, vehicleAcceleration, vehicleBrake, vehicleRight - vehicleLeft);
    }

    Camera* cam = _mainCamera.object->getCameraTrait();
    CameraSystemUpdateViewMatrix(cam, cameraLeft - cameraRight, cameraDown - cameraUp, cameraForward - cameraBackward);
    CameraSystemUpdateProjectionMatrix(cam);
#endif

    handleAllActions();
    _actions.clear();

    return true;
}

bool
PrototypeVulkanRenderer::render3D()
{
    vkWaitForFences(_device, 1, &_synchronization.inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    u32      imageIndex;
    VkResult result = vkAcquireNextImageKHR(_device,
                                            _swapchain.swapchain,
                                            UINT64_MAX,
                                            _synchronization.semaphores[_currentFrame].imageAvailable,
                                            VK_NULL_HANDLE,
                                            &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return true;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        PROTOTYPE_ASSERT_MSG(false, "Failed to acquire swap chain image");
    }

    updateUniformBuffer(imageIndex);

    if (_synchronization.imagesInFlightFences[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(_device, 1, &_synchronization.imagesInFlightFences[imageIndex], VK_TRUE, UINT64_MAX);
    }
    _synchronization.imagesInFlightFences[imageIndex] = _synchronization.inFlightFences[_currentFrame];

    std::vector<VkSemaphore>          waitSemaphores   = { _synchronization.semaphores[_currentFrame].imageAvailable };
    std::vector<VkSemaphore>          signalSemaphores = { _synchronization.semaphores[_currentFrame].renderFinished };
    std::vector<VkPipelineStageFlags> waitStageMasks   = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    std::vector<VkSwapchainKHR>       swapchains       = { _swapchain.swapchain };

    VkSubmitInfo submitInfo         = {};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = waitSemaphores.data();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores.data();
    submitInfo.pWaitDstStageMask    = waitStageMasks.data();
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &_commandPools.standard.buffers[imageIndex];

    vkResetFences(_device, 1, &_synchronization.inFlightFences[_currentFrame]);

    VK_CHECK(vkQueueSubmit(_queueFamilies.queues.graphics, 1, &submitInfo, _synchronization.inFlightFences[_currentFrame]));

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    _ui->beginFrame(_framebufferResized);
    {
        _uiState = _ui->drawFrame(0, _window->_resolution.x, _window->_resolution.y);
        if (_uiState & PrototypeUIState_SceneViewResized) {}
    }
    _ui->endFrame();

    _ui->render(0, 0, _window->_resolution.x, _window->_resolution.y);
    _ui->frameRender();

    _ui->framePresent();
#else
    VkPresentInfoKHR presentInfo   = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores.data();
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = swapchains.data();
    presentInfo.pImageIndices      = &imageIndex;
    presentInfo.pResults           = nullptr;

    result = vkQueuePresentKHR(_queueFamilies.queues.presentation, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized) {
        _framebufferResized = false;
        recreateSwapchain();
    } else if (result != VK_SUCCESS) {
        PROTOTYPE_ASSERT_MSG(false, "Failed to present swap chain image");
    }
#endif
    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return true;
}

bool
PrototypeVulkanRenderer::render2D()
{
    return true;
}

void
PrototypeVulkanRenderer::switchScenes(PrototypeScene* scene)
{
    PrototypeEngineInternalApplication::scene = scene;

    auto cameraObjects           = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera);
    _mainCamera.object           = *cameraObjects.begin();
    _mainCamera.node             = static_cast<PrototypeSceneNode*>(_mainCamera.object->parentNode());
    _mainCamera.viewMatrix       = static_cast<const f32*>(&_mainCamera.object->getCameraTrait()->viewMatrix()[0][0]);
    _mainCamera.projectionMatrix = static_cast<const f32*>(&_mainCamera.object->getCameraTrait()->projectionMatrix()[0][0]);
#if defined(PROTOTYPE_ENGINE_DEVELOPMENT_MODE)
    _ui->sceneView()->setCamera(&_mainCamera);
#endif

    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();
}

void
PrototypeVulkanRenderer::scheduleRecordPass()
{
    _needsRecord = true;
}

void
PrototypeVulkanRenderer::beginRecordPass()
{
    if (!_needsRecord) { return; }
    _needsRecord = false;

    recreateCommandBuffers();
    PrototypeEngineInternalApplication::window->resetDeltaTime();
}

void
PrototypeVulkanRenderer::endRecordPass()
{}

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
PrototypeUI*
PrototypeVulkanRenderer::ui()
{
    return _ui.get();
}
#endif

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
// Camera*
// PrototypeVulkanRenderer::editorGameCamera()
// {
//     return static_cast<PvtCamera*>(_ui->_sceneView.camera())->object->getCameraTrait();
// }

Camera*
PrototypeVulkanRenderer::editorSceneCamera()
{
    return static_cast<PvtCamera*>(_ui->sceneView()->camera())->object->getCameraTrait();
}

// Camera*
// PrototypeVulkanRenderer::editorPaintCamera()
// {}
#else
Camera*
PrototypeVulkanRenderer::mainCamera()
{
    return _mainCamera.object->getCameraTrait();
}
#endif

void
PrototypeVulkanRenderer::mapPrototypeMeshBuffer(PrototypeMeshBuffer* meshBuffer)
{}

void
PrototypeVulkanRenderer::mapPrototypeShaderBuffer(PrototypeShaderBuffer* shaderBuffer)
{}

void
PrototypeVulkanRenderer::mapPrototypeTextureBuffer(PrototypeTextureBuffer* textureBuffer)
{}

void
PrototypeVulkanRenderer::mapPrototypeMaterial(PrototypeMaterial* material)
{}

void
PrototypeVulkanRenderer::onMeshBufferGpuUpload(PrototypeMeshBuffer* meshBuffer)
{}

void
PrototypeVulkanRenderer::onShaderBufferGpuUpload(PrototypeShaderBuffer* shaderBuffer)
{}

void
PrototypeVulkanRenderer::onTextureBufferGpuUpload(PrototypeTextureBuffer* textureBuffer)
{}

void
PrototypeVulkanRenderer::fetchCamera(const std::string& name, void** data)
{}

void
PrototypeVulkanRenderer::fetchDefaultMesh(void** data)
{}

void
PrototypeVulkanRenderer::fetchMesh(const std::string& name, void** data)
{}

void
PrototypeVulkanRenderer::fetchDefaultShader(void** data)
{}

void
PrototypeVulkanRenderer::fetchShader(const std::string& name, void** data)
{}

void
PrototypeVulkanRenderer::fetchDefaultTexture(void** data)
{}

void
PrototypeVulkanRenderer::fetchTexture(const std::string& name, void** data)
{}

void
PrototypeVulkanRenderer::fetchDefaultMaterial(void** data)
{}

void
PrototypeVulkanRenderer::fetchMaterial(const std::string& name, void** data)
{}

void
PrototypeVulkanRenderer::fetchDefaultFramebuffer(void** data)
{}

void
PrototypeVulkanRenderer::fetchFramebuffer(const std::string& name, void** data)
{}

// [[nodiscard]] PrototypeSceneNode*
// PrototypeVulkanRenderer::onSceneViewClick(const glm::vec2& coordinates, const glm::vec2& Size)
// {
//     PrototypeEngineInternalApplication::scene->clearSelectedNodes();
//     Camera*     cam                 = _mainCamera.object->getCameraTrait();
//     const auto& camPosition         = cam->position();
//     const auto& camViewMatrix       = cam->viewMatrix();
//     const auto& camProjectionMatrix = cam->projectionMatrix();

//     glm::vec3 ray;
//     PrototypeMaths::projectRayFromClipSpacePoint(
//       ray, camViewMatrix, camProjectionMatrix, coordinates.x, coordinates.y, Size.x, Size.y);

//     auto optHit =
//       PrototypeEngineInternalApplication::physics->raycast({ camPosition.x, camPosition.y, camPosition.z }, ray, cam->zfar());
//     if (optHit.has_value()) {
//         auto hit = optHit.value();
//         if (hit && hit->parentNode()) {
//             auto node = static_cast<PrototypeSceneNode*>(hit->parentNode());
//             return node;
//         }
//     }
//     return nullptr;
// }

void
PrototypeVulkanRenderer::onMouse(i32 button, i32 action, i32 mods)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onMouse(button, action, mods)) return;
    if (_ui->needsMouse() && !_ui->sceneView()->isHovered()) return;
#endif
}

void
PrototypeVulkanRenderer::onMouseMove(f64 xpos, f64 ypos)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onMouseMove(xpos, ypos)) return;
    if (_ui->needsMouse() && !_ui->sceneView()->isHovered()) return;
#endif
}

void
PrototypeVulkanRenderer::onMouseDrag(i32 button, f64 xoffset, f64 yoffset)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onMouseDrag(button, xoffset, yoffset)) return;
    if (_ui->needsMouse() && !_ui->sceneView()->isHovered()) return;
#else
    if (_window->_mouseDown.z) { CameraSystemRotate(_mainCamera.object->getCameraTrait(), (f32)xoffset, (f32)yoffset); }
#endif
}

void
PrototypeVulkanRenderer::onMouseScroll(f64 xoffset, f64 yoffset)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onMouseScroll(xoffset, yoffset)) return;
    if (_ui->needsMouse() && !_ui->sceneView()->isHovered()) return;
#else
    CameraSystemRotate(_mainCamera.object->getCameraTrait(), (f32)xoffset, (f32)yoffset);
#endif
}

void
PrototypeVulkanRenderer::onKeyboard(i32 key, i32 scancode, i32 action, i32 mods)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onKeyboard(key, scancode, action, mods)) return;
    if (_ui->needsKeyboard() || (_ui->needsMouse() && !_ui->sceneView()->isHovered())) return;
#else
    _input->onKeyboard(key, scancode, action, mods);
    if (action == GLFW_PRESS) {
        // spawning stuff in scene
        {
            Camera* cam = _mainCamera.object->getCameraTrait();
            const auto& cameraPosition = cam->position();
            const auto& cameraRotation = cam->rotation();
            const auto& cameraResolution = cam->resolution();
            const auto& camViewMatrix = cam->viewMatrix();
            const auto& camProjectionMatrix = cam->projectionMatrix();

            glm::vec3 ray;
            PrototypeMaths::projectRayFromClipSpacePoint(ray,
                                                         camViewMatrix,
                                                         camProjectionMatrix,
                                                         _window->_mouseLocation.x,
                                                         _window->_mouseLocation.y,
                                                         _window->_resolution.x,
                                                         _window->_resolution.y);
            glm::vec3 pos = { cameraPosition.x, cameraPosition.y, cameraPosition.z };
            glm::vec3 rot = { cameraRotation.x, cameraRotation.y, 0.0f };
            const f32 speed = 10.0f;
            ray.x *= speed;
            ray.y *= speed;
            ray.z *= speed;

            if (key == GLFW_KEY_1) {
                shortcutSpawnSphere(pos, rot, ray);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_2) {
                shortcutSpawnCube(pos, rot, ray);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_3) {
                shortcutSpawnConvexMesh(pos, rot, ray, "ico.obj", PROTOTYPE_DEFAULT_MATERIAL);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_4) {
                shortcutSpawnConvexMesh(pos, rot, ray, "monkey.obj", PROTOTYPE_DEFAULT_MATERIAL);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_5) {
                shortcutSpawnConvexMesh(pos, rot, ray, "capsule.obj", PROTOTYPE_DEFAULT_MATERIAL);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_6) {
                shortcutSpawnConvexMesh(pos, rot, ray, "cylinder.obj", PROTOTYPE_DEFAULT_MATERIAL);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_7) {
                shortcutSpawnVehicle(pos, rot, ray, "CUBE", PROTOTYPE_DEFAULT_MATERIAL);
                PrototypeEngineInternalApplication::physics->spawnVehicle();
            }
        }
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_G) {
            PrototypeEngineInternalApplication::physics->controlledVehiclesToggleGearDirection();
        } else if (key == GLFW_KEY_EQUAL) {
            PrototypeEngineInternalApplication::physics->requestNextVehicleAccessControl();
        } else if (key == GLFW_KEY_MINUS) {
            PrototypeEngineInternalApplication::physics->requestPreviousVehicleAccessControl();
        } else if (key == GLFW_KEY_0) {
            PrototypeEngineInternalApplication::physics->controlledVehiclesFlip();
        }
    }
#endif
}

void
PrototypeVulkanRenderer::onWindowResize(i32 width, i32 height)
{
    Camera* cam = _mainCamera.object->getCameraTrait();
    CameraSystemSetResolution(cam, (f32)width, (f32)height);
    _actions.emplace_back([this]() { _framebufferResized = true; });
}

void
PrototypeVulkanRenderer::onWindowDragDrop(i32 numFiles, const char** names)
{}

void
PrototypeVulkanRenderer::detectQueueFamilyIndices(VkPhysicalDevice       physicalDevice,
                                                  VkSurfaceKHR           surface,
                                                  PtvQueueFamilyIndices& queueFamilyIndices)
{
    u32 queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

    std::vector<i32> graphicsQueueIndicesStack;
    std::vector<i32> presentationQueueIndicesStack;

    for (u32 i = 0; i < queueFamilyCount; ++i) {
        {
            bool matches = true;
            matches &= queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
            if (matches) { graphicsQueueIndicesStack.push_back(i); }
        }
        {
            VkBool32 supportsPresentation = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresentation);
            if (supportsPresentation == VK_TRUE) { presentationQueueIndicesStack.push_back(i); }
        }
    }

    // prefer different queue indices
    // we just need to make sure we have separate graphics and presentation indices
    // check [VUID-VkDeviceCreateInfo-queueFamilyIndex-02802] for more details
    std::sort(graphicsQueueIndicesStack.begin(), graphicsQueueIndicesStack.end());
    std::sort(presentationQueueIndicesStack.begin(), presentationQueueIndicesStack.end());
    if (graphicsQueueIndicesStack.size() <= presentationQueueIndicesStack.size()) {
        for (i32 gi : graphicsQueueIndicesStack) {
            for (i32 pi : presentationQueueIndicesStack) {
                if (pi != gi) {
                    queueFamilyIndices.graphics     = gi;
                    queueFamilyIndices.presentation = pi;
                    return;
                }
            }
        }
    } else {
        for (i32 pi : presentationQueueIndicesStack) {
            for (i32 gi : graphicsQueueIndicesStack) {
                if (gi != pi) {
                    queueFamilyIndices.presentation = pi;
                    queueFamilyIndices.graphics     = gi;
                    return;
                }
            }
        }
    }
}

bool
PrototypeVulkanRenderer::meetsRequirements(VkPhysicalDevice             physicalDevice,
                                           VkSurfaceKHR                 surface,
                                           const PtvQueueFamilyIndices& queueFamilyIndices)
{
    PtvSwapchainSupportDetails swapchainDetails = querySwapchainSupportDetails(physicalDevice, surface);

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);

    bool supportsRequiredQueueFamilies = true;
    supportsRequiredQueueFamilies &= queueFamilyIndices.graphics != -1;
    supportsRequiredQueueFamilies &= queueFamilyIndices.presentation != -1;
    if (!supportsRequiredQueueFamilies) { return false; }

    // device extension support
    u32 extensionCount;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr));
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data()));

    bool success = true;
    for (auto requiredExtension : PrototypeVulkanRenderer::requiredExtensions) {
        bool found = false;
        for (auto availableExtension : availableExtensions) {
            if (strcmp(requiredExtension, availableExtension.extensionName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            success = false;
            PrototypeLogger::fatal("Missing required extension %s", requiredExtension);
        }
    }
    if (!success) { return false; }

    bool swapchainMeetsRequirements = true;
    swapchainMeetsRequirements &= !swapchainDetails.formats.empty();
    swapchainMeetsRequirements &= !swapchainDetails.presentationModes.empty();
    if (!swapchainMeetsRequirements) {
        PrototypeLogger::fatal("Failed to meet swapchain requirements");
        return false;
    }

    if (!features.samplerAnisotropy) {
        PrototypeLogger::fatal("Failed to find feature <samplerAnisotropy>");
        return false;
    }

    return true;
}

bool
PrototypeVulkanRenderer::loadTexturesFromBuffers()
{
    _textures.resize(PrototypeEngineInternalApplication::database->textureBuffers.size());
    auto textureBuffersIt = PrototypeEngineInternalApplication::database->textureBuffers.begin();
    for (PtvTexture& texture : _textures) {
        if (!createTextureImage(texture, textureBuffersIt->second)) { return false; }
        if (!createTextureImageView(texture)) { return false; }
        if (!createTextureSampler(texture)) { return false; }
        ++textureBuffersIt;
    }
    return true;
}

void
PrototypeVulkanRenderer::clearTextures()
{
    for (PtvTexture& texture : _textures) {
        vkDestroySampler(_device, texture.sampler, nullptr);
        vkDestroyImageView(_device, texture.imageView, nullptr);
        vkDestroyImage(_device, texture.image, nullptr);
        vkFreeMemory(_device, texture.memory, nullptr);
    }
    _textures.clear();
}

bool
PrototypeVulkanRenderer::loadMeshesFromBuffers()
{
    _geometryBuffers.resize(PrototypeEngineInternalApplication::database->meshBuffers.size());
    auto meshBuffersIt = PrototypeEngineInternalApplication::database->meshBuffers.begin();
    for (PtvGeometryBuffer& geometryBuffer : _geometryBuffers) {
        if (!createGeometryBuffer(geometryBuffer, meshBuffersIt->second)) { return false; }
        ++meshBuffersIt;
    }
    return true;
}

void
PrototypeVulkanRenderer::clearMeshes()
{
    for (PtvGeometryBuffer& geometryBuffer : _geometryBuffers) {
        destroyBuffer(geometryBuffer.vertex);
        destroyBuffer(geometryBuffer.index);
    }
    _geometryBuffers.clear();
}

bool
PrototypeVulkanRenderer::createInstance()
{
    // platform extensions
    std::vector<const char*> platformExtensions;
    {
        u32          glfwRequiredExtensionsCount;
        const char** glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionsCount);
        platformExtensions =
          std::vector<const char*>(glfwRequiredExtensions, glfwRequiredExtensions + glfwRequiredExtensionsCount);
        platformExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

#if defined(PROTOTYPE_TARGET_DEBUG)
    // validation layers
    std::vector<VkLayerProperties> availableLayers;
    {
        u32 availableLayersCount;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayersCount, nullptr));
        availableLayers.resize(availableLayersCount);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayersCount, availableLayers.data()));
    }

    // check available layers are available
    bool success = true;
    for (auto requiredValidationLayer : PrototypeVulkanRenderer::requiredValidationLayers) {
        bool found = false;
        for (auto availableLayer : availableLayers) {
            if (strcmp(requiredValidationLayer, availableLayer.layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            success = false;
            PrototypeLogger::fatal("Missing required validation layer %s", requiredValidationLayer);
        }
    }
    if (!success) { return false; }
#endif

    VkApplicationInfo appInfo  = {};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion         = VK_API_VERSION_1_0;
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pApplicationName   = "PrototypeApplication";
    appInfo.pEngineName        = "PrototypeEngine";
    appInfo.engineVersion      = VK_MAKE_VERSION(0, 0, 1);

    VkInstanceCreateInfo instanceCreateInfo    = {};
    instanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo        = &appInfo;
    instanceCreateInfo.enabledExtensionCount   = (u32)platformExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = platformExtensions.data();
#if defined(PROTOTYPE_TARGET_DEBUG)
    instanceCreateInfo.enabledLayerCount   = (u32)requiredValidationLayers.size();
    instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
#else
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = nullptr;
#endif

    // Create instance
    VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &_instance));

    return true;
}

#if defined(PROTOTYPE_TARGET_DEBUG)
bool
PrototypeVulkanRenderer::createDebuggerUtilsMessenger()
{
    // Create debugger
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity                    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;
    debugCreateInfo.pUserData       = this;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");
    PROTOTYPE_ASSERT_MSG(func, "Failed to get address of vkCreateDebugUtilsMessengerEXT")
    func(_instance, &debugCreateInfo, nullptr, &_debugMessenger);

    return true;
}
#endif

bool
PrototypeVulkanRenderer::createSurface()
{
    // Create surface
    _window->createSurface(_instance, &_surface);

    return true;
}

bool
PrototypeVulkanRenderer::selectPhysicalDevice()
{
    u32 deviceCount;
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    if (deviceCount <= 0) {
        PrototypeLogger::fatal("Couldn't find any supported physical devices");
        return false;
    }
    vkEnumeratePhysicalDevices(_instance, &deviceCount, physicalDevices.data());
    for (u32 i = 0; i < deviceCount; ++i) {
        detectQueueFamilyIndices(physicalDevices[i], _surface, _queueFamilies.indices);
        if (meetsRequirements(physicalDevices[i], _surface, _queueFamilies.indices)) {
            _physicalDevice = physicalDevices[i];
            break;
        }
    }
    if (_physicalDevice == nullptr) {
        PrototypeLogger::fatal("Couldn't find a matching physical device");
        return false;
    }

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(_physicalDevice, &physicalDeviceProperties);
    _window->setPhysicalDeviceStr(
      std::string(physicalDeviceProperties.deviceName, physicalDeviceProperties.deviceName + VK_MAX_PHYSICAL_DEVICE_NAME_SIZE));
    _window->refreshTitleBar();

    return true;
}

bool
PrototypeVulkanRenderer::createLogicalDevice()
{
    detectQueueFamilyIndices(_physicalDevice, _surface, _queueFamilies.indices);

    std::vector<u32> queueIndices = { (u32)_queueFamilies.indices.graphics, (u32)_queueFamilies.indices.presentation };
    std::vector<std::vector<f32>> queuePriorities  = { { 1.0f }, { 1.0f } };
    VkPhysicalDeviceFeatures      deviceFeatures   = {};
    deviceFeatures.samplerAnisotropy               = VK_TRUE;
    std::vector<const char*> requireExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfo(queueIndices.size());
    for (u32 i = 0; i < queueIndices.size(); ++i) {
        queueCreateInfo[i].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[i].queueFamilyIndex = queueIndices[i];
        queueCreateInfo[i].queueCount       = 1;
        queueCreateInfo[i].flags            = 0;
        queueCreateInfo[i].pNext            = nullptr;
        queueCreateInfo[i].pQueuePriorities = queuePriorities[i].data();
    }

    VkDeviceCreateInfo deviceCreateInfo      = {};
    deviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount    = (u32)queueIndices.size();
    deviceCreateInfo.pQueueCreateInfos       = queueCreateInfo.data();
    deviceCreateInfo.pEnabledFeatures        = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount   = (u32)requireExtensionNames.size();
    deviceCreateInfo.ppEnabledExtensionNames = requireExtensionNames.data();
    deviceCreateInfo.pNext                   = nullptr;

#if defined(PROTOTYPE_TARGET_DEBUG)
    deviceCreateInfo.enabledLayerCount   = (u32)requiredValidationLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
#else
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
#endif
    VK_CHECK(vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device));

    vkGetDeviceQueue(_device, _queueFamilies.indices.graphics, 0, &_queueFamilies.queues.graphics);
    vkGetDeviceQueue(_device, _queueFamilies.indices.presentation, 0, &_queueFamilies.queues.presentation);

    return true;
}

bool
PrototypeVulkanRenderer::createSwapchain()
{
    auto cameraObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera);

    auto    cameraObject = *cameraObjects.begin();
    Camera* cam          = cameraObject->getCameraTrait();
    CameraSystemSetResolution(cam, _window->_resolution.x, _window->_resolution.y);

    PtvSwapchainSupportDetails swapchainDetails = querySwapchainSupportDetails(_physicalDevice, _surface);
    _swapchain.format                           = selectSwapSurfaceFormat(swapchainDetails.formats);
    _swapchain.presentMode                      = selectSwapPresentMode(swapchainDetails.presentationModes);
    _swapchain.extent                           = selectSwapExtent(swapchainDetails.capabilities, _window);

    u32 imageCount = swapchainDetails.capabilities.minImageCount + 1;
    if (swapchainDetails.capabilities.maxImageCount > 0 && imageCount > swapchainDetails.capabilities.maxImageCount) {
        imageCount = swapchainDetails.capabilities.maxImageCount;
    }

    // create swapchain
    std::vector<u32> queueFamilyIndices = { (u32)_queueFamilies.indices.graphics, (u32)_queueFamilies.indices.presentation };

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface                  = _surface;
    swapchainCreateInfo.minImageCount            = imageCount;
    swapchainCreateInfo.imageFormat              = _swapchain.format.format;
    swapchainCreateInfo.imageColorSpace          = _swapchain.format.colorSpace;
    swapchainCreateInfo.imageExtent              = _swapchain.extent;
    swapchainCreateInfo.imageArrayLayers         = 1;
    swapchainCreateInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (_queueFamilies.indices.graphics != _queueFamilies.indices.presentation) {
        swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = (u32)queueFamilyIndices.size();
        swapchainCreateInfo.pQueueFamilyIndices   = queueFamilyIndices.data();
    } else {
        swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices   = nullptr;
    }
    swapchainCreateInfo.preTransform   = swapchainDetails.capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode    = _swapchain.presentMode;
    swapchainCreateInfo.clipped        = VK_TRUE;
    swapchainCreateInfo.oldSwapchain   = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(_device, &swapchainCreateInfo, nullptr, &_swapchain.swapchain));

    u32 swapchainImagesCount;
    vkGetSwapchainImagesKHR(_device, _swapchain.swapchain, &swapchainImagesCount, nullptr);
    _swapchain.images.resize(swapchainImagesCount);
    vkGetSwapchainImagesKHR(_device, _swapchain.swapchain, &swapchainImagesCount, _swapchain.images.data());

    return true;
}

bool
PrototypeVulkanRenderer::cleanupSwapchain()
{
    vkDestroyImageView(_device, _depthTexture.imageView, nullptr);
    vkDestroyImage(_device, _depthTexture.image, nullptr);
    vkFreeMemory(_device, _depthTexture.memory, nullptr);

    for (auto framebuffer : _swapchain.framebuffers) { vkDestroyFramebuffer(_device, framebuffer, nullptr); }
    _swapchain.framebuffers.clear();

    vkFreeCommandBuffers(
      _device, _commandPools.reset.pool, (u32)_commandPools.reset.buffers.size(), _commandPools.reset.buffers.data());
    _commandPools.reset.buffers.clear();

    vkFreeCommandBuffers(
      _device, _commandPools.transient.pool, (u32)_commandPools.transient.buffers.size(), _commandPools.transient.buffers.data());
    _commandPools.transient.buffers.clear();

    vkFreeCommandBuffers(
      _device, _commandPools.standard.pool, (u32)_commandPools.standard.buffers.size(), _commandPools.standard.buffers.data());
    _commandPools.standard.buffers.clear();

    vkDestroyPipeline(_device, _graphicsPipeline.pipeline, nullptr);
    _graphicsPipeline.pipeline = nullptr;
    vkDestroyPipelineLayout(_device, _graphicsPipeline.layout, nullptr);
    _graphicsPipeline.layout = nullptr;

    vkDestroyRenderPass(_device, _renderPass, nullptr);
    _renderPass = nullptr;

    for (VkImageView& imageView : _swapchain.imageViews) { vkDestroyImageView(_device, imageView, nullptr); }
    _swapchain.imageViews.clear();

    vkDestroySwapchainKHR(_device, _swapchain.swapchain, nullptr);
    _swapchain.swapchain = nullptr;

    for (auto uniformBuffer : _uniformBuffers) {
        vkDestroyBuffer(_device, uniformBuffer.buffer, nullptr);
        vkFreeMemory(_device, uniformBuffer.memory, nullptr);
    }
    _uniformBuffers.clear();

    vkDestroyDescriptorPool(_device, _descriptor.pool, nullptr);

    vkDestroyDescriptorSetLayout(_device, _descriptor.matrices.layout, nullptr);
    vkDestroyDescriptorSetLayout(_device, _descriptor.materials.layout, nullptr);

    return true;
}

bool
PrototypeVulkanRenderer::recreateSwapchain(bool withBuffers)
{
    // handle window minimized case
    int width = 0, height = 0;
    glfwGetFramebufferSize(_window->_handle, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(_window->_handle, &width, &height);
        glfwWaitEvents();
    }
    // wait for device to become idle
    vkDeviceWaitIdle(_device);

    bool success = true;
    // cleanup
    success &= cleanupSwapchain();

    if (withBuffers) {
        clearMeshes();
        clearTextures();
        loadMeshesFromBuffers();
        loadTexturesFromBuffers();
    }

    // recreate
    success &= createSwapchain();
    success &= createSwapchainImageViews();
    success &= createRenderPass();
    success &= createDescriptorSetLayout();
    success &= createGraphicsPipeline();
    success &= createDepthResources();
    success &= createFramebuffers();
    success &= createUniformBuffers();
    success &= createDescriptorPool();
    success &= createDescriptorSets();
    success &= createCommandBuffers();

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    _ui->recreateVulkanData();
#endif

    return success;
}

bool
PrototypeVulkanRenderer::createSwapchainImageViews()
{
    _swapchain.imageViews.resize(_swapchain.images.size());
    for (u32 i = 0; i < _swapchain.imageViews.size(); ++i) {
        _swapchain.imageViews[i] = createImageView(_swapchain.images[i], _swapchain.format.format, VK_IMAGE_ASPECT_COLOR_BIT);
        // VkImageViewCreateInfo imageviewCreateInfo           = {};
        // imageviewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        // imageviewCreateInfo.image                           = _swapchain.images[i];
        // imageviewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        // imageviewCreateInfo.format                          = _swapchain.format.format;
        // imageviewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        // imageviewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        // imageviewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        // imageviewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        // imageviewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        // imageviewCreateInfo.subresourceRange.baseMipLevel   = 0;
        // imageviewCreateInfo.subresourceRange.levelCount     = 1;
        // imageviewCreateInfo.subresourceRange.baseArrayLayer = 0;
        // imageviewCreateInfo.subresourceRange.layerCount     = 1;

        // VK_CHECK(vkCreateImageView(_device, &imageviewCreateInfo, nullptr, &_swapchain.imageViews[i]));
    }

    return true;
}

bool
PrototypeVulkanRenderer::createShaders()
{
    for (const auto& pair : PrototypeEngineInternalApplication::database->shaderBuffers) {
        auto        shaderBuffer = pair.second;
        std::string vertexShaderSrc;
        std::string fragmentShaderSrc;
        for (const auto& source : shaderBuffer->sources()) {
            if (source->type == PrototypeShaderBufferSourceType_VertexShader) {
                vertexShaderSrc = source->code;
            } else if (source->type == PrototypeShaderBufferSourceType_FragmentShader) {
                fragmentShaderSrc = source->code;
            }
        }
        VkShaderModuleCreateInfo vertexShaderCreateInfo = {};
        vertexShaderCreateInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertexShaderCreateInfo.codeSize                 = vertexShaderSrc.size();
        vertexShaderCreateInfo.pCode                    = (u32*)vertexShaderSrc.c_str();

        VkShaderModuleCreateInfo fragmentShaderCreateInfo = {};
        fragmentShaderCreateInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragmentShaderCreateInfo.codeSize                 = fragmentShaderSrc.size();
        fragmentShaderCreateInfo.pCode                    = (u32*)fragmentShaderSrc.c_str();

        PtvShaderModule shaderModule = {};

        VK_CHECK(vkCreateShaderModule(_device, &vertexShaderCreateInfo, nullptr, &shaderModule.vertex));
        VK_CHECK(vkCreateShaderModule(_device, &fragmentShaderCreateInfo, nullptr, &shaderModule.fragment));

        // staging
        VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {};
        vertexShaderStageInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexShaderStageInfo.stage                           = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStageInfo.module                          = shaderModule.vertex;
        vertexShaderStageInfo.pName                           = "main";

        VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {};
        fragmentShaderStageInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentShaderStageInfo.stage                           = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStageInfo.module                          = shaderModule.fragment;
        fragmentShaderStageInfo.pName                           = "main";

        _shaders.push_back({ { vertexShaderStageInfo, fragmentShaderStageInfo }, shaderModule, shaderBuffer->name() });
    }
    return true;
}

bool
PrototypeVulkanRenderer::cleanupShaders()
{
    for (const auto& shader : _shaders) {
        vkDestroyShaderModule(_device, shader.module.vertex, nullptr);
        vkDestroyShaderModule(_device, shader.module.fragment, nullptr);
    }
    _shaders.clear();

    return true;
}

bool
PrototypeVulkanRenderer::createRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format                  = _swapchain.format.format;
    colorAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout             = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment            = 0;
    colorAttachmentRef.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format                  = PrototypeVulkanRenderer::findSupportedDepthFormat(_physicalDevice);
    depthAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment            = 1;
    depthAttachmentRef.layout                = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass    = {};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

    VkSubpassDependency dependency = {};
    dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass          = 0;
    dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask       = 0;
    dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount        = (u32)attachments.size();
    renderPassCreateInfo.pAttachments           = attachments.data();
    renderPassCreateInfo.subpassCount           = 1;
    renderPassCreateInfo.pSubpasses             = &subpass;
    renderPassCreateInfo.dependencyCount        = 1;
    renderPassCreateInfo.pDependencies          = &dependency;

    VK_CHECK(vkCreateRenderPass(_device, &renderPassCreateInfo, nullptr, &_renderPass));

    return true;
}

bool
PrototypeVulkanRenderer::createDescriptorSetLayout()
{
    // layout for matrices
    {
        // TODO:
        // Skelatal animation (Bones) modify descriptorCount ...
        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding                      = 0;
        uboLayoutBinding.descriptorCount              = 1;
        uboLayoutBinding.descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.stageFlags                   = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers           = nullptr;

        std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding };

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount                    = (u32)bindings.size();
        layoutCreateInfo.pBindings                       = bindings.data();

        VK_CHECK(vkCreateDescriptorSetLayout(_device, &layoutCreateInfo, nullptr, &_descriptor.matrices.layout));
    }

    // layout for material textures
    {
        // Color texture
        VkDescriptorSetLayoutBinding colorSamplerLayoutBinding = {};
        colorSamplerLayoutBinding.binding                      = 0;
        colorSamplerLayoutBinding.descriptorCount              = 1;
        colorSamplerLayoutBinding.descriptorType               = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        colorSamplerLayoutBinding.pImmutableSamplers           = nullptr;
        colorSamplerLayoutBinding.stageFlags                   = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::vector<VkDescriptorSetLayoutBinding> bindings = { colorSamplerLayoutBinding };

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount                    = (u32)bindings.size();
        layoutCreateInfo.pBindings                       = bindings.data();

        VK_CHECK(vkCreateDescriptorSetLayout(_device, &layoutCreateInfo, nullptr, &_descriptor.materials.layout));
    }

    return true;
}

bool
PrototypeVulkanRenderer::createGraphicsPipeline()
{
    if (!createShaders()) { return false; }

    auto bindingDescription   = getBindingDescription();
    auto attributeDesciptions = getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
    vertexInputCreateInfo.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount        = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions           = &bindingDescription;
    vertexInputCreateInfo.vertexAttributeDescriptionCount      = (u32)attributeDesciptions.size();
    vertexInputCreateInfo.pVertexAttributeDescriptions         = attributeDesciptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
    };
    inputAssemblyCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x          = 0.0f;
    viewport.y          = (f32)_swapchain.extent.height;
    viewport.width      = (f32)_swapchain.extent.width;
    viewport.height     = -(f32)_swapchain.extent.height;
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;

    VkRect2D scissor = {};
    scissor.offset   = { 0, 0 };
    scissor.extent   = _swapchain.extent;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount                     = 1;
    viewportStateCreateInfo.pViewports                        = &viewport;
    viewportStateCreateInfo.scissorCount                      = 1;
    viewportStateCreateInfo.pScissors                         = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
    };
    rasterizationCreateInfo.depthClampEnable        = VK_FALSE;
    rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationCreateInfo.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizationCreateInfo.lineWidth               = 1.0f;
    rasterizationCreateInfo.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizationCreateInfo.frontFace               = VK_FRONT_FACE_CLOCKWISE;
    rasterizationCreateInfo.depthBiasEnable         = VK_FALSE;
    rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationCreateInfo.depthBiasClamp          = 0.0f;
    rasterizationCreateInfo.depthBiasSlopeFactor    = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    multisamplingCreateInfo.sampleShadingEnable                  = VK_FALSE;
    multisamplingCreateInfo.rasterizationSamples                 = VK_SAMPLE_COUNT_1_BIT;
    multisamplingCreateInfo.minSampleShading                     = 1.0f;
    multisamplingCreateInfo.pSampleMask                          = nullptr;
    multisamplingCreateInfo.alphaToCoverageEnable                = VK_FALSE;
    multisamplingCreateInfo.alphaToOneEnable                     = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    const bool ENABLE_BLEND = false;
    if (ENABLE_BLEND) {
        colorBlendAttachment.blendEnable         = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    } else {
        colorBlendAttachment.blendEnable         = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    }
    colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
    colorBlendCreateInfo.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCreateInfo.logicOpEnable                       = VK_FALSE;
    colorBlendCreateInfo.logicOp                             = VK_LOGIC_OP_COPY;
    colorBlendCreateInfo.attachmentCount                     = 1;
    colorBlendCreateInfo.pAttachments                        = &colorBlendAttachment;
    colorBlendCreateInfo.blendConstants[0]                   = 0.0f;
    colorBlendCreateInfo.blendConstants[1]                   = 0.0f;
    colorBlendCreateInfo.blendConstants[2]                   = 0.0f;
    colorBlendCreateInfo.blendConstants[3]                   = 0.0f;

    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
    depthStencilCreateInfo.sType                                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilCreateInfo.depthTestEnable                       = VK_TRUE;
    depthStencilCreateInfo.depthWriteEnable                      = VK_TRUE;
    depthStencilCreateInfo.depthCompareOp                        = VK_COMPARE_OP_LESS;
    depthStencilCreateInfo.depthBoundsTestEnable                 = VK_FALSE;
    depthStencilCreateInfo.minDepthBounds                        = 0.0f;
    depthStencilCreateInfo.maxDepthBounds                        = 1.0f;
    depthStencilCreateInfo.stencilTestEnable                     = VK_FALSE;
    depthStencilCreateInfo.front                                 = {};
    depthStencilCreateInfo.back                                  = {};

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType                            = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount                = (u32)dynamicStates.size();
    dynamicStateCreateInfo.pDynamicStates                   = dynamicStates.data();

    // Define the push constant range used by the pipeline layout
    // Note that the spec only requires a minimum of 128 bytes, so for passing larger blocks of data you'd use UBOs or SSBOs
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags          = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset              = 0;
    pushConstantRange.size                = sizeof(PtvPushConstantData);

    std::array<VkDescriptorSetLayout, 2> setLayouts = { _descriptor.matrices.layout, _descriptor.materials.layout };

    VkPipelineLayoutCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineCreateInfo.setLayoutCount             = (u32)setLayouts.size();
    pipelineCreateInfo.pSetLayouts                = setLayouts.data();
    pipelineCreateInfo.pushConstantRangeCount     = 1;
    pipelineCreateInfo.pPushConstantRanges        = &pushConstantRange;

    VK_CHECK(vkCreatePipelineLayout(_device, &pipelineCreateInfo, nullptr, &_graphicsPipeline.layout));

    PtvShader mainShader;
    for (const auto& shader : _shaders) {
        if (shader.name == PROTOTYPE_DEFAULT_SHADER) {
            mainShader = shader;
            break;
        }
    }

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = { mainShader.stages.vertex, mainShader.stages.fragment };

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount                   = (u32)shaderStages.size(); // vertex and fragment stages only
    graphicsPipelineCreateInfo.pStages                      = shaderStages.data();
    graphicsPipelineCreateInfo.pVertexInputState            = &vertexInputCreateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState          = &inputAssemblyCreateInfo;
    graphicsPipelineCreateInfo.pViewportState               = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState          = &rasterizationCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState            = &multisamplingCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState           = &depthStencilCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState             = &colorBlendCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState                = nullptr;
    graphicsPipelineCreateInfo.layout                       = _graphicsPipeline.layout;
    graphicsPipelineCreateInfo.renderPass                   = _renderPass;
    graphicsPipelineCreateInfo.subpass                      = 0;
    graphicsPipelineCreateInfo.basePipelineHandle           = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex            = -1;

    VK_CHECK(
      vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &_graphicsPipeline.pipeline));

    if (!cleanupShaders()) { return false; }

    return true;
}

VkCommandBuffer
PrototypeVulkanRenderer::pushSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool                 = _commandPools.reset.pool;
    allocInfo.commandBufferCount          = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    _commandPools.reset.buffers.push_back(commandBuffer);

    return commandBuffer;
}

void
PrototypeVulkanRenderer::popSingleTimeCommands()
{
    VkCommandBuffer commandBuffer = _commandPools.reset.buffers.back();

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    vkQueueSubmit(_queueFamilies.queues.graphics, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_queueFamilies.queues.graphics);

    vkFreeCommandBuffers(_device, _commandPools.reset.pool, 1, &commandBuffer);

    _commandPools.reset.buffers.pop_back();
}

VkCommandBuffer
PrototypeVulkanRenderer::pushGraphicsCommand()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool                 = _commandPools.standard.pool;
    allocInfo.commandBufferCount          = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    _commandPools.standard.buffers.push_back(commandBuffer);

    return commandBuffer;
}

void
PrototypeVulkanRenderer::popGraphicsCommand()
{
    VkCommandBuffer commandBuffer = _commandPools.standard.buffers.back();

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    vkQueueSubmit(_queueFamilies.queues.graphics, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_queueFamilies.queues.graphics);

    vkFreeCommandBuffers(_device, _commandPools.standard.pool, 1, &commandBuffer);

    _commandPools.standard.buffers.pop_back();
}

bool
PrototypeVulkanRenderer::createImage(u32                   width,
                                     u32                   height,
                                     VkFormat              format,
                                     VkImageTiling         tiling,
                                     VkImageUsageFlags     usage,
                                     VkMemoryPropertyFlags properties,
                                     PtvTexture&           texture)
{
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType         = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width      = width;
    imageInfo.extent.height     = height;
    imageInfo.extent.depth      = 1;
    imageInfo.mipLevels         = 1;
    imageInfo.arrayLayers       = 1;
    imageInfo.format            = format;
    imageInfo.tiling            = tiling;
    imageInfo.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage             = usage;
    imageInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateImage(_device, &imageInfo, nullptr, &texture.image));

    VkMemoryRequirements memRequirements = {};
    vkGetImageMemoryRequirements(_device, texture.image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize       = memRequirements.size;
    allocInfo.memoryTypeIndex      = findMemoryType(_physicalDevice, memRequirements.memoryTypeBits, properties);

    VK_CHECK(vkAllocateMemory(_device, &allocInfo, nullptr, &texture.memory));

    vkBindImageMemory(_device, texture.image, texture.memory, 0);

    return true;
}

void
PrototypeVulkanRenderer::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = pushSingleTimeCommands();
    {
        VkImageMemoryBarrier barrier            = {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = oldLayout;
        barrier.newLayout                       = newLayout;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = image;
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;

        VkPipelineStageFlags srcStage = {};
        VkPipelineStageFlags dstStage = {};

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            PrototypeLogger::fatal("Unsupported layout transition");
        }

        vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }
    popSingleTimeCommands();
}

void
PrototypeVulkanRenderer::copyBufferToImage(VkBuffer buffer, VkImage image, u32 width, u32 height)
{
    VkCommandBuffer commandBuffer = pushSingleTimeCommands();
    {
        VkBufferImageCopy region               = {};
        region.bufferOffset                    = 0;
        region.bufferRowLength                 = 0;
        region.bufferImageHeight               = 0;
        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = 1;
        region.imageOffset                     = { 0, 0, 0 };
        region.imageExtent                     = { width, height, 1 };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }
    popSingleTimeCommands();
}

bool
PrototypeVulkanRenderer::createTextureImage(PtvTexture& texture, const PrototypeTextureBuffer* textureBuffer)
{
    auto         src       = textureBuffer->source();
    VkDeviceSize imageSize = src.width * src.height * src.components;

    PtvBuffer staging = {};
    createBuffer(imageSize,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 staging);

    void* data;
    vkMapMemory(_device, staging.memory, 0, imageSize, 0, &data);
    memcpy(data, src.data.data(), (size_t)imageSize);
    vkUnmapMemory(_device, staging.memory);

    createImage(src.width,
                src.height,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                texture);

    transitionImageLayout(
      texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(staging.buffer, texture.image, (u32)src.width, (u32)src.height);
    transitionImageLayout(
      texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    destroyBuffer(staging);

    return true;
}

VkImageView
PrototypeVulkanRenderer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo           = {};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = image;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = format;
    viewInfo.subresourceRange.aspectMask     = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    VkImageView imageView;
    VK_CHECK(vkCreateImageView(_device, &viewInfo, nullptr, &imageView));

    return imageView;
}

bool
PrototypeVulkanRenderer::createTextureImageView(PtvTexture& texture)
{
    texture.imageView = createImageView(texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

    return true;
}

bool
PrototypeVulkanRenderer::createTextureSampler(PtvTexture& texture)
{
    VkSamplerCreateInfo samplerInfo     = {};
    samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter               = VK_FILTER_LINEAR;
    samplerInfo.minFilter               = VK_FILTER_LINEAR;
    samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable        = VK_TRUE;
    samplerInfo.maxAnisotropy           = 16.0f;
    samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable           = VK_FALSE;
    samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VK_CHECK(vkCreateSampler(_device, &samplerInfo, nullptr, &texture.sampler));

    return true;
}

bool
PrototypeVulkanRenderer::createDepthResources()
{
    VkFormat depthFormat = PrototypeVulkanRenderer::findSupportedDepthFormat(_physicalDevice);
    createImage(_swapchain.extent.width,
                _swapchain.extent.height,
                depthFormat,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                _depthTexture);
    _depthTexture.imageView = createImageView(_depthTexture.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    return true;
}

bool
PrototypeVulkanRenderer::createFramebuffers()
{
    _swapchain.framebuffers.resize(_swapchain.imageViews.size());
    for (size_t i = 0; i < _swapchain.imageViews.size(); ++i) {
        std::vector<VkImageView> attachments           = { _swapchain.imageViews[i], _depthTexture.imageView };
        VkFramebufferCreateInfo  framebufferCreateInfo = {};
        framebufferCreateInfo.sType                    = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass               = _renderPass;
        framebufferCreateInfo.attachmentCount          = (u32)attachments.size();
        framebufferCreateInfo.pAttachments             = attachments.data();
        framebufferCreateInfo.width                    = _swapchain.extent.width;
        framebufferCreateInfo.height                   = _swapchain.extent.height;
        framebufferCreateInfo.layers                   = 1;

        VK_CHECK(vkCreateFramebuffer(_device, &framebufferCreateInfo, nullptr, &_swapchain.framebuffers[i]));
    }

    return true;
}

bool
PrototypeVulkanRenderer::createCommandPools()
{
    VkCommandPoolCreateInfo defaultCommandPoolCreateInfo = {};
    defaultCommandPoolCreateInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    defaultCommandPoolCreateInfo.queueFamilyIndex        = (u32)_queueFamilies.indices.graphics;
    defaultCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(_device, &defaultCommandPoolCreateInfo, nullptr, &_commandPools.standard.pool));

    VkCommandPoolCreateInfo transientCommandPoolCreateInfo = {};
    transientCommandPoolCreateInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    transientCommandPoolCreateInfo.queueFamilyIndex        = (u32)_queueFamilies.indices.graphics;
    transientCommandPoolCreateInfo.flags                   = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    VK_CHECK(vkCreateCommandPool(_device, &transientCommandPoolCreateInfo, nullptr, &_commandPools.transient.pool));

    VkCommandPoolCreateInfo resetCommandPoolCreateInfo = {};
    resetCommandPoolCreateInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    resetCommandPoolCreateInfo.queueFamilyIndex        = (u32)_queueFamilies.indices.graphics;
    resetCommandPoolCreateInfo.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(_device, &resetCommandPoolCreateInfo, nullptr, &_commandPools.reset.pool));

    return true;
}

bool
PrototypeVulkanRenderer::createCommandBuffers()
{
    {
        _commandPools.standard.buffers.resize(_swapchain.framebuffers.size());
        VkCommandBufferAllocateInfo allocCreateInfo = {};
        allocCreateInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocCreateInfo.commandPool                 = _commandPools.standard.pool;
        allocCreateInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocCreateInfo.commandBufferCount          = (u32)_commandPools.standard.buffers.size();
        vkAllocateCommandBuffers(_device, &allocCreateInfo, _commandPools.standard.buffers.data());
    }
    {
        _commandPools.transient.buffers.resize(_swapchain.framebuffers.size());
        VkCommandBufferAllocateInfo allocCreateInfo = {};
        allocCreateInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocCreateInfo.commandPool                 = _commandPools.transient.pool;
        allocCreateInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocCreateInfo.commandBufferCount          = (u32)_commandPools.transient.buffers.size();
        vkAllocateCommandBuffers(_device, &allocCreateInfo, _commandPools.transient.buffers.data());
    }
    recordCommandBuffers();

    return true;
}

bool
PrototypeVulkanRenderer::recreateCommandBuffers()
{
    vkDeviceWaitIdle(_device);

    vkFreeCommandBuffers(
      _device, _commandPools.transient.pool, (u32)_commandPools.transient.buffers.size(), _commandPools.transient.buffers.data());
    _commandPools.transient.buffers.clear();

    vkFreeCommandBuffers(
      _device, _commandPools.standard.pool, (u32)_commandPools.standard.buffers.size(), _commandPools.standard.buffers.data());
    _commandPools.standard.buffers.clear();

    return createCommandBuffers();
}

bool
PrototypeVulkanRenderer::recordCommandBuffers()
{
    auto meshRendererObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(
      PrototypeTraitTypeMaskMeshRenderer | PrototypeTraitTypeMaskTransform);

    std::vector<PrototypeObject*> renderedObjects(meshRendererObjects.begin(), meshRendererObjects.end());

    for (size_t i = 0; i < _commandPools.standard.buffers.size(); ++i) {
        u32 objectIndex = 0;

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags                    = 0;
        commandBufferBeginInfo.pInheritanceInfo         = nullptr;

        VK_CHECK(vkBeginCommandBuffer(_commandPools.standard.buffers[i], &commandBufferBeginInfo));
        {
            VkRenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass            = _renderPass;
            renderPassBeginInfo.framebuffer           = _swapchain.framebuffers[i];
            renderPassBeginInfo.renderArea.offset     = { 0, 0 };
            renderPassBeginInfo.renderArea.extent     = _swapchain.extent;
            std::array<VkClearValue, 2> clearValues   = {};
            clearValues[0].color                      = { 0.0f, 0.0f, 0.0f, 1.0f };
            clearValues[1].depthStencil               = { 1.0f, 0 };
            renderPassBeginInfo.clearValueCount       = (u32)clearValues.size();
            renderPassBeginInfo.pClearValues          = clearValues.data();

            vkCmdBeginRenderPass(_commandPools.standard.buffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
            {
                vkCmdBindPipeline(_commandPools.standard.buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline.pipeline);
                vkCmdBindDescriptorSets(_commandPools.standard.buffers[i],
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        _graphicsPipeline.layout,
                                        0,
                                        1,
                                        &_descriptor.matrices.sets[i],
                                        0,
                                        nullptr);
                // TODO:
                // Batch same vertex buffers which have the same graphics pipeline in single bind !!
                for (size_t geometryIndex = 0; geometryIndex < _geometryBuffers.size(); ++geometryIndex) {
                    for (size_t r = 0; r < renderedObjects.size(); ++r) {
                        MeshRenderer* mr = renderedObjects[r]->getMeshRendererTrait();
                        for (const auto& meshMaterialPair : mr->data()) {
                            if (meshMaterialPair.mesh == _geometryBuffers[geometryIndex].name) {
                                PtvPushConstantData data = {};
                                data.transformIndex      = (u32)r;
                                auto materialIt =
                                  PrototypeEngineInternalApplication::database->materials.find(meshMaterialPair.material);
                                data.materialIndex = (u32)materialIt->second->id();
                                vkCmdBindDescriptorSets(
                                  _commandPools.standard.buffers[i],
                                  VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  _graphicsPipeline.layout,
                                  1,
                                  1,
                                  &_descriptor.materials
                                     .sets[i * PrototypeEngineInternalApplication::database->materials.size() +
                                           std::distance(PrototypeEngineInternalApplication::database->materials.begin(),
                                                         materialIt)],
                                  0,
                                  nullptr);
                                vkCmdPushConstants(_commandPools.standard.buffers[i],
                                                   _graphicsPipeline.layout,
                                                   VK_SHADER_STAGE_VERTEX_BIT,
                                                   0,
                                                   sizeof(PtvPushConstantData),
                                                   &data);

                                const VkDeviceSize vertexOffsets[1] = { 0 };
                                const VkDeviceSize indexOffset      = 0;

                                vkCmdBindVertexBuffers(_commandPools.standard.buffers[i],
                                                       0,
                                                       1,
                                                       &_geometryBuffers[geometryIndex].vertex.buffer,
                                                       vertexOffsets);
                                vkCmdBindIndexBuffer(_commandPools.standard.buffers[i],
                                                     _geometryBuffers[geometryIndex].index.buffer,
                                                     indexOffset,
                                                     VK_INDEX_TYPE_UINT32);
                                vkCmdDrawIndexed(
                                  _commandPools.standard.buffers[i], (u32)_geometryBuffers[geometryIndex].indexCount, 1, 0, 0, 0);
                                ++objectIndex;
                            }
                        }
                    }
                }
            };
            vkCmdEndRenderPass(_commandPools.standard.buffers[i]);
        }
        VK_CHECK(vkEndCommandBuffer(_commandPools.standard.buffers[i]));
    }

    return true;
}

bool
PrototypeVulkanRenderer::createSemaphores()
{
    for (PtvSemaphores& semaphore : _synchronization.semaphores) {
        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &semaphore.imageAvailable));
        VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &semaphore.renderFinished));
    }

    return true;
}

bool
PrototypeVulkanRenderer::createFences()
{
    for (VkFence& fence : _synchronization.inFlightFences) {
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags             = VK_FENCE_CREATE_SIGNALED_BIT;
        VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &fence));
    }
    _synchronization.imagesInFlightFences.resize(_swapchain.images.size(), VK_NULL_HANDLE);

    return true;
}

bool
PrototypeVulkanRenderer::createBuffer(VkDeviceSize          size,
                                      VkBufferUsageFlags    usageFlags,
                                      VkMemoryPropertyFlags propertyFlags,
                                      PtvBuffer&            buffer)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size               = size;
    bufferCreateInfo.usage              = usageFlags;
    bufferCreateInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(_device, &bufferCreateInfo, nullptr, &buffer.buffer));

    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(_device, buffer.buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize       = memoryRequirements.size;
    allocInfo.memoryTypeIndex      = findMemoryType(_physicalDevice, memoryRequirements.memoryTypeBits, propertyFlags);

    VK_CHECK(vkAllocateMemory(_device, &allocInfo, nullptr, &buffer.memory));

    vkBindBufferMemory(_device, buffer.buffer, buffer.memory, 0);

    return true;
}

bool
PrototypeVulkanRenderer::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = pushSingleTimeCommands();
    {
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset    = 0;
        copyRegion.dstOffset    = 0;
        copyRegion.size         = size;

        vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);
    }
    popSingleTimeCommands();
    return true;
}

bool
PrototypeVulkanRenderer::destroyBuffer(PtvBuffer buffer)
{
    vkDestroyBuffer(_device, buffer.buffer, nullptr);
    vkFreeMemory(_device, buffer.memory, nullptr);

    return true;
}

bool
PrototypeVulkanRenderer::createGeometryBuffer(PtvGeometryBuffer& geometryBuffer, const PrototypeMeshBuffer* meshBuffer)
{
    // link names to help selection in record buffers ..
    geometryBuffer.name = meshBuffer->name();
    // don't forget to set geometry vertex count, used in drawing loop ..
    geometryBuffer.indexCount = (u32)meshBuffer->source().indices.size();
    // vertex buffers
    {
        VkDeviceSize bufferSize = sizeof(meshBuffer->source().vertices[0]) * meshBuffer->source().vertices.size();
        // create staging buffer
        PtvBuffer stagingBuffer = {};
        if (!createBuffer(bufferSize,
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          stagingBuffer)) {
            return false;
        }
        // Map memory
        void* data;
        vkMapMemory(_device, stagingBuffer.memory, 0, bufferSize, 0, &data);
        memcpy(data, meshBuffer->source().vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(_device, stagingBuffer.memory);
        // create the actual vertex buffer
        createBuffer(bufferSize,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     geometryBuffer.vertex);
        // copy staging buffer to actual vertex buffer
        copyBuffer(stagingBuffer.buffer, geometryBuffer.vertex.buffer, bufferSize);
        // destroy staging buffer
        destroyBuffer(stagingBuffer);
    }

    // index buffer
    {
        VkDeviceSize bufferSize = sizeof(meshBuffer->source().indices[0]) * meshBuffer->source().indices.size();
        // create staging buffer
        PtvBuffer stagingBuffer = {};
        if (!createBuffer(bufferSize,
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          stagingBuffer)) {
            return false;
        }
        // Map memory
        void* data;
        vkMapMemory(_device, stagingBuffer.memory, 0, bufferSize, 0, &data);
        memcpy(data, meshBuffer->source().indices.data(), (size_t)bufferSize);
        vkUnmapMemory(_device, stagingBuffer.memory);
        // create the actual index buffer
        createBuffer(bufferSize,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     geometryBuffer.index);
        // copy staging buffer to actual index buffer
        copyBuffer(stagingBuffer.buffer, geometryBuffer.index.buffer, bufferSize);
        // destroy staging buffer
        destroyBuffer(stagingBuffer);
    }
    return true;
}

bool
PrototypeVulkanRenderer::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(PtvUniformBufferObject);

    _uniformBuffers.resize(_swapchain.images.size());
    for (size_t i = 0; i < _uniformBuffers.size(); ++i) {
        createBuffer(bufferSize,
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     _uniformBuffers[i]);
    }
    return true;
}

bool
PrototypeVulkanRenderer::createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes = {};
    poolSizes[0].type                             = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount                  = (u32)_swapchain.images.size();
    poolSizes[1].type                             = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount                  = (u32)(
      _swapchain.images.size() * PrototypeEngineInternalApplication::database->materials.size() * 1); // (Color, Notmal, ...)

    VkDescriptorPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.poolSizeCount              = (u32)poolSizes.size();
    poolCreateInfo.pPoolSizes                 = poolSizes.data();
    poolCreateInfo.maxSets =
      (u32)(_swapchain.images.size() + _swapchain.images.size() * PrototypeEngineInternalApplication::database->materials.size());

    VK_CHECK(vkCreateDescriptorPool(_device, &poolCreateInfo, nullptr, &_descriptor.pool));

    return true;
}

bool
PrototypeVulkanRenderer::createDescriptorSets()
{
    // ubo ... matrices
    {
        std::vector<VkDescriptorSetLayout> layouts(_swapchain.images.size(), _descriptor.matrices.layout);
        VkDescriptorSetAllocateInfo        allocInfo = {};
        allocInfo.sType                              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool                     = _descriptor.pool;
        allocInfo.descriptorSetCount                 = (u32)_swapchain.images.size();
        allocInfo.pSetLayouts                        = layouts.data();

        _descriptor.matrices.sets.resize(_swapchain.images.size());
        VK_CHECK(vkAllocateDescriptorSets(_device, &allocInfo, _descriptor.matrices.sets.data()));

        for (size_t i = 0; i < _descriptor.matrices.sets.size(); i++) {
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer                 = _uniformBuffers[i].buffer;
            bufferInfo.offset                 = 0;
            bufferInfo.range                  = sizeof(PtvUniformBufferObject);

            VkWriteDescriptorSet uboWriteDescriptor = {};
            uboWriteDescriptor.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            uboWriteDescriptor.dstSet               = _descriptor.matrices.sets[i];
            uboWriteDescriptor.dstBinding           = 0;
            uboWriteDescriptor.dstArrayElement      = 0;
            uboWriteDescriptor.descriptorType       = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboWriteDescriptor.descriptorCount      = 1;
            uboWriteDescriptor.pBufferInfo          = &bufferInfo;

            vkUpdateDescriptorSets(_device, 1, &uboWriteDescriptor, 0, nullptr);
        }
    }

    // materials
    {
        std::vector<VkDescriptorSetLayout> layouts(_swapchain.images.size() *
                                                     PrototypeEngineInternalApplication::database->materials.size(),
                                                   _descriptor.materials.layout);
        VkDescriptorSetAllocateInfo        allocInfo = {};
        allocInfo.sType                              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool                     = _descriptor.pool;
        allocInfo.descriptorSetCount =
          (u32)(_swapchain.images.size() * PrototypeEngineInternalApplication::database->materials.size());
        allocInfo.pSetLayouts = layouts.data();

        _descriptor.materials.sets.resize(_swapchain.images.size() *
                                          PrototypeEngineInternalApplication::database->materials.size());
        VK_CHECK(vkAllocateDescriptorSets(_device, &allocInfo, _descriptor.materials.sets.data()));

        for (size_t i = 0; i < _swapchain.images.size(); ++i) {
            for (size_t m = 0; m < PrototypeEngineInternalApplication::database->materials.size(); ++m) {
                auto materialIt = PrototypeEngineInternalApplication::database->materials.begin();
                std::advance(materialIt, m);
                auto material = materialIt->second;

                size_t index        = i * PrototypeEngineInternalApplication::database->materials.size() + m;
                size_t textureIndex = 0;

                for (size_t t = 0; t < PrototypeEngineInternalApplication::database->textureBuffers.size(); ++t) {
                    auto textureBufferIt = PrototypeEngineInternalApplication::database->textureBuffers.begin();
                    std::advance(textureBufferIt, t);
                    auto textureBuffer = textureBufferIt->second;
                    if (material->textures()[0]->id() == textureBuffer->id()) {
                        textureIndex = t;
                        break;
                    }
                }

                VkDescriptorImageInfo colorMapInfo = {};
                colorMapInfo.imageLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                colorMapInfo.imageView             = _textures[textureIndex].imageView;
                colorMapInfo.sampler               = _textures[textureIndex].sampler;

                VkWriteDescriptorSet textureWriteDescriptor = {};
                textureWriteDescriptor.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                textureWriteDescriptor.dstSet               = _descriptor.materials.sets[index];
                textureWriteDescriptor.dstBinding           = 0; // sampler2D binding index in frag shader
                textureWriteDescriptor.dstArrayElement      = 0;
                textureWriteDescriptor.descriptorType       = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                textureWriteDescriptor.descriptorCount      = 1;
                textureWriteDescriptor.pImageInfo           = &colorMapInfo;

                std::array<VkWriteDescriptorSet, 1> writeDescriptors = { textureWriteDescriptor };

                vkUpdateDescriptorSets(_device, (u32)writeDescriptors.size(), writeDescriptors.data(), 0, nullptr);
            }
        }
    }

    return true;
}

bool
PrototypeVulkanRenderer::createGui()
{
    return true;
}

void
PrototypeVulkanRenderer::updateUniformBuffer(u32 currentImageIndex)
{
    auto meshRendererObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(
      PrototypeTraitTypeMaskMeshRenderer | PrototypeTraitTypeMaskTransform);

    std::vector<PrototypeObject*> renderedObjects(meshRendererObjects.begin(), meshRendererObjects.end());

    size_t                 modelCounter = 0;
    PtvUniformBufferObject ubo          = {};
    for (size_t r = 0; r < renderedObjects.size(); ++r) {
        Transform* transform    = renderedObjects[r]->getTransformTrait();
        ubo.transforms[r].model = transform->modelScaled();
    }

    Camera* cam           = _mainCamera.object->getCameraTrait();
    ubo.camera.view       = cam->viewMatrix();
    ubo.camera.projection = cam->projectionMatrix();

    // f / aspect_ratio, 0.0f, 0.0f, 0.0f,
    // 0.0f, -f, 0.0f, 0.0f,
    // 0.0f, 0.0f, far_plane / (near_plane - far_plane), -1.0f,
    // 0.0f, 0.0f, (near_plane * far_plane) / (near_plane - far_plane), 0.0f

    // std::cout << "View Matrix:\n";
    // std::cout   << ubo.camera.view[0][0] << " " << ubo.camera.view[0][1] << " " << ubo.camera.view[0][2] << " " <<
    // ubo.camera.view[0][3] << "\n"
    //             << ubo.camera.view[1][0] << " " << ubo.camera.view[1][1] << " " << ubo.camera.view[1][2] << " " <<
    //             ubo.camera.view[1][3] << "\n"
    //             << ubo.camera.view[2][0] << " " << ubo.camera.view[2][1] << " " << ubo.camera.view[2][2] << " " <<
    //             ubo.camera.view[2][3] << "\n"
    //             << ubo.camera.view[3][0] << " " << ubo.camera.view[3][1] << " " << ubo.camera.view[3][2] << " " <<
    //             ubo.camera.view[3][3] << "\n\n";

    // std::cout << "Projection Matrix:\n";
    // std::cout   << ubo.camera.projection[0][0] << " " << ubo.camera.projection[0][1] << " " << ubo.camera.projection[0][2] << "
    // " << ubo.camera.projection[0][3] << "\n"
    //             << ubo.camera.projection[1][0] << " " << ubo.camera.projection[1][1] << " " << ubo.camera.projection[1][2] << "
    //             " << ubo.camera.projection[1][3] << "\n"
    //             << ubo.camera.projection[2][0] << " " << ubo.camera.projection[2][1] << " " << ubo.camera.projection[2][2] << "
    //             " << ubo.camera.projection[2][3] << "\n"
    //             << ubo.camera.projection[3][0] << " " << ubo.camera.projection[3][1] << " " << ubo.camera.projection[3][2] << "
    //             " << ubo.camera.projection[3][3] << "\n\n";

    void* data;
    vkMapMemory(_device, _uniformBuffers[currentImageIndex].memory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(_device, _uniformBuffers[currentImageIndex].memory);
}

void
PrototypeVulkanRenderer::handleAllActions()
{
    for (const auto& action : _actions) { action(); }
    _actions.clear();
}

PvtCamera&
PrototypeVulkanRenderer::pvtMainCamera()
{
    return _mainCamera;
}
