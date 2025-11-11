#include "Core/Rendering/Vulkan/Core/VulkanContext.h"

#include <GLFW/glfw3.h>

CE::VulkanContext::VulkanContext(AppInfo* info) : m_info{info}
{
}

void CE::VulkanContext::Initialize()
{
  if (!InitWindow())
  {
    CE_RENDER_ERROR("Failed to create Window");
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

  m_deviceManager = std::make_shared<DeviceManager>();
  if (!m_deviceManager->Initialize(m_instance, m_surface))
  {
    CE_RENDER_ERROR("Failed to initialize DeviceManager");
    Shutdown();
    return;
  }

  m_swapchainManager = std::make_shared<SwapchainManager>(m_instance, m_surface, m_deviceManager, m_window);
  if (!m_swapchainManager->Initialize())
  {
    CE_RENDER_ERROR("Failed to initialize SwapchainManager");
    Shutdown();
    return;
  }

  m_pipelineManager = std::make_shared<PipelineManager>(m_deviceManager);
  if (!m_pipelineManager->Initialize())
  {
    CE_RENDER_ERROR("Failed to initialize PipelineManager");
    Shutdown();
    return;
  }

  m_bufferManager = std::make_shared<BufferManager>(m_deviceManager);
  if (!m_bufferManager->Initialize())
  {
    CE_RENDER_ERROR("Failed to initialize BufferManager");
    Shutdown();
    return;
  }

  // Создаем UBO буферы и проверяем их создание
  if (!m_bufferManager->CreateUniformBuffer(m_sceneUBOBufferName, sizeof(SceneUBO)))
  {
    CE_RENDER_ERROR("Failed to create scene UBO buffer");
    Shutdown();
    return;
  }

  if (!m_bufferManager->CreateUniformBuffer(m_lightingUBOBufferName, sizeof(LightingUBO)))
  {
    CE_RENDER_ERROR("Failed to create lighting UBO buffer");
    Shutdown();
    return;
  }

  // Проверяем что буферы созданы
  if (m_bufferManager->GetBuffer(m_sceneUBOBufferName) == VK_NULL_HANDLE ||
      m_bufferManager->GetBuffer(m_lightingUBOBufferName) == VK_NULL_HANDLE)
  {
    CE_RENDER_ERROR("UBO buffers are null after creation");
    Shutdown();
    return;
  }

  m_descriptorManager = std::make_shared<DescriptorManager>(m_deviceManager, m_bufferManager);
  if (!m_descriptorManager->Initialize())
  {
    CE_RENDER_ERROR("Failed to initialize DescriptorManager");
    Shutdown();
    return;
  }

  // Создаем CommandBufferManager
  m_commandBufferManager = std::make_shared<CommandBufferManager>(m_deviceManager);
  if (!m_commandBufferManager->Initialize())
  {
    CE_RENDER_ERROR("Failed to initialize CommandBufferManager");
    Shutdown();
    return;
  }

  // Create command buffers for swapchain images
  if (!m_commandBufferManager->CreateCommandBuffers(m_swapchainManager->GetImageCount()))
  {
    CE_RENDER_ERROR("Failed to create command buffers");
    Shutdown();
    return;
  }

  // Create default mesh pipeline
  if (!m_pipelineManager->CreateMeshPipeline("mesh", m_swapchainManager->GetRenderPass()))
  {
    CE_RENDER_ERROR("Failed to create mesh pipeline");
    Shutdown();
    return;
  }

  CreateSyncObjects();

  CE_RENDER_DEBUG("VulkanContext initialized successfully");
}

void CE::VulkanContext::Shutdown()
{
  if (m_deviceManager && m_deviceManager->GetDevice() != VK_NULL_HANDLE)
  {
    vkDeviceWaitIdle(m_deviceManager->GetDevice());
  }

  CleanupSyncObjects();

  for (auto& [name, buffers] : m_meshBufferMap)
  {
    UnregisterMesh(name);
  }
  m_meshBufferMap.clear();

  if (m_descriptorManager)
  {
    m_descriptorManager->Shutdown();
    m_descriptorManager.reset();
  }

  if (m_commandBufferManager)
  {
    m_commandBufferManager->Shutdown();
    m_commandBufferManager.reset();
  }

  if (m_bufferManager)
  {
    m_bufferManager->Shutdown();
    m_bufferManager.reset();
  }

  if (m_pipelineManager)
  {
    m_pipelineManager->Shutdown();
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
  VkDevice device = m_deviceManager->GetDevice();

  vkWaitForFences(device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(device, m_swapchainManager->GetSwapchain(), UINT64_MAX,
                                          m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_frameBufferResized)
  {
    m_frameBufferResized = false;
    m_swapchainManager->RecreateSwapchain();
    return;
  }
  else if (result != VK_SUCCESS)
  {
    CE_RENDER_ERROR("Failed to acquire swap chain image");
    return;
  }

  if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE)
  {
    vkWaitForFences(device, 1, &m_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
  }
  m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];

  vkResetFences(device, 1, &m_inFlightFences[m_currentFrame]);

  UpdateUniformBuffers(renderData);

  RecordCommandBuffer(imageIndex, renderData);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &m_commandBufferManager->GetCommandBuffers()[imageIndex];

  VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  result = vkQueueSubmit(m_deviceManager->GetGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame]);
  VK_CHECK(result, "Failed to submit draw command buffer");

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {m_swapchainManager->GetSwapchain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  result = vkQueuePresentKHR(m_deviceManager->GetPresentQueue(), &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_frameBufferResized)
  {
    m_frameBufferResized = false;
    m_swapchainManager->RecreateSwapchain();
  }
  else if (result != VK_SUCCESS)
  {
    CE_RENDER_ERROR("Failed to present swap chain image");
  }

  m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void CE::VulkanContext::RegisterMesh(const std::string& name, const StaticMesh& mesh)
{
  std::string vertexBufferName = name + "_vertices";
  if (!m_bufferManager->CreateVertexBuffer(vertexBufferName, mesh.vertices))
  {
    CE_RENDER_ERROR("Failed to create vertex buffer for mesh: ", name);
    return;
  }

  std::string indexBufferName = name + "_indices";
  if (!m_bufferManager->CreateIndexBuffer(indexBufferName, mesh.indices))
  {
    CE_RENDER_ERROR("Failed to create index buffer for mesh: ", name);
    m_bufferManager->DestroyBuffer(vertexBufferName);
    return;
  }

  std::string modelUBOName = name + "_model_ubo";
  if (!m_bufferManager->CreateUniformBuffer(modelUBOName, sizeof(ModelUBO)))
  {
    CE_RENDER_ERROR("Failed to create model UBO for mesh: ", name);
    m_bufferManager->DestroyBuffer(vertexBufferName);
    m_bufferManager->DestroyBuffer(indexBufferName);
    return;
  }

  VkDescriptorSetLayout pipelineLayout = m_pipelineManager->GetDescriptorSetLayout();
  if (!m_descriptorManager->CreateMeshDescriptorSet(name, pipelineLayout))
  {
    CE_RENDER_ERROR("Failed to create descriptor set for mesh: ", name);
    m_bufferManager->DestroyBuffer(vertexBufferName);
    m_bufferManager->DestroyBuffer(indexBufferName);
    m_bufferManager->DestroyBuffer(modelUBOName);
    return;
  }

  if (!m_descriptorManager->UpdateMeshDescriptorSet(name,
                                                    m_sceneUBOBufferName,
                                                    modelUBOName,
                                                    m_lightingUBOBufferName))
  {
    CE_RENDER_ERROR("Failed to update descriptor set for mesh: ", name);
    m_bufferManager->DestroyBuffer(vertexBufferName);
    m_bufferManager->DestroyBuffer(indexBufferName);
    m_bufferManager->DestroyBuffer(modelUBOName);
    return;
  }

  MeshBuffers buffers;
  buffers.vertexBufferName = vertexBufferName;
  buffers.indexBufferName = indexBufferName;
  buffers.modelUBOName = modelUBOName;
  m_meshBufferMap[name] = buffers;
}

void CE::VulkanContext::UnregisterMesh(const std::string& name)
{
  auto it = m_meshBufferMap.find(name);
  if (it != m_meshBufferMap.end())
  {
    const auto& buffers = it->second;
    m_bufferManager->DestroyBuffer(buffers.vertexBufferName);
    m_bufferManager->DestroyBuffer(buffers.indexBufferName);
    m_bufferManager->DestroyBuffer(buffers.modelUBOName);
    m_meshBufferMap.erase(name);
  }

  CE_RENDER_DEBUG("Unregistered mesh: ", name);
}

void CE::VulkanContext::CreateSyncObjects()
{
  uint32_t imageCount = m_swapchainManager->GetImageCount();

  m_imageAvailableSemaphores.resize(imageCount);
  m_renderFinishedSemaphores.resize(imageCount);
  m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
  m_imagesInFlight.resize(imageCount, VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < imageCount; i++)
  {
    VkResult result1 = vkCreateSemaphore(m_deviceManager->GetDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]);
    VkResult result2 = vkCreateSemaphore(m_deviceManager->GetDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]);

    if (result1 != VK_SUCCESS || result2 != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create synchronization objects for swapchain images!");
    }
  }

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    VkResult result = vkCreateFence(m_deviceManager->GetDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]);
    if (result != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create fence!");
    }
  }

  CE_RENDER_DEBUG("Created synchronization objects for ", imageCount, " swapchain images and ", MAX_FRAMES_IN_FLIGHT, " frames in flight");
}

