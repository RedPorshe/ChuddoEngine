#pragma once

// GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Core Vulkan
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

// Стандартные контейнеры
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace CE
{

  struct QueueFamilyIndices
  {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() const
    {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

  struct SwapChainSupportDetails
  {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  struct VKContext
  {
    // main vk
    VkInstance Instance{VK_NULL_HANDLE};
    VkPhysicalDevice physDevice{VK_NULL_HANDLE};
    VkDevice Device{VK_NULL_HANDLE};
    VkSurfaceKHR Surface{VK_NULL_HANDLE};
    VkSwapchainKHR SwapShain{VK_NULL_HANDLE};
    // glfw
    GLFWwindow* Window = nullptr;
    bool framebufferResized = false;
    // queues
    VkQueue graphicsQueue{VK_NULL_HANDLE};
    VkQueue presentQueue{VK_NULL_HANDLE};
    uint32_t graphicsQueueFamily = 0;
    uint32_t presentQueueFamily = 0;
    // render pass
    VkRenderPass RenderPass{VK_NULL_HANDLE};
    // pipelines
    std::unordered_map<std::string, VkPipeline> Pipelines;
    std::unordered_map<std::string, VkPipelineLayout> PipelineLayouts;
    // Swapchain details
    VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent = {0, 0};
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    // command pool
    VkCommandPool CommandPool{VK_NULL_HANDLE};
    std::vector<VkCommandBuffer> CommandBuffers;
    // sync objects
    std::vector<VkSemaphore> ImageAvailableSemaphores;
    std::vector<VkSemaphore> RenderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> FrameFences;

    // frame Managment
    uint32_t CurrentFrame = 0;
    uint32_t imageIndex = 0;
    static constexpr float MAX_DELTA_TIME = 0.1f;
    static constexpr int MAX_FRAMES_IN_FLIGHT = 3;
    void Cleanup();

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
  };
}  // namespace CE