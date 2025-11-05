#pragma once
#include <vulkan/vulkan.h>

#include <vector>

#include "Core/Render/Vulkan/VKContext.hpp"

namespace CE
{

  extern const std::vector<const char*> validationLayers;
  extern const std::vector<const char*> deviceExtensions;

  bool CheckValidationLayerSupport();

  std::vector<const char*> GetRequiredExtensions(VKContext& m_vkInfo);

  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VKContext& m_vkInfo);

  bool IsDeviceSuitable(VkPhysicalDevice device, VKContext& m_vkInfo);

  bool CheckDeviceExtensionSupport(VkPhysicalDevice device, VKContext& m_vkInfo);

  SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VKContext& m_vkInfo);

  uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

}  // namespace CE