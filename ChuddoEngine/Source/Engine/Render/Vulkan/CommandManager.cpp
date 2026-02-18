#include "Render/Vulkan/CommandManager.h"
#include <vector>

bool CommandManager::Initialize(VulkanContextInfo &info)
{
    // Nothing heavy here - command pools are created by VulkanDevice
    return true;
}

void CommandManager::Shutdown(VulkanContextInfo &info)
{
    // Command buffers will be freed when swapchain and pools are destroyed
    info.CommandBuffers.clear();
}

bool CommandManager::AllocateCommandBuffers(VulkanContextInfo &info)
{
    if (info.GraphicsCommandPool == VK_NULL_HANDLE)
        return false;

    size_t count = info.SwapChainImages.size();
    info.CommandBuffers.resize(count, VK_NULL_HANDLE);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = info.GraphicsCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(count);

    if (vkAllocateCommandBuffers(info.Device, &allocInfo, info.CommandBuffers.data()) != VK_SUCCESS)
        return false;

    return true;
}