void CE::VulkanContext::CleanupSyncObjects()
{
  VkDevice device = m_deviceManager->GetDevice();

  for (size_t i = 0; i < m_imageAvailableSemaphores.size(); i++)
  {
    if (m_imageAvailableSemaphores[i] != VK_NULL_HANDLE)
    {
      vkDestroySemaphore(device, m_imageAvailableSemaphores[i], nullptr);
      m_imageAvailableSemaphores[i] = VK_NULL_HANDLE;
    }
  }

  for (size_t i = 0; i < m_renderFinishedSemaphores.size(); i++)
  {
    if (m_renderFinishedSemaphores[i] != VK_NULL_HANDLE)
    {
      vkDestroySemaphore(device, m_renderFinishedSemaphores[i], nullptr);
      m_renderFinishedSemaphores[i] = VK_NULL_HANDLE;
    }
  }

  for (size_t i = 0; i < m_inFlightFences.size(); i++)
  {
    if (m_inFlightFences[i] != VK_NULL_HANDLE)
    {
      vkDestroyFence(device, m_inFlightFences[i], nullptr);
      m_inFlightFences[i] = VK_NULL_HANDLE;
    }
  }

  CE_RENDER_DEBUG("Synchronization objects destroyed");
}

void CE::VulkanContext::RecordCommandBuffer(uint32_t imageIndex, const FrameRenderData& renderData)
{
  m_commandBufferManager->BeginRecording(imageIndex);

  std::vector<VkClearValue> clearValues(2);
  clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil = {1.0f, 0};

  m_commandBufferManager->BeginRenderPass(imageIndex,
                                          m_swapchainManager->GetRenderPass(),
                                          m_swapchainManager->GetFramebuffers()[imageIndex],
                                          m_swapchainManager->GetExtent(),
                                          clearValues);

  VkPipeline meshPipeline = m_pipelineManager->GetPipeline("mesh");
  if (meshPipeline != VK_NULL_HANDLE)
  {
    m_commandBufferManager->BindPipeline(imageIndex, meshPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapchainManager->GetExtent().width);
    viewport.height = static_cast<float>(m_swapchainManager->GetExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    m_commandBufferManager->SetViewport(imageIndex, viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapchainManager->GetExtent();
    m_commandBufferManager->SetScissor(imageIndex, scissor);

    for (const auto& renderObject : renderData.renderObjects)
    {
      if (!renderObject.mesh)
        continue;

      std::string meshName = "mesh_" + std::to_string(reinterpret_cast<uintptr_t>(renderObject.mesh));

      if (m_meshBufferMap.find(meshName) == m_meshBufferMap.end())
      {
        RegisterMesh(meshName, *renderObject.mesh);
      }

      const auto& meshBuffers = m_meshBufferMap[meshName];

      ModelUBO modelUBO = renderData.GetModelUBO(renderObject.transform, renderObject.color);

      m_bufferManager->UpdateUniformBuffer(meshBuffers.modelUBOName, &modelUBO, sizeof(ModelUBO));

      VkBuffer vertexBuffer = m_bufferManager->GetBuffer(meshBuffers.vertexBufferName);
      VkBuffer indexBuffer = m_bufferManager->GetBuffer(meshBuffers.indexBufferName);

      if (vertexBuffer != VK_NULL_HANDLE && indexBuffer != VK_NULL_HANDLE)
      {
        VkDescriptorSet meshDescriptorSet = m_descriptorManager->GetMeshDescriptorSet(meshName);
        if (meshDescriptorSet != VK_NULL_HANDLE)
        {
          std::vector<VkDescriptorSet> descriptorSets = {meshDescriptorSet};
          m_commandBufferManager->BindDescriptorSets(imageIndex,
                                                     m_pipelineManager->GetPipelineLayout(),
                                                     0, descriptorSets);
        }

        std::vector<VkBuffer> vertexBuffers = {vertexBuffer};
        std::vector<VkDeviceSize> offsets = {0};
        m_commandBufferManager->BindVertexBuffers(imageIndex, 0, vertexBuffers, offsets);
        m_commandBufferManager->BindIndexBuffer(imageIndex, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        m_commandBufferManager->DrawIndexed(imageIndex,
                                            static_cast<uint32_t>(renderObject.mesh->indices.size()),
                                            1, 0, 0, 0);
      }
    }
  }

  m_commandBufferManager->EndRenderPass(imageIndex);

  m_commandBufferManager->EndRecording(imageIndex);
}

void CE::VulkanContext::UpdateUniformBuffers(const FrameRenderData& renderData)
{
  LightingUBO lightingUBO = renderData.lighting;

  m_bufferManager->UpdateUniformBuffer(m_lightingUBOBufferName, &lightingUBO, sizeof(LightingUBO));

  SceneUBO sceneUBO = renderData.GetSceneUBO();
  m_bufferManager->UpdateUniformBuffer(m_sceneUBOBufferName, &sceneUBO, sizeof(SceneUBO));
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

  if (m_info->Fullscreen)
  {
    // Borderless Fullscreen режим
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // Устанавливаем параметры видеорежима для borderless
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    // Отключаем рамку и заголовок окна
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(
        mode->width,
        mode->height,
        m_info->AppName.c_str(),
        nullptr,  // nullptr для borderless fullscreen
        nullptr);

    // Перемещаем окно на основной монитор
    if (m_window)
    {
      glfwSetWindowPos(m_window, 0, 0);
    }

    CE_RENDER_DEBUG("Borderless fullscreen window created: ", mode->width, "x", mode->height, " @", mode->refreshRate, "Hz");
  }
  else
  {
    // Оконный режим
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);  // Включаем рамку

    m_window = glfwCreateWindow(m_info->Width, m_info->Height, m_info->AppName.c_str(), nullptr, nullptr);
    CE_RENDER_DEBUG("Windowed window created : ", m_info->Width, "x", m_info->Height);
  }

  if (!m_window)
  {
    CE_RENDER_DEBUG("Failed to Create window");
    glfwTerminate();
    return false;
  }

  glfwSetWindowUserPointer(m_window, this);

  glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
                                 {
                                   (void)width;
                                   (void)height;
                                   auto ctx = reinterpret_cast<CE::VulkanContext*>(glfwGetWindowUserPointer(window));
                                   if (ctx)
                                     ctx->OnFramebufferResized(); });
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