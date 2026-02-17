#include "Render/Vulkan/VulkanDevice.h"
#include <set>
#include <cstring>
#include <algorithm>
#include <limits>

// ============================================================================
// VulkanDevice Implementation - ТОЛЬКО методы класса
// ============================================================================

VulkanDevice::~VulkanDevice ()
	{
		// Shutdown should be called explicitly with info
		// Destructor just ensures no dangling handles
	Device = VK_NULL_HANDLE;
	PhysicalDevice = VK_NULL_HANDLE;
	}

bool VulkanDevice::Initialize ( VulkanContextInfo & info )
	{
	LOG_DEBUG ( "Initializing Vulkan device..." );

	if (!PickPhysicalDevice ( info ))
		{
		LOG_FATAL ( "Failed to pick physical device" );
		return false;
		}

	if (!CreateLogicalDevice ( info ))
		{
		LOG_FATAL ( "Failed to create logical device" );
		return false;
		}

	CreateCommandPools ( info );

	// Update info with device handles
	info.Device = Device;
	info.PhysicalDevice = PhysicalDevice;
	info.GraphicsQueue = GraphicsQueue;
	info.PresentQueue = PresentQueue;
	info.ComputeQueue = ComputeQueue;
	info.TransferQueue = TransferQueue;
	info.QueueIndices = QueueIndices;

	LOG_DEBUG ( "Vulkan device initialized successfully" );
	return true;
	}

void VulkanDevice::Shutdown ( VulkanContextInfo & info )
	{
	LOG_DEBUG ( "Shutting down Vulkan device..." );
	LOG_DEBUG ( "  Device handle: ", ( void * ) Device );
	LOG_DEBUG ( "  GraphicsCommandPool: ", ( void * ) GraphicsCommandPool );  // члены класса
	LOG_DEBUG ( "  ComputeCommandPool: ", ( void * ) ComputeCommandPool );
	LOG_DEBUG ( "  TransferCommandPool: ", ( void * ) TransferCommandPool );

	// 1. Ждём завершения всех операций на устройстве
	if (Device != VK_NULL_HANDLE)
		{
		vkDeviceWaitIdle ( Device );
		}

		// 2. Уничтожаем command pools (только через члены класса!)
	if (GraphicsCommandPool != VK_NULL_HANDLE && Device != VK_NULL_HANDLE)
		{
		vkDestroyCommandPool ( Device, GraphicsCommandPool, nullptr );
		GraphicsCommandPool = VK_NULL_HANDLE;
		LOG_DEBUG ( "Graphics command pool destroyed" );
		}

	if (ComputeCommandPool != VK_NULL_HANDLE && Device != VK_NULL_HANDLE)
		{
			// Проверяем, не ссылается ли на тот же pool
		if (ComputeCommandPool != GraphicsCommandPool)
			{
			vkDestroyCommandPool ( Device, ComputeCommandPool, nullptr );
			LOG_DEBUG ( "Compute command pool destroyed (separate)" );
			}
		else
			{
			LOG_DEBUG ( "Compute command pool same as Graphics, skipping" );
			}
		ComputeCommandPool = VK_NULL_HANDLE;
		}

	if (TransferCommandPool != VK_NULL_HANDLE && Device != VK_NULL_HANDLE)
		{
		if (TransferCommandPool != GraphicsCommandPool)
			{
			vkDestroyCommandPool ( Device, TransferCommandPool, nullptr );
			LOG_DEBUG ( "Transfer command pool destroyed" );
			}
		else
			{
			LOG_DEBUG ( "Transfer command pool same as Graphics, skipping" );
			}
		TransferCommandPool = VK_NULL_HANDLE;
		}

		// 3. Теперь можно уничтожить устройство
	if (Device != VK_NULL_HANDLE)
		{
		vkDestroyDevice ( Device, nullptr );
		Device = VK_NULL_HANDLE;
		LOG_DEBUG ( "Vulkan device destroyed" );
		}

		// 4. Очищаем handles в info (только после уничтожения!)
	info.GraphicsCommandPool = VK_NULL_HANDLE;
	info.ComputeCommandPool = VK_NULL_HANDLE;
	info.TransferCommandPool = VK_NULL_HANDLE;
	info.Device = VK_NULL_HANDLE;
	info.PhysicalDevice = VK_NULL_HANDLE;
	info.GraphicsQueue = VK_NULL_HANDLE;
	info.PresentQueue = VK_NULL_HANDLE;
	info.ComputeQueue = VK_NULL_HANDLE;
	info.TransferQueue = VK_NULL_HANDLE;
	}



