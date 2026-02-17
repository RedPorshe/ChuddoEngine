#pragma once
#include "CoreMinimal.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <set>
#include "Render/Vulkan/VulkanContext.h"  // ВАЖНО: включаем контекст для структур

class VulkanDevice
    {
    public:
        VulkanDevice () = default;
        ~VulkanDevice ();

        bool Initialize ( VulkanContextInfo & info );
        void Shutdown ( VulkanContextInfo & info );

        // Getters
        VkDevice GetDevice () const { return Device; }
        VkPhysicalDevice GetPhysicalDevice () const { return PhysicalDevice; }

        VkQueue GetGraphicsQueue () const { return GraphicsQueue; }
        VkQueue GetPresentQueue () const { return PresentQueue; }
        VkQueue GetComputeQueue () const { return ComputeQueue; }
        VkQueue GetTransferQueue () const { return TransferQueue; }

        const QueueFamilyIndices & GetQueueIndices () const { return QueueIndices; }

        // Helper functions
        uint32_t FindMemoryType ( uint32_t typeFilter, VkMemoryPropertyFlags properties ) const;
        SwapChainSupportDetails QuerySwapChainSupport ( VkSurfaceKHR surface ) const;
        VkFormat FindSupportedFormat ( const std::vector<VkFormat> & candidates,
                                       VkImageTiling tiling,
                                       VkFormatFeatureFlags features ) const;

        VkSampleCountFlagBits GetMaxUsableSampleCount () const;

    private:
        bool PickPhysicalDevice ( VulkanContextInfo & info );
        bool CreateLogicalDevice ( VulkanContextInfo & info );
        void CreateCommandPools ( VulkanContextInfo & info );

        bool IsDeviceSuitable ( VkPhysicalDevice device, VkSurfaceKHR surface ) const;
        QueueFamilyIndices FindQueueFamilies ( VkPhysicalDevice device, VkSurfaceKHR surface ) const;
        bool CheckDeviceExtensionSupport ( VkPhysicalDevice device ) const;
        SwapChainSupportDetails QuerySwapChainSupport ( VkPhysicalDevice device, VkSurfaceKHR surface ) const;

        void LogDeviceProperties ( VkPhysicalDevice device ) const;
        void LogQueueFamilies ( VkPhysicalDevice device ) const;

        VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        VkDevice Device = VK_NULL_HANDLE;

        VkQueue GraphicsQueue = VK_NULL_HANDLE;
        VkQueue PresentQueue = VK_NULL_HANDLE;
        VkQueue ComputeQueue = VK_NULL_HANDLE;
        VkQueue TransferQueue = VK_NULL_HANDLE;

        VkCommandPool GraphicsCommandPool = VK_NULL_HANDLE;
        VkCommandPool ComputeCommandPool = VK_NULL_HANDLE;
        VkCommandPool TransferCommandPool = VK_NULL_HANDLE;


        QueueFamilyIndices QueueIndices;

        const std::vector<const char *> DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };
    };