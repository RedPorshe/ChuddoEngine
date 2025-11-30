#pragma once
#include <memory>
#include <vector>

#include "CoreMinimal.h"
#include "Engine/Core/Rendering/Vulkan/Managers/DeviceManager.h"
#include "Engine/Core/Rendering/Vulkan/Utils/VulkanUtils.h"
#include "vulkan/vulkan.h"

// Forward declare SDL type
struct SDL_Window;


  class SwapchainManager
  {
   public:
    SwapchainManager(VkInstance instance, VkSurfaceKHR surface, std::shared_ptr<DeviceManager> deviceManager, SDL_Window* window);
    ~SwapchainManager();

    // Удаляем копирование
    SwapchainManager(const SwapchainManager&) = delete;
    SwapchainManager& operator=(const SwapchainManager&) = delete;

    bool Initialize();
    void Cleanup();
    void RecreateSwapchain();

    // Getters
    VkSwapchainKHR GetSwapchain() const
    {
      return m_swapchain;
    }
    VkFormat GetImageFormat() const
    {
      return m_swapchainImageFormat;
    }
    VkFormat GetDepthFormat() const
    {
      return m_depthFormat;
    }
    VkExtent2D GetExtent() const
    {
      return m_swapchainExtent;
    }
    const std::vector<VkImage>& GetImages() const
    {
      return m_swapchainImages;
    }
    const std::vector<VkImageView>& GetImageViews() const
    {
      return m_swapchainImageViews;
    }
    const std::vector<VkFramebuffer>& GetFramebuffers() const
    {
      return m_swapchainFramebuffers;
    }
    VkRenderPass GetRenderPass() const
    {
      return m_renderPass;
    }
    VkImageView GetDepthImageView() const
    {
      return m_depthImageView;
    }
    uint32_t GetImageCount() const
    {
      return static_cast<uint32_t>(m_swapchainImages.size());
    }

   private:
    void CreateSwapchain();
    void CreateImageViews();
    void CreateDepthResources();
    void CreateRenderPass();
    void CreateFramebuffers();
    void CleanupSwapchain();

    // Helper functions
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkFormat FindDepthFormat();
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

   private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    std::shared_ptr<DeviceManager> m_deviceManager;

    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkFormat m_swapchainImageFormat;
    VkFormat m_depthFormat;
    VkExtent2D m_swapchainExtent;

    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;
    std::vector<VkFramebuffer> m_swapchainFramebuffers;

    // Depth resources
    VkImage m_depthImage = VK_NULL_HANDLE;
    VkDeviceMemory m_depthImageMemory = VK_NULL_HANDLE;
    VkImageView m_depthImageView = VK_NULL_HANDLE;

    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    // Window pointer used to query actual framebuffer size when needed
    SDL_Window* m_window = nullptr;
  };