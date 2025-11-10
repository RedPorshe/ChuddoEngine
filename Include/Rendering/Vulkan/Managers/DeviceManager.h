#pragma once
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "CoreMinimal.h"
#include "vulkan/vulkan.h"

namespace CE
{
  // Queue family indices
  struct QueueFamilyIndices
  {
    uint32_t graphicsFamily = UINT32_MAX;
    uint32_t presentFamily = UINT32_MAX;

    bool IsComplete() const
    {
      return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX;
    }
  };

  class DeviceManager
  {
   public:
    DeviceManager() = default;
    ~DeviceManager() = default;

    // Удаляем копирование
    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;

    bool Initialize(VkInstance instance, VkSurfaceKHR surface);
    void Shutdown();

    // Getters
    VkPhysicalDevice GetPhysicalDevice() const
    {
      return m_physicalDevice;
    }
    VkDevice GetDevice() const
    {
      return m_device;
    }
    VkQueue GetGraphicsQueue() const
    {
      return m_graphicsQueue;
    }
    VkQueue GetPresentQueue() const
    {
      return m_presentQueue;
    }
    QueueFamilyIndices& getIndices()
    {
      return m_queueIndices;
    }
    QueueFamilyIndices
    FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

   private:
    bool PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
    bool CreateLogicalDevice(VkSurfaceKHR surface);
    bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    int RateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR surface);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

   private:
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    QueueFamilyIndices m_queueIndices;
    const std::vector<const char*> m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  };
}  // namespace CE