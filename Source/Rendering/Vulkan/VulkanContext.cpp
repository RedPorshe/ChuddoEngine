#include "Rendering/Vulkan/VulkanContext.h"

CE::VulkanContext::VulkanContext(AppInfo* info) : m_info{info}
{
}

CE::VulkanContext::VulkanContext(AppInfo* info) : m_info{info}
{
}

void CE::VulkanContext::Initialize()
{
  if (!InitWindow())
  {
    Shutdown();
    return;
  }

  if (!CreateInstance())
  {
    CE_RENDER_ERROR("Failed to create Instance");
    Shutdown();
    return;
  }

  if (bIsValidationEnabled && !SetupDebugMessenger())
  {
    CE_RENDER_ERROR("Failed to setup debug messenger");
    Shutdown();
    return;
  }
  if (!CreateSurface())
  {
    CE_RENDER_ERROR("Failed to Create Surface");
    Shutdown();
    return;
  }

  // Используем make_shared вместо make_unique
  m_deviceManager = std::make_shared<DeviceManager>();
  if (!m_deviceManager->Initialize(m_instance, m_surface))
  {
    CE_RENDER_ERROR("Failed to initialize DeviceManager");
    Shutdown();
    return;
  }

  // Create SwapchainManager
  m_swapchainManager = std::make_shared<SwapchainManager>(m_instance, m_surface, m_deviceManager);
  if (!m_swapchainManager->Initialize())
  {
    CE_RENDER_ERROR("Failed to initialize SwapchainManager");
    Shutdown();
    return;
  }

  // Create PipelineManager
  m_pipelineManager = std::make_shared<PipelineManager>(m_deviceManager, m_swapchainManager);
  if (!m_pipelineManager->Initialize())
  {
    CE_RENDER_ERROR("Failed to initialize PipelineManager");
    Shutdown();
    return;
  }

  CE_RENDER_DEBUG("VulkanContext initialized successfully");
}

void CE::VulkanContext::Shutdown()
{
  if (m_deviceManager && m_deviceManager->GetDevice() != VK_NULL_HANDLE)
  {
    vkDeviceWaitIdle(m_deviceManager->GetDevice());
  }

  if (m_pipelineManager)
  {
    m_pipelineManager->Cleanup();
    m_pipelineManager.reset();
  }

  if (m_swapchainManager)
  {
    m_swapchainManager->Cleanup();
    m_swapchainManager.reset();
  }

  if (m_deviceManager)
  {
    m_deviceManager->Shutdown();
    m_deviceManager.reset();
  }

  if (m_surface != VK_NULL_HANDLE)
  {
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    CE_RENDER_DEBUG("Surface destroyed");
    m_surface = VK_NULL_HANDLE;
  }
  // Cleanup debug messenger
  if (bIsValidationEnabled && m_debugMessenger != VK_NULL_HANDLE)
  {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
      func(m_instance, m_debugMessenger, nullptr);
    }
    CE_RENDER_DEBUG("Debug messenger destroyed");
  }

  // Cleanup instance
  if (m_instance != VK_NULL_HANDLE)
  {
    vkDestroyInstance(m_instance, nullptr);
    CE_RENDER_DEBUG("Vulkan instance destroyed");
    m_instance = VK_NULL_HANDLE;
  }

  // Cleanup window
  if (m_window)
  {
    glfwDestroyWindow(m_window);
    m_window = nullptr;
    CE_RENDER_DEBUG("Window destroyed");
  }

  glfwTerminate();
  CE_RENDER_DEBUG("GLFW terminated");
}

void CE::VulkanContext::DrawFrame(const FrameRenderData& renderData)
{
  CE_RENDER_DEBUG("render in stub mode; render data object count : ", renderData.renderObjects.size());
  CE_RENDER_DEBUG("Vulkan would render: ", renderData.renderObjects.size(),
                  " objects with camera at (",
                  renderData.camera.position.x, ", ",
                  renderData.camera.position.y, ", ",
                  renderData.camera.position.z, ")");
  glfwPollEvents();
}

bool CE::VulkanContext::ShouldClose() const
{
  return glfwWindowShouldClose(m_window);
}

bool CE::VulkanContext::InitWindow()
{
  if (!glfwInit())
    return false;
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  CE_CHECK((m_window = glfwCreateWindow(m_info->Width, m_info->Height, m_info->AppName.c_str(), nullptr, nullptr)) != nullptr);
  if (!m_window)
  {
    CE_RENDER_DEBUG("Failed to Create window");
    glfwTerminate();
    return false;
  }
  CE_RENDER_DEBUG("Window Created : ", m_window);
  return true;
}

bool CE::VulkanContext::CreateInstance()
{
  VkApplicationInfo AppInfo{};
  AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  AppInfo.pNext = nullptr;
  AppInfo.pApplicationName = m_info->AppName.c_str();
  AppInfo.applicationVersion = VK_MAKE_VERSION(m_info->AppVerion[0], m_info->AppVerion[1], m_info->AppVerion[2]);
  AppInfo.pEngineName = m_info->EngineName.c_str();
  AppInfo.engineVersion = VK_MAKE_VERSION(m_info->EngineVersion[0], m_info->EngineVersion[1], m_info->EngineVersion[2]);
  AppInfo.apiVersion = VK_API_VERSION_1_0;

  auto Extensions = VulkanUtils::GetRequiredExtensions(bIsValidationEnabled);

  // Validation layers
  std::vector<const char*> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  // Check validation layer support
  if (bIsValidationEnabled && !VulkanUtils::CheckValidationLayerSupport(validationLayers))
  {
    CE_RENDER_ERROR("Validation layers requested, but not available!");
    return false;
  }

  VkInstanceCreateInfo CreateInfo{};
  CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  CreateInfo.pNext = nullptr;
  CreateInfo.flags = 0;
  CreateInfo.pApplicationInfo = &AppInfo;
  CreateInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions.size());
  CreateInfo.ppEnabledExtensionNames = Extensions.data();

  // Set validation layers
  if (bIsValidationEnabled)
  {
    CreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    CreateInfo.ppEnabledLayerNames = validationLayers.data();
  }
  else
  {
    CreateInfo.enabledLayerCount = 0;
    CreateInfo.ppEnabledLayerNames = nullptr;
  }

  VkResult result = vkCreateInstance(&CreateInfo, nullptr, &m_instance);
  VK_CHECK(result, "Failed to create instance");
  CE_RENDER_DEBUG("Instance Created successfully: ", static_cast<void*>(m_instance));
  return true;
}

bool CE::VulkanContext::SetupDebugMessenger()
{
  if (!bIsValidationEnabled)
    return true;

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = DebugCallback;
  createInfo.pUserData = nullptr;

  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr)
  {
    VkResult result = func(m_instance, &createInfo, nullptr, &m_debugMessenger);
    VK_CHECK(result, "Failed to setup debug messenger");
    CE_RENDER_DEBUG("Debug messenger created successfully");
    return true;
  }
  else
  {
    CE_RENDER_ERROR("Failed to get vkCreateDebugUtilsMessengerEXT function pointer");
    return false;
  }
}

bool CE::VulkanContext::CreateSurface()
{
  VkResult result = glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface);
  VK_CHECK(result, "Failed to create window surface");
  return true;
}
VKAPI_ATTR VkBool32 VKAPI_CALL CE::VulkanContext::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
  (void)messageType;
  (void)pUserData;
  // Log Vulkan validation messages
  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
  {
    CE_RENDER_ERROR("Vulkan Validation Error: ", pCallbackData->pMessage);
  }
  else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
  {
    CE_RENDER_WARN("Vulkan Validation Warning: ", pCallbackData->pMessage);
  }
  else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
  {
    CE_RENDER_DEBUG("Vulkan Validation Info: ", pCallbackData->pMessage);
  }
  else
  {
    CE_RENDER_TRACE("Vulkan Validation Verbose: ", pCallbackData->pMessage);
  }

  return VK_FALSE;
}