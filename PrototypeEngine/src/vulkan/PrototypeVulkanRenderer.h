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

#include "PrototypeVulkan.h"

#include "../core/PrototypeRenderer.h"

#include "PrototypeVulkanUI.h"

#include <PrototypeCommon/Maths.h>

#include <array>
#include <functional>
#include <map>
#include <vector>

struct PrototypeObject;

struct PrototypeVulkanWindow;
struct PrototypeMeshBuffer;
struct PrototypeShaderBuffer;
struct PrototypeTextureBuffer;
struct PrototypeMaterial;

struct PrototypeSceneNode;

typedef std::pair<PrototypeSceneNode*, PrototypeObject*> NodeObjectPair;

struct PrototypeVulkanRenderer final : PrototypeRenderer
{
    explicit PrototypeVulkanRenderer(PrototypeVulkanWindow* window);

    ~PrototypeVulkanRenderer() final = default;

    // initializes a window
    bool init() final;

    // de-initializes a window
    void deInit() final;

    // updates the renderer, update ubos
    bool update() final;

    // render 3D objects
    bool render3D() final;

    // render ui
    bool render2D() final;

    // switches scenes
    void switchScenes(PrototypeScene* scene) final;

    // schedule a rendering pass
    void scheduleRecordPass() final;

    // start recording instructions
    void beginRecordPass() final;

    // stop recording instructions
    void endRecordPass() final;

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    // get ui
    PrototypeUI* ui() final;
#endif

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    // Camera* editorGameCamera() final;
    Camera* editorSceneCamera() final;
    // Camera* editorPaintCamera() final;
#else
    Camera*         mainCamera() final;
#endif

    // call it to map buffers to gpu data
    void mapPrototypeMeshBuffer(PrototypeMeshBuffer* meshBuffer) final;

    // call it to map buffers to gpu data
    void mapPrototypeShaderBuffer(PrototypeShaderBuffer* shaderBuffer) final;

    // call it to map buffers to gpu data
    void mapPrototypeTextureBuffer(PrototypeTextureBuffer* textureBuffer) final;

    // call it to map buffers to gpu data
    void mapPrototypeMaterial(PrototypeMaterial* material) final;

    // called when a mesh buffer data needs to get uploaded to gpu memory
    void onMeshBufferGpuUpload(PrototypeMeshBuffer* meshBuffer) final;

    // called when a shader buffer data needs to get uploaded to gpu memory
    void onShaderBufferGpuUpload(PrototypeShaderBuffer* shaderBuffer) final;

    // called when a texture buffer data needs to get uploaded to gpu memory
    void onTextureBufferGpuUpload(PrototypeTextureBuffer* textureBuffer) final;

    // fetch a camera
    void fetchCamera(const std::string& name, void** data) final;

    // fetch the default mesh
    void fetchDefaultMesh(void** data) final;

    // fetch a mesh by name
    void fetchMesh(const std::string& name, void** data) final;

    // fetch the default shader
    void fetchDefaultShader(void** data) final;

    // fetch a shader by name
    void fetchShader(const std::string& name, void** data) final;

    // fetch the default texture
    void fetchDefaultTexture(void** data) final;

    // fetch a texture by name
    void fetchTexture(const std::string& name, void** data) final;

    // fetch the default material
    void fetchDefaultMaterial(void** data) final;

    // fetch a material by name
    void fetchMaterial(const std::string& name, void** data) final;

    // fetch the default framebuffer
    void fetchDefaultFramebuffer(void** data) final;

    // fetch a framebuffer by name
    void fetchFramebuffer(const std::string& name, void** data) final;

    // called when click triggers on the rendering viewport
    // [[nodiscard]] PrototypeSceneNode* onSceneViewClick(const glm::vec2& coordinates, const glm::vec2& Size) final;

    // called when mouse clicks events triggers
    void onMouse(i32 button, i32 action, i32 mods) final;

    // called when mouse cursor movement event triggers
    void onMouseMove(f64 xpos, f64 ypos) final;

    // called when mouse drag event triggers
    void onMouseDrag(i32 button, f64 xoffset, f64 yoffset) final;

    // called when mouse scroll event triggers
    void onMouseScroll(f64 xoffset, f64 yoffset) final;

    // called when keyboard keys events trigger
    void onKeyboard(i32 key, i32 scancode, i32 action, i32 mods) final;

    // called when window resize event triggers
    void onWindowResize(i32 width, i32 height) final;

    // called when drag and dropping files event triggers
    void onWindowDragDrop(i32 numFiles, const char** names) final;

    // get the camera
    PvtCamera& pvtMainCamera();

  private:
    friend struct PrototypeVulkanWindow;
    friend struct PrototypeVulkanUI;

    static const std::vector<const char*> requiredValidationLayers;
    static const std::vector<const char*> requiredExtensions;

