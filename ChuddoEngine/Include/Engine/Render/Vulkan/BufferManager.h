#pragma once
#include "Render/Vulkan/VulkanContext.h"

class BufferManager
{
public:
    BufferManager() = default;
    ~BufferManager() = default;

    bool Initialize(VulkanContextInfo &info);
    void Shutdown(VulkanContextInfo &info);
};
