#include "Rendering/Vulkan/DeviceManager.h"

#include "Rendering/Vulkan/VulkanUtils.h"

namespace CE
{
  const char* VkPhysicalDeviceTypeString(VkPhysicalDeviceType type)
  {
    switch (type)
    {
      case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        return "Other";
      case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return "Integrated GPU";
      case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return "Discrete GPU";
      case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return "Virtual GPU";
      case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return "CPU";
      default:
        return "Unknown";
    }
  }

  bool DeviceManager::Initialize(VkInstance instance, VkSurfaceKHR surface)
  {
    if (!PickPhysicalDevice(instance, surface))
    {
      CE_RENDER_ERROR("Failed to pick physical device");
      return false;
    }

    if (!CreateLogicalDevice(surface))
    {
      CE_RENDER_ERROR("Failed to create logical device");
      return false;
    }

    CE_RENDER_DEBUG("DeviceManager initialized successfully");
    return true;
  }

  void DeviceManager::Shutdown()
  {
    if (m_device != VK_NULL_HANDLE)
    {
      vkDestroyDevice(m_device, nullptr);
      m_device = VK_NULL_HANDLE;
      CE_RENDER_DEBUG("Logical device destroyed");
    }

    // Physical device doesn't need destruction
    m_physicalDevice = VK_NULL_HANDLE;
  }

  QueueFamilyIndices DeviceManager::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
  {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // Find graphics queue family
    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
      // Check for graphics support
      if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
      {
        indices.graphicsFamily = i;
      }

      // Check for presentation support
      VkBool32 presentSupport = false;
      if (surface != VK_NULL_HANDLE)
      {
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
        {
          indices.presentFamily = i;
        }
      }
      else
      {
        // Если surface не предоставлен, используем ту же очередь что и для графики
        indices.presentFamily = indices.graphicsFamily;
      }

      if (indices.graphicsFamily != UINT32_MAX &&
          (surface == VK_NULL_HANDLE || indices.presentFamily != UINT32_MAX))
      {
        m_queueIndices.graphicsFamily = indices.graphicsFamily;
        m_queueIndices.presentFamily = indices.presentFamily;
        break;
      }
    }

    return indices;
  }

  bool DeviceManager::PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
  {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
      CE_RENDER_ERROR("Failed to find GPUs with Vulkan support!");
      return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    CE_RENDER_DEBUG("Found ", deviceCount, " physical device(s)");

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices)
    {
      int score = RateDeviceSuitability(device, surface);
      candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0)
    {
      m_physicalDevice = candidates.rbegin()->second;

      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties(m_physicalDevice, &deviceProperties);
      CE_RENDER_DEBUG("Selected physical device: ", deviceProperties.deviceName,
                      " (Score: ", candidates.rbegin()->first, ")");
      return true;
    }

    CE_RENDER_ERROR("Failed to find a suitable GPU!");
    return false;
  }

  bool DeviceManager::CreateLogicalDevice(VkSurfaceKHR surface)
  {
    QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice, surface);

    if (!indices.IsComplete())
    {
      CE_RENDER_ERROR("Failed to find required queue families!");
      return false;
    }

    // Create one queue for each family
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }

    // Specify used device features
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;  // Request anisotropy if available

    // Create the logical device
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    // Enable device extensions
    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

// Validation layers (deprecated for modern Vulkan, but kept for compatibility)
#ifdef _DEBUG
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    VkResult result = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
    VK_CHECK(result, "Failed to create logical device!");

    // Retrieve queue handles
    vkGetDeviceQueue(m_device, indices.graphicsFamily, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.presentFamily, 0, &m_presentQueue);

    CE_RENDER_DEBUG("Logical device created successfully");
    CE_RENDER_DEBUG("Graphics queue family: ", indices.graphicsFamily);
    CE_RENDER_DEBUG("Present queue family: ", indices.presentFamily);

    return true;
  }

  bool DeviceManager::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
  {
    QueueFamilyIndices indices = FindQueueFamilies(device, surface);

    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    // For now, we also require swap chain support
    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
      // We'll check swap chain details later, for now just assume it's adequate
      swapChainAdequate = true;
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
  }

  int DeviceManager::RateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR surface)
  {
    if (!IsDeviceSuitable(device, surface))
    {
      return 0;
    }

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Prefer devices with higher VRAM
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++)
    {
      if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
      {
        score += static_cast<int>(memoryProperties.memoryHeaps[i].size / (1024 * 1024));  // MB
      }
    }

    CE_RENDER_DEBUG("Device ", deviceProperties.deviceName, " scored: ", score);
    return score;
  }

  bool DeviceManager::CheckDeviceExtensionSupport(VkPhysicalDevice device)
  {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
      requiredExtensions.erase(extension.extensionName);
    }

    if (!requiredExtensions.empty())
    {
      CE_RENDER_DEBUG("Missing device extensions:");
      for (const auto& missing : requiredExtensions)
      {
        CE_RENDER_DEBUG("  - ", missing);
      }
    }

    return requiredExtensions.empty();
  }
}  // namespace CE