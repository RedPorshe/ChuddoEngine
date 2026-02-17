#include "Render/Vulkan/InstanceVK.h"
#include "Render/Vulkan/VulkanContext.h"
#include <GLFW/glfw3.h>
#include <cstring>

InstanceVK::InstanceVK ()
    {}

InstanceVK::~InstanceVK ()
    {
        // Деструктор вызывает Shutdown для гарантии очистки
    Shutdown ();
    }

bool InstanceVK::CheckValidationLayerSupport ()
    {
#ifdef _DEBUG
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties ( &layerCount, nullptr );

    std::vector<VkLayerProperties> availableLayers ( layerCount );
    vkEnumerateInstanceLayerProperties ( &layerCount, availableLayers.data () );

    for (const char * layerName : ValidationLayers)
        {
        bool layerFound = false;
        for (const auto & layerProperties : availableLayers)
            {
            if (strcmp ( layerName, layerProperties.layerName ) == 0)
                {
                layerFound = true;
                break;
                }
            }
        if (!layerFound)
            {
            LOG_ERROR ( "Validation layer not found: ", layerName );
            return false;
            }
        }
    LOG_DEBUG ( "All validation layers are supported" );
#endif
    return true;
    }

std::vector<const char *> InstanceVK::GetRequiredExtensions () const
    {
    uint32_t glfwExtCount = 0;
    const char ** glfwExt = glfwGetRequiredInstanceExtensions ( &glfwExtCount );

    std::vector<const char *> extensions ( glfwExt, glfwExt + glfwExtCount );

#ifdef _DEBUG
    extensions.push_back ( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );   
#endif

    return extensions;
    }

VKAPI_ATTR VkBool32 VKAPI_CALL InstanceVK::DebugCallback (
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
    void * pUserData )
    {
        // Игнорируем вербозные сообщения (слишком много)
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        return VK_FALSE;

    // Конвертируем severity в уровень лога
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
        LOG_ERROR ( "[VULKAN VALIDATION] ", pCallbackData->pMessage );
        }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
        LOG_WARN ( "[VULKAN VALIDATION] ", pCallbackData->pMessage );
        }
    else
        {
        LOG_DEBUG ( "[VULKAN VALIDATION] ", pCallbackData->pMessage );
        }

    return VK_FALSE;
    }

bool InstanceVK::Initialize (  VulkanContextInfo & info )
    {
    LOG_DEBUG ( "Initializing Vulkan instance..." );

#ifdef _DEBUG
    // Проверяем поддержку validation layers в debug режиме
    if (!CheckValidationLayerSupport ())
        {
        LOG_WARN ( "Validation layers requested but not available - continuing without them" );
        }
#endif

    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = info.AppName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION (info.AppVersionMajor, info.AppVersionMinor, info.AppVersionPatch );
    appInfo.pEngineName = info.EngineName.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION ( info.EngineVersionMajor, info.EngineVersionMinor, info.EngineVersionPatch );
    appInfo.apiVersion = VK_API_VERSION_1_0;

    auto extensions = GetRequiredExtensions ();

    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast< uint32_t >( extensions.size () );
    createInfo.ppEnabledExtensionNames = extensions.data ();

#ifdef _DEBUG
    // Временно включаем слои валидации для создания instance
    createInfo.enabledLayerCount = static_cast< uint32_t >( ValidationLayers.size () );
    createInfo.ppEnabledLayerNames = ValidationLayers.data ();
    LOG_DEBUG ( "Enabling ", ValidationLayers.size (), " validation layers for instance creation" );
#else
    createInfo.enabledLayerCount = 0;
#endif

    VkResult result = vkCreateInstance ( &createInfo, nullptr, &Instance );

    if (result != VK_SUCCESS)
        {
        LOG_FATAL ( "Failed to create Vulkan instance, error: ", static_cast< int >( result ) );
        return false;
        }
    info.Instance = Instance;
    LOG_DEBUG ( "Vulkan instance created successfully: ", ( void * ) Instance );

#ifdef _DEBUG
    // Загружаем функции для debug messenger
    CreateDebugUtilsMessenger = ( PFN_vkCreateDebugUtilsMessengerEXT )
        vkGetInstanceProcAddr ( Instance, "vkCreateDebugUtilsMessengerEXT" );
    DestroyDebugUtilsMessenger = ( PFN_vkDestroyDebugUtilsMessengerEXT )
        vkGetInstanceProcAddr ( Instance, "vkDestroyDebugUtilsMessengerEXT" );

    if (!CreateDebugUtilsMessenger || !DestroyDebugUtilsMessenger)
        {
        LOG_ERROR ( "Failed to load debug messenger functions - continuing without debug messenger" );
        }
    else
        {
            // Создаём debug messenger
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        debugCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        debugCreateInfo.pfnUserCallback = DebugCallback;
        debugCreateInfo.pUserData = nullptr; // Можно передать this если нужно

        result = CreateDebugUtilsMessenger ( Instance, &debugCreateInfo, nullptr, &DebugMessenger );

        if (result != VK_SUCCESS)
            {
            LOG_ERROR ( "Failed to create debug messenger, error: ", static_cast< int >( result ) );
            }
        else
            {
            LOG_DEBUG ( "Debug messenger created successfully: ", ( void * ) DebugMessenger );
            }
        }
#endif

    return true;
    }

void InstanceVK::Shutdown ()
    {
    if (Instance == VK_NULL_HANDLE)
        {
        LOG_DEBUG ( "Vulkan instance already destroyed, skipping" );
        return;
        }

    LOG_DEBUG ( "Shutting down Vulkan instance..." );

#ifdef _DEBUG
    if (DebugMessenger != VK_NULL_HANDLE && DestroyDebugUtilsMessenger != nullptr)
        {
        DestroyDebugUtilsMessenger ( Instance, DebugMessenger, nullptr );
        DebugMessenger = VK_NULL_HANDLE;
        LOG_DEBUG ( "Debug messenger destroyed" );
        }
#endif

    vkDestroyInstance ( Instance, nullptr );
    LOG_DEBUG ( "Vulkan instance destroyed successfully" );
    Instance = VK_NULL_HANDLE;
    }