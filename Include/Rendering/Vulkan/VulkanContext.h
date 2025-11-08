#pragma once
#include <memory>
#include <unordered_map>

#include "CoreMinimal.h"
#include "Rendering/Data/RenderData.h"
#include "Rendering/Vulkan/BufferManager.h"
#include "Rendering/Vulkan/CommandBufferManager.h"
#include "Rendering/Vulkan/DescriptorManager.h"
#include "Rendering/Vulkan/DeviceManager.h"
#include "Rendering/Vulkan/PipelineManager.h"
#include "Rendering/Vulkan/SwapchainManager.h"
#include "Rendering/Vulkan/VulkanUtils.h"
#include "vulkan/vulkan.h"

struct GLFWwindow;

namespace CE
{
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

    void Initialize();
    void Shutdown();
    void DrawFrame(const FrameRenderData& renderData);
    bool ShouldClose() const;
    void Pollevents()
    {
      glfwPollEvents();
    }
    void RegisterMesh(const std::string& name, const StaticMesh& mesh);
    void UnregisterMesh(const std::string& name);

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
    GLFWwindow* m_window = nullptr;

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

    // Buffers
    std::unordered_map<std::string, MeshBuffers> m_meshBufferMap;
    const std::string m_sceneUBOBufferName = "scene_ubo";
    const std::string m_lightingUBOBufferName = "lighting_ubo";

    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;
    const bool bIsValidationEnabled = true;
  };
}  // namespace CE