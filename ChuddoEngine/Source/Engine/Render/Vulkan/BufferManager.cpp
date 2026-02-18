#include "Render/Vulkan/BufferManager.h"
#include "Render/Vulkan/VulkanDevice.h"
#include <vulkan/vulkan.h>

bool BufferManager::Initialize(VulkanContextInfo &info)
{
    // Placeholder for buffer creation utilities (vertex/index/uniform). No global buffers currently.
    return true;
}

void BufferManager::Shutdown(VulkanContextInfo &info)
{
    // Nothing to free at the manager level yet
}