bool VulkanDevice::PickPhysicalDevice ( VulkanContextInfo & info )
	{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices ( info.Instance, &deviceCount, nullptr );

	if (deviceCount == 0)
		{
		LOG_FATAL ( "No Vulkan compatible GPUs found" );
		return false;
		}

	std::vector<VkPhysicalDevice> devices ( deviceCount );
	vkEnumeratePhysicalDevices ( info.Instance, &deviceCount, devices.data () );

	LOG_DEBUG ( "Found ", deviceCount, " physical device(s):" );

	for (const auto & device : devices)
		{
		LogDeviceProperties ( device );
		LogQueueFamilies ( device );

		if (IsDeviceSuitable ( device, info.Surface ))
			{
			PhysicalDevice = device;
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties ( device, &deviceProperties );
			LOG_DEBUG ( "Selected physical device: ", deviceProperties.deviceName );

			// Query swap chain support and store in info
			info.SwapChainSupport = QuerySwapChainSupport ( device, info.Surface );
			info.SwapChainSupport.Log ();

			// Get max usable sample count for MSAA
			info.MsaaSamples = GetMaxUsableSampleCount ();

			return true;
			}
		}

	LOG_FATAL ( "No suitable physical device found" );
	return false;
	}

bool VulkanDevice::IsDeviceSuitable ( VkPhysicalDevice device, VkSurfaceKHR surface ) const
	{
	QueueFamilyIndices indices = FindQueueFamilies ( device, surface );

	bool extensionsSupported = CheckDeviceExtensionSupport ( device );

	bool swapChainAdequate = false;
	if (extensionsSupported)
		{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport ( device, surface );
		swapChainAdequate = !swapChainSupport.Formats.empty () && !swapChainSupport.PresentModes.empty ();
		}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures ( device, &supportedFeatures );

	return indices.IsComplete () &&
		extensionsSupported &&
		swapChainAdequate &&
		supportedFeatures.samplerAnisotropy;
	}

QueueFamilyIndices VulkanDevice::FindQueueFamilies ( VkPhysicalDevice device, VkSurfaceKHR surface ) const
	{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties ( device, &queueFamilyCount, nullptr );

	std::vector<VkQueueFamilyProperties> queueFamilies ( queueFamilyCount );
	vkGetPhysicalDeviceQueueFamilyProperties ( device, &queueFamilyCount, queueFamilies.data () );

	int i = 0;
	for (const auto & queueFamily : queueFamilies)
		{
			// Graphics
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
			indices.GraphicsFamily = i;
			}

			// Compute (separate queue if available)
		if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
			if (!( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ) || !indices.ComputeFamily.has_value ())
				{
				indices.ComputeFamily = i;
				}
			}

			// Transfer (separate queue if available)
		if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
			if (!( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ) &&
				 !( queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT ))
				{
				indices.TransferFamily = i;
				}
			}

			// Present
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR ( device, i, surface, &presentSupport );
		if (presentSupport)
			{
			indices.PresentFamily = i;
			}

		if (indices.IsComplete () && indices.HasCompute () && indices.HasTransfer ())
			break;

		i++;
		}

		// Fallback: if no dedicated compute queue, use graphics queue
	if (!indices.ComputeFamily.has_value () && indices.GraphicsFamily.has_value ())
		{
		indices.ComputeFamily = indices.GraphicsFamily;
		}

		// Fallback: if no dedicated transfer queue, use graphics queue
	if (!indices.TransferFamily.has_value () && indices.GraphicsFamily.has_value ())
		{
		indices.TransferFamily = indices.GraphicsFamily;
		}

	return indices;
	}

bool VulkanDevice::CheckDeviceExtensionSupport ( VkPhysicalDevice device ) const
	{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties ( device, nullptr, &extensionCount, nullptr );

	std::vector<VkExtensionProperties> availableExtensions ( extensionCount );
	vkEnumerateDeviceExtensionProperties ( device, nullptr, &extensionCount, availableExtensions.data () );

	std::set<std::string> requiredExtensions ( DeviceExtensions.begin (), DeviceExtensions.end () );

   // LOG_DEBUG ( "Required device extensions:" );
	for (const auto & ext : DeviceExtensions)
		{
	  //  LOG_DEBUG ( "  ", ext );
		}

   // LOG_DEBUG ( "Available device extensions:" );
	for (const auto & extension : availableExtensions)
		{
	   // LOG_DEBUG ( "  ", extension.extensionName );
		requiredExtensions.erase ( extension.extensionName );
		}

	return requiredExtensions.empty ();
	}

SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport ( VkPhysicalDevice device, VkSurfaceKHR surface ) const
	{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR ( device, surface, &details.Capabilities );

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR ( device, surface, &formatCount, nullptr );

	if (formatCount != 0)
		{
		details.Formats.resize ( formatCount );
		vkGetPhysicalDeviceSurfaceFormatsKHR ( device, surface, &formatCount, details.Formats.data () );
		}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR ( device, surface, &presentModeCount, nullptr );

	if (presentModeCount != 0)
		{
		details.PresentModes.resize ( presentModeCount );
		vkGetPhysicalDeviceSurfacePresentModesKHR ( device, surface, &presentModeCount, details.PresentModes.data () );
		}

	return details;
	}

bool VulkanDevice::CreateLogicalDevice ( VulkanContextInfo & info )
	{
	QueueIndices = FindQueueFamilies ( PhysicalDevice, info.Surface );

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		QueueIndices.GraphicsFamily.value (),
		QueueIndices.PresentFamily.value (),
		QueueIndices.ComputeFamily.value (),
		QueueIndices.TransferFamily.value ()
		};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
		{
		VkDeviceQueueCreateInfo queueCreateInfo {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back ( queueCreateInfo );
		}

	VkPhysicalDeviceFeatures deviceFeatures {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;  // For wireframe rendering
	deviceFeatures.wideLines = VK_TRUE;
	deviceFeatures.largePoints = VK_TRUE;

	VkDeviceCreateInfo createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast< uint32_t >( queueCreateInfos.size () );
	createInfo.pQueueCreateInfos = queueCreateInfos.data ();
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast< uint32_t >( DeviceExtensions.size () );
	createInfo.ppEnabledExtensionNames = DeviceExtensions.data ();

	// Device layers are deprecated but kept for compatibility
	createInfo.enabledLayerCount = 0;

	if (vkCreateDevice ( PhysicalDevice, &createInfo, nullptr, &Device ) != VK_SUCCESS)
		{
		LOG_FATAL ( "Failed to create logical device" );
		return false;
		}

		// Get queues - правильно: 3 аргумента (device, queueFamilyIndex, queueIndex, pQueue)
	vkGetDeviceQueue ( Device, QueueIndices.GraphicsFamily.value (), 0, &GraphicsQueue );
	vkGetDeviceQueue ( Device, QueueIndices.PresentFamily.value (), 0, &PresentQueue );
	vkGetDeviceQueue ( Device, QueueIndices.ComputeFamily.value (), 0, &ComputeQueue );
	vkGetDeviceQueue ( Device, QueueIndices.TransferFamily.value (), 0, &TransferQueue );

	LOG_DEBUG ( "Logical device created successfully" );
	QueueIndices.Log ();

	return true;
	}

void VulkanDevice::CreateCommandPools ( VulkanContextInfo & info )
	{
		// Graphics command pool
	VkCommandPoolCreateInfo poolInfo {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = QueueIndices.GraphicsFamily.value ();

	if (vkCreateCommandPool ( Device, &poolInfo, nullptr, &info.GraphicsCommandPool ) != VK_SUCCESS)
		{
		LOG_ERROR ( "Failed to create graphics command pool" );
		}
	else
		{
		LOG_DEBUG ( "Graphics command pool created" );
		}
	GraphicsCommandPool = info.GraphicsCommandPool;

		// Compute command pool (if different from graphics)
	if (QueueIndices.ComputeFamily.value () != QueueIndices.GraphicsFamily.value ())
		{
		poolInfo.queueFamilyIndex = QueueIndices.ComputeFamily.value ();
		if (vkCreateCommandPool ( Device, &poolInfo, nullptr, &info.ComputeCommandPool ) != VK_SUCCESS)
			{
			LOG_ERROR ( "Failed to create compute command pool" );
			}
		}
	else
		{
		info.ComputeCommandPool = info.GraphicsCommandPool;
		}
	ComputeCommandPool = info.ComputeCommandPool;
		// Transfer command pool (if different from graphics)
	if (QueueIndices.TransferFamily.value () != QueueIndices.GraphicsFamily.value ())
		{
		poolInfo.queueFamilyIndex = QueueIndices.TransferFamily.value ();
		if (vkCreateCommandPool ( Device, &poolInfo, nullptr, &info.TransferCommandPool ) != VK_SUCCESS)
			{
			LOG_ERROR ( "Failed to create transfer command pool" );
			}
		}
	else
		{
		info.TransferCommandPool = info.GraphicsCommandPool;
		}
	TransferCommandPool = info.TransferCommandPool;
	}

uint32_t VulkanDevice::FindMemoryType ( uint32_t typeFilter, VkMemoryPropertyFlags properties ) const
	{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties ( PhysicalDevice, &memProperties );

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
		if (( typeFilter & ( 1 << i ) ) && ( memProperties.memoryTypes[ i ].propertyFlags & properties ) == properties)
			{
			return i;
			}
		}

	LOG_FATAL ( "Failed to find suitable memory type" );
	return 0;
	}

SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport ( VkSurfaceKHR surface ) const
	{
	return QuerySwapChainSupport ( PhysicalDevice, surface );
	}

VkFormat VulkanDevice::FindSupportedFormat ( const std::vector<VkFormat> & candidates,
											 VkImageTiling tiling,
											 VkFormatFeatureFlags features ) const
	{
	for (VkFormat format : candidates)
		{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties ( PhysicalDevice, format, &props );

		if (tiling == VK_IMAGE_TILING_LINEAR && ( props.linearTilingFeatures & features ) == features)
			{
			return format;
			}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && ( props.optimalTilingFeatures & features ) == features)
			{
			return format;
			}
		}

	LOG_FATAL ( "Failed to find supported format" );
	return VK_FORMAT_UNDEFINED;
	}

