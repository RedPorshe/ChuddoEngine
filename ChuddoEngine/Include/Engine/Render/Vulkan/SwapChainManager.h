#pragma once
#include "Render/Vulkan/VulkanContext.h"

class VulkanDevice;

class SwapChainManager
{
public:
    SwapChainManager() = default;
    ~SwapChainManager() = default;

    // Initialize swapchain, image views, render pass, pipeline layout and framebuffers
    bool Initialize(VulkanContextInfo &info, VulkanDevice * device);
    void Shutdown(VulkanContextInfo &info);
    bool CreateFramebuffers(VulkanContextInfo &info);
    // Create renderpass, pipeline layout and framebuffers after depth/color images are ready
    bool CreateRenderPassAndFramebuffers(VulkanContextInfo &info, VulkanDevice* device);
};
