#pragma once
#include "Render/Vulkan/VulkanContext.h"

class VulkanDevice;

class ImageManager
{
public:
    ImageManager() = default;
    ~ImageManager() = default;

    bool Initialize(VulkanContextInfo &info, VulkanDevice* device);
    void Shutdown(VulkanContextInfo &info);
    bool RecreateDepthImage(VulkanContextInfo &info, VulkanDevice* device);
};