VkSampleCountFlagBits VulkanDevice::GetMaxUsableSampleCount () const
	{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties ( PhysicalDevice, &physicalDeviceProperties );

	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
		physicalDeviceProperties.limits.framebufferDepthSampleCounts;

	if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
	if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
	if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
	if (counts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
	if (counts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
	if (counts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;

	return VK_SAMPLE_COUNT_1_BIT;
	}

void VulkanDevice::LogDeviceProperties ( VkPhysicalDevice device ) const
	{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	VkPhysicalDeviceMemoryProperties memoryProperties;

	vkGetPhysicalDeviceProperties ( device, &deviceProperties );
	vkGetPhysicalDeviceFeatures ( device, &deviceFeatures );
	vkGetPhysicalDeviceMemoryProperties ( device, &memoryProperties );

	LOG_DEBUG ( "  Device: ", deviceProperties.deviceName );
	LOG_DEBUG ( "    Type: ", static_cast< int >( deviceProperties.deviceType ) );
	LOG_DEBUG ( "    API Version: ", VK_VERSION_MAJOR ( deviceProperties.apiVersion ), ".",
				VK_VERSION_MINOR ( deviceProperties.apiVersion ), ".",
				VK_VERSION_PATCH ( deviceProperties.apiVersion ) );
	LOG_DEBUG ( "    Driver Version: ", deviceProperties.driverVersion );
	LOG_DEBUG ( "    Vendor ID: ", deviceProperties.vendorID );
	LOG_DEBUG ( "    Device ID: ", deviceProperties.deviceID );
	LOG_DEBUG ( "    Memory Heaps: ", memoryProperties.memoryHeapCount );
	LOG_DEBUG ( "    Memory Types: ", memoryProperties.memoryTypeCount );
	}

void VulkanDevice::LogQueueFamilies ( VkPhysicalDevice device ) const
	{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties ( device, &queueFamilyCount, nullptr );

	std::vector<VkQueueFamilyProperties> queueFamilies ( queueFamilyCount );
	vkGetPhysicalDeviceQueueFamilyProperties ( device, &queueFamilyCount, queueFamilies.data () );

	LOG_DEBUG ( "    Queue Families: ", queueFamilyCount );
	for (size_t i = 0; i < queueFamilies.size (); i++)
		{
		std::string flags;
		if (queueFamilies[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT) flags += "GRAPHICS ";
		if (queueFamilies[ i ].queueFlags & VK_QUEUE_COMPUTE_BIT) flags += "COMPUTE ";
		if (queueFamilies[ i ].queueFlags & VK_QUEUE_TRANSFER_BIT) flags += "TRANSFER ";
		if (queueFamilies[ i ].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) flags += "SPARSE ";
		if (queueFamilies[ i ].queueFlags & VK_QUEUE_PROTECTED_BIT) flags += "PROTECTED ";

		LOG_DEBUG ( "      [", i, "] ", queueFamilies[ i ].queueCount, " queues - ", flags );
		}
	}