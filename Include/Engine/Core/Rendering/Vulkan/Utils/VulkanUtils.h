#pragma once
#include <vector>

#include "CoreMinimal.h"
#include "vulkan/vulkan.h"


// Макрос для проверки результатов Vulkan
#define VK_CHECK(result, message)                                \
  do                                                             \
  {                                                              \
    VkResult vkResult = (result);                                \
    if (vkResult != VK_SUCCESS)                                  \
    {                                                            \
      RENDER_ERROR(message, " (Error code: ", vkResult, ")"); \
      throw std::runtime_error(message);                         \
    }                                                            \
  } while (0)

  // Вспомогательные функции для Vulkan
  class VulkanUtils
  {
   public:
    static bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);
    static std::vector<const char*> GetRequiredExtensions(bool enableValidationLayers);
    static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);
    static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    static void CreateBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size,
                             VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                             VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    static void CopyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue,
                           VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  };
