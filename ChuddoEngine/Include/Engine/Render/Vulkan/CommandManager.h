#pragma once
#include "Render/Vulkan/VulkanContext.h"
#include <vulkan/vulkan.h>

class CommandManager
{
public:
    CommandManager() = default;
    ~CommandManager() = default;

    bool Initialize(VulkanContextInfo &info);
    void Shutdown(VulkanContextInfo &info);

    // Allocate command buffers for swapchain images
    bool AllocateCommandBuffers(VulkanContextInfo &info);
};
