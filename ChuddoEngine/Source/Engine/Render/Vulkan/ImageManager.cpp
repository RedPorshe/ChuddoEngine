#include "Render/Vulkan/ImageManager.h"
#include "Render/Vulkan/VulkanDevice.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

static VkFormat FindDepthFormat(VulkanDevice* device)
{
    std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };
    return device->FindSupportedFormat(candidates, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool ImageManager::Initialize(VulkanContextInfo &info, VulkanDevice* device)
{
    if (!device)
        return false;
    return RecreateDepthImage(info, device);
}

bool ImageManager::RecreateDepthImage(VulkanContextInfo &info, VulkanDevice* device)
{
    // Destroy previous
    if (info.DepthImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(info.Device, info.DepthImageView, nullptr);
        info.DepthImageView = VK_NULL_HANDLE;
    }
    if (info.DepthImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(info.Device, info.DepthImage, nullptr);
        info.DepthImage = VK_NULL_HANDLE;
    }
    if (info.DepthImageMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(info.Device, info.DepthImageMemory, nullptr);
        info.DepthImageMemory = VK_NULL_HANDLE;
    }
    // Also destroy any MSAA color image resources if present
    if (info.ColorImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(info.Device, info.ColorImageView, nullptr);
        info.ColorImageView = VK_NULL_HANDLE;
    }
    if (info.ColorImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(info.Device, info.ColorImage, nullptr);
        info.ColorImage = VK_NULL_HANDLE;
    }
    if (info.ColorImageMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(info.Device, info.ColorImageMemory, nullptr);
        info.ColorImageMemory = VK_NULL_HANDLE;
    }

    VkFormat depthFormat = FindDepthFormat(device);
    info.DepthFormat = depthFormat;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = info.SwapChainExtent.width;
    imageInfo.extent.height = info.SwapChainExtent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(info.Device, &imageInfo, nullptr, &info.DepthImage) != VK_SUCCESS)
        return false;

    VkMemoryRequirements memReq{};
    vkGetImageMemoryRequirements(info.Device, info.DepthImage, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = device->FindMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(info.Device, &allocInfo, nullptr, &info.DepthImageMemory) != VK_SUCCESS)
        return false;

    vkBindImageMemory(info.Device, info.DepthImage, info.DepthImageMemory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = info.DepthImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = depthFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(info.Device, &viewInfo, nullptr, &info.DepthImageView) != VK_SUCCESS)
        return false;

    return true;
}

void ImageManager::Shutdown(VulkanContextInfo &info)
{
    if (info.DepthImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(info.Device, info.DepthImageView, nullptr);
        info.DepthImageView = VK_NULL_HANDLE;
    }
    if (info.DepthImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(info.Device, info.DepthImage, nullptr);
        info.DepthImage = VK_NULL_HANDLE;
    }
    if (info.DepthImageMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(info.Device, info.DepthImageMemory, nullptr);
        info.DepthImageMemory = VK_NULL_HANDLE;
    }

    // Destroy MSAA color resources if present
    if (info.ColorImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(info.Device, info.ColorImageView, nullptr);
        info.ColorImageView = VK_NULL_HANDLE;
    }
    if (info.ColorImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(info.Device, info.ColorImage, nullptr);
        info.ColorImage = VK_NULL_HANDLE;
    }
    if (info.ColorImageMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(info.Device, info.ColorImageMemory, nullptr);
        info.ColorImageMemory = VK_NULL_HANDLE;
    }
}
