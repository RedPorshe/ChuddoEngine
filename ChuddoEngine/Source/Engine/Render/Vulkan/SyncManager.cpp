#include "Render/Vulkan/SyncManager.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

bool SyncManager::Initialize(VulkanContextInfo &info)
{
    // Create semaphores and fences for max frames in flight
    size_t maxFrames = static_cast<size_t>(info.MaxFramesInFlight);
    info.ImageAvailableSemaphores.resize(maxFrames, VK_NULL_HANDLE);
    info.RenderFinishedSemaphores.resize(maxFrames, VK_NULL_HANDLE);
    info.InFlightFences.resize(maxFrames, VK_NULL_HANDLE);
    info.ImagesInFlight.resize(info.SwapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < maxFrames; ++i)
    {
        if (vkCreateSemaphore(info.Device, &semaphoreInfo, nullptr, &info.ImageAvailableSemaphores[i]) != VK_SUCCESS)
        {
            return false;
        }
        if (vkCreateSemaphore(info.Device, &semaphoreInfo, nullptr, &info.RenderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            return false;
        }
        if (vkCreateFence(info.Device, &fenceInfo, nullptr, &info.InFlightFences[i]) != VK_SUCCESS)
        {
            return false;
        }
    }

    return true;
}

void SyncManager::Shutdown(VulkanContextInfo &info)
{
    for (auto & sem : info.ImageAvailableSemaphores)
    {
        if (sem != VK_NULL_HANDLE)
            vkDestroySemaphore(info.Device, sem, nullptr);
    }
    for (auto & sem : info.RenderFinishedSemaphores)
    {
        if (sem != VK_NULL_HANDLE)
            vkDestroySemaphore(info.Device, sem, nullptr);
    }
    for (auto & f : info.InFlightFences)
    {
        if (f != VK_NULL_HANDLE)
            vkDestroyFence(info.Device, f, nullptr);
    }

    info.ImageAvailableSemaphores.clear();
    info.RenderFinishedSemaphores.clear();
    info.InFlightFences.clear();
    info.ImagesInFlight.clear();
}

void SyncManager::ResizeImageTracking(VulkanContextInfo &info, size_t imageCount)
{
    info.ImagesInFlight.assign(imageCount, VK_NULL_HANDLE);
}
