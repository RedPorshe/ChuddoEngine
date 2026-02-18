#pragma once
#include "Render/Vulkan/VulkanContext.h"

class SyncManager
{
public:
    SyncManager() = default;
    ~SyncManager() = default;

    // Initialize sync primitives based on context info. Return true on success.
    bool Initialize(VulkanContextInfo &info);
    void Shutdown(VulkanContextInfo &info);
    void ResizeImageTracking(VulkanContextInfo &info, size_t imageCount);
};
