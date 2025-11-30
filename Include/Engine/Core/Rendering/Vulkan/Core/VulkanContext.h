#pragma once
#include <memory>
#include <unordered_map>

#include <SDL3/SDL.h>

#include "CoreMinimal.h"
#include "Engine/Core/Rendering/Data/RenderData.h"
#include "Engine/Core/Rendering/Vulkan/Managers/BufferManager.h"
#include "Engine/Core/Rendering/Vulkan/Managers/CommandBufferManager.h"
#include "Engine/Core/Rendering/Vulkan/Managers/DescriptorManager.h"
#include "Engine/Core/Rendering/Vulkan/Managers/DeviceManager.h"
#include "Engine/Core/Rendering/Vulkan/Managers/PipelineManager.h"
#include "Engine/Core/Rendering/Vulkan/Managers/SwapchainManager.h"
#include "Engine/Core/Rendering/Vulkan/Utils/VulkanUtils.h"
#include "vulkan/vulkan.h"

struct GLFWwindow;


  struct MeshBuffers
  {
    std::string vertexBufferName;
    std::string indexBufferName;
    std::string modelUBOName;
  };

  class VulkanContext
  {
   public:
    VulkanContext(AppInfo* info);
    ~VulkanContext() {};

    SDL_Window* GetWindow() const
    {
      return m_window;
    }
    void Initialize();
    void Shutdown();
    void DrawFrame(const FrameRenderData& renderData);
    bool ShouldClose() const;
    void Pollevents()
    {
      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
        if (event.type == SDL_EVENT_QUIT)
        {
          m_shouldClose = true;
        }
        // Handle other events if needed
      }
    }
    // Called by the GLFW framebuffer-size callback when the window changes
    // size so the rendering code can recreate the swapchain on next frame.
    void OnFramebufferResized()
    {
      m_frameBufferResized = true;
    }
    void RegisterMesh(const std::string& name, const FStaticMesh& mesh);
    void UnregisterMesh(const std::string& name);

    // Getters for ImGui integration
    VkInstance GetInstance() const { return m_instance; }
    VkPhysicalDevice GetPhysicalDevice() const { return m_deviceManager->GetPhysicalDevice(); }
    VkDevice GetDevice() const { return m_deviceManager->GetDevice(); }
    uint32_t GetGraphicsQueueFamily() const { return m_deviceManager->getIndices().graphicsFamily; }
    VkQueue GetGraphicsQueue() const { return m_deviceManager->GetGraphicsQueue(); }
    VkDescriptorPool GetDescriptorPool() const { return m_descriptorManager->GetDescriptorPool(); }
    uint32_t GetSwapchainImageCount() const { return m_swapchainManager->GetImageCount(); }
    VkRenderPass GetRenderPass() const { return m_swapchainManager->GetRenderPass(); }
    VkCommandBuffer BeginSingleTimeCommands() { return m_commandBufferManager->BeginSingleTimeCommands(); }
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer) { m_commandBufferManager->EndSingleTimeCommands(commandBuffer); }
    VkCommandBuffer GetCurrentCommandBuffer() const { return m_currentCommandBuffer; }

   private:
    bool InitWindow();
    bool CreateInstance();
    bool SetupDebugMessenger();
    bool CreateSurface();
    void CreateSyncObjects();
    void CleanupSyncObjects();
    void RecordCommandBuffer(uint32_t imageIndex, const FrameRenderData& renderData);
    void UpdateUniformBuffers(const FrameRenderData& renderData);

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

   private:
    AppInfo* m_info = nullptr;
    SDL_Window* m_window = nullptr;

    VkInstance m_instance = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<SwapchainManager> m_swapchainManager;
    std::shared_ptr<PipelineManager> m_pipelineManager;
    std::shared_ptr<BufferManager> m_bufferManager;
    std::shared_ptr<DescriptorManager> m_descriptorManager;
    std::shared_ptr<CommandBufferManager> m_commandBufferManager;

    // Sync objects
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_imagesInFlight;
    uint32_t m_currentFrame = 0;
    bool m_frameBufferResized = false;
    bool m_shouldClose = false;

    // Buffers
    std::unordered_map<std::string, MeshBuffers> m_meshBufferMap;
    const std::string m_sceneUBOBufferName = "scene_ubo";
    const std::string m_lightingUBOBufferName = "lighting_ubo";

    // Current command buffer for ImGui
    VkCommandBuffer m_currentCommandBuffer = VK_NULL_HANDLE;

    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;
    const bool bIsValidationEnabled = true;
  };