    static PtvSwapchainSupportDetails querySwapchainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    static VkSurfaceFormatKHR         selectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
    static VkPresentModeKHR           selectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    static VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, PrototypeVulkanWindow* window);
    static void       detectQueueFamilyIndices(VkPhysicalDevice device, VkSurfaceKHR surface, PtvQueueFamilyIndices& indices);
    static bool       meetsRequirements(VkPhysicalDevice device, VkSurfaceKHR surface, const PtvQueueFamilyIndices& indices);
    static u32        findMemoryType(VkPhysicalDevice physicalDevice, u32 typeFilter, VkMemoryPropertyFlags propertyFlags);
    static VkFormat   findSupportedFormat(VkPhysicalDevice             physicalDevice,
                                          const std::vector<VkFormat>& candidates,
                                          VkImageTiling                tiling,
                                          VkFormatFeatureFlags         features);
    static VkFormat   findSupportedDepthFormat(VkPhysicalDevice physicalDevice);
    static bool       hasStencilComponent(VkFormat format);

    bool loadTexturesFromBuffers();
    void clearTextures();
    bool loadMeshesFromBuffers();
    void clearMeshes();

    bool createInstance();
#if defined(PROTOTYPE_TARGET_DEBUG)
    bool createDebuggerUtilsMessenger();
#endif
    bool            createSurface();
    bool            selectPhysicalDevice();
    bool            createLogicalDevice();
    bool            createSwapchain();
    bool            cleanupSwapchain();
    bool            recreateSwapchain(bool withBuffers = false);
    bool            createSwapchainImageViews();
    bool            createShaders();
    bool            cleanupShaders();
    bool            createRenderPass();
    bool            createDescriptorSetLayout();
    bool            createGraphicsPipeline();
    VkCommandBuffer pushSingleTimeCommands();
    void            popSingleTimeCommands();
    VkCommandBuffer pushGraphicsCommand();
    void            popGraphicsCommand();
    bool            createImage(u32                   width,
                                u32                   height,
                                VkFormat              format,
                                VkImageTiling         tiling,
                                VkImageUsageFlags     usage,
                                VkMemoryPropertyFlags properties,
                                PtvTexture&           texture);
    void            transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void            copyBufferToImage(VkBuffer buffer, VkImage image, u32 width, u32 height);
    bool            createTextureImage(PtvTexture& texture, const PrototypeTextureBuffer* textureBuffer);
    VkImageView     createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    bool            createTextureImageView(PtvTexture& texture);
    bool            createTextureSampler(PtvTexture& texture);
    bool            createDepthResources();
    bool            createFramebuffers();
    bool            createCommandPools();
    bool            createCommandBuffers();
    bool            recreateCommandBuffers();
    bool            recordCommandBuffers();
    bool            createSemaphores();
    bool            createFences();
    bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, PtvBuffer& buffer);
    bool copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
    bool destroyBuffer(PtvBuffer buffer);
    bool createGeometryBuffer(PtvGeometryBuffer& geometryBufer, const PrototypeMeshBuffer* meshBuffer);
    bool createUniformBuffers();
    bool createDescriptorPool();
    bool createDescriptorSets();
    bool createGui();
    void updateUniformBuffer(u32 currentImageIndex);
    void handleAllActions();

    PtvSwapchain                       _swapchain;        // => 100 bytes <=
    PtvCommandPools                    _commandPools;     // => 96 bytes
    PrototypeUIState_                  _uiState;          // 4 bytes
    PtvDescriptor                      _descriptor;       // => 88 bytes <=
    PrototypeVulkanWindow*             _window;           // 8 bytes
    PtvSynchronization                 _synchronization;  // => 72 bytes <=
    PtvQueueFamilies                   _queueFamilies;    // 24 bytes
    PtvTexture                         _depthTexture;     // => 32 bytes <=
    PvtCamera                          _mainCamera;       // 32 bytes
    std::vector<PtvShader>             _shaders;          // 24 bytes
    VkInstance                         _instance;         // 8 bytes
    std::vector<std::function<void()>> _actions;          // => 24 bytes <=
    std::vector<PtvGeometryBuffer>     _geometryBuffers;  // 24 bytes
    std::vector<PtvBuffer>             _uniformBuffers;   // 24 bytes
    std::vector<PtvTexture>            _textures;         // 24 bytes
    PtvGraphicsPipeline                _graphicsPipeline; // => 16 bytes <=
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    std::unique_ptr<PrototypeVulkanUI> _ui; // 8 bytes
#else
    PrototypeInput* _input;
#endif
    VkDebugUtilsMessengerEXT _debugMessenger;     // 8 bytes
    VkSurfaceKHR             _surface;            // 8 bytes
    VkPhysicalDevice         _physicalDevice;     // 8 bytes
    VkDevice                 _device;             // 8 bytes
    VkRenderPass             _renderPass;         // 8 bytes
    u64                      _currentFrame;       // 8 bytes
    bool                     _framebufferResized; // 1 byte
    bool                     _needsRecord;        // 1 byte
};
