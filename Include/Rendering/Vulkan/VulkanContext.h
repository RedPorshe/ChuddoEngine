#pragma once
#include "Core/AppInfo.h"
#include "CoreMinimal.h"
#include "Rendering/Data/RenderData.h"
#include "Rendering/Vulkan/DeviceManager.h"
#include "Rendering/Vulkan/PipelineManager.h"
#include "Rendering/Vulkan/SwapchainManager.h"
#include "Rendering/Vulkan/VulkanUtils.h"

namespace CE
{
  class VulkanContext
  {
   public:
    VulkanContext(AppInfo* info);
    ~VulkanContext() = default;

    void Initialize();
    void Shutdown();
    void DrawFrame(const FrameRenderData& renderData);
    bool ShouldClose() const;

   private:
    // funcs
    bool InitWindow();
    bool CreateInstance();
    bool SetupDebugMessenger();
    bool CreateSurface();
    // Debug callback
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

   private:
    // properties and variables
    VkInstance m_instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT m_debugMessenger{VK_NULL_HANDLE};
    AppInfo* m_info = nullptr;
    GLFWwindow* m_window = nullptr;
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};

    // Меняем на shared_ptr для совместимости
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<SwapchainManager> m_swapchainManager;
    std::shared_ptr<PipelineManager> m_pipelineManager;

#ifdef _DEBUG
    bool bIsValidationEnabled{true};
#else
    bool bIsValidationEnabled{false};
#endif
  };
}  // namespace CE