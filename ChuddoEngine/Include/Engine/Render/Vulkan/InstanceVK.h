#pragma once
#include "CoreMinimal.h"
#include <vulkan/vulkan.h>
#include <vector>
struct VulkanContextInfo;
class InstanceVK
    {
    public:
        InstanceVK ();
        ~InstanceVK ();

        bool Initialize (  VulkanContextInfo & info );
        void Shutdown ();

        VkInstance GetInstance () const { return Instance; }

    private:
        bool CheckValidationLayerSupport ();
        std::vector<const char *> GetRequiredExtensions () const;

        // Дебаг колбэк
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback (
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
            void * pUserData );

        VkInstance Instance = VK_NULL_HANDLE;

#ifdef _DEBUG
    // Validation layers
        std::vector<const char *> ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
            };

            // Debug messenger
        VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;

        // Функции для работы с debug extension
        PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessenger = nullptr;
        PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessenger = nullptr;
#endif
    };