#include "Renderer/Vulkan/VKUniformManager.hpp"
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

namespace CE
{
  VKUniformManager::VKUniformManager(VKInfo& vkInfo, VKBufferManager& bufferManager)
      : m_vkInfo{vkInfo}, m_bufferManager{bufferManager}
  {
  }

  void VKUniformManager::Destroy()
  {
    if (m_descriptorPool != VK_NULL_HANDLE)
    {
      vkDestroyDescriptorPool(m_vkInfo.device, m_descriptorPool, nullptr);
      m_descriptorPool = VK_NULL_HANDLE;
    }

    if (m_descriptorSetLayout != VK_NULL_HANDLE)
    {
      vkDestroyDescriptorSetLayout(m_vkInfo.device, m_descriptorSetLayout, nullptr);
      m_descriptorSetLayout = VK_NULL_HANDLE;
    }

    m_descriptorSets.clear();
  }

  bool VKUniformManager::Initialize()
  {
    // ВЫЧИСЛЯЕМ dynamic alignment
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_vkInfo.PhysicalDevice, &properties);

    m_modelUBOSize = sizeof(ModelUBO);
    m_dynamicAlignment = (m_modelUBOSize + properties.limits.minUniformBufferOffsetAlignment - 1) & ~(properties.limits.minUniformBufferOffsetAlignment - 1);

    // СОЗДАЕМ ТРИ ТИПА БУФЕРОВ (добавили lighting)
    if (!CreateSceneUniformBuffers())
      return false;
    if (!CreateModelUniformBuffers())
      return false;
    if (!CreateLightingUniformBuffers())  // <-- ДОБАВИЛИ
      return false;
    if (!CreateDescriptorSetLayout())
      return false;
    if (!CreateDescriptorPool())
      return false;
    if (!CreateDescriptorSets())
      return false;

    return true;
  }

  bool VKUniformManager::CreateSceneUniformBuffers()
  {
    return m_bufferManager.CreateUniformBuffers("scene_ubo", sizeof(SceneUBO));
  }

  bool VKUniformManager::CreateModelUniformBuffers()
  {
    size_t bufferSize = m_dynamicAlignment * m_maxObjects;
    return m_bufferManager.CreateUniformBuffers("model_ubo", bufferSize);
  }

  bool VKUniformManager::CreateLightingUniformBuffers()
  {
    // Проверка структуры LightingUBO
    std::cout << "=== LightingUBO Structure Analysis ===" << std::endl;
    std::cout << "Total size: " << sizeof(LightingUBO) << " bytes" << std::endl;
    std::cout << "Field offsets:" << std::endl;
    std::cout << "  lightPositions[0]: " << offsetof(LightingUBO, lightPositions[0]) << std::endl;
    std::cout << "  lightColors[0]: " << offsetof(LightingUBO, lightColors[0]) << std::endl;
    std::cout << "  lightIntensities[0]: " << offsetof(LightingUBO, lightIntensities[0]) << std::endl;
    std::cout << "  ambientColor: " << offsetof(LightingUBO, ambientColor) << std::endl;
    std::cout << "  ambientIntensity: " << offsetof(LightingUBO, ambientIntensity) << std::endl;
    std::cout << "  lightCount: " << offsetof(LightingUBO, lightCount) << std::endl;

    bool success = m_bufferManager.CreateUniformBuffers("lighting_ubo", sizeof(LightingUBO));

    if (success)
    {
      CE_CORE_INFO("Successfully created lighting uniform buffers, size: {} bytes", sizeof(LightingUBO));
    }
    else
    {
      CE_CORE_ERROR("Failed to create lighting uniform buffers");
    }

    return success;
  }

  bool VKUniformManager::CreateDescriptorSetLayout()
  {
    // Scene UBO binding
    VkDescriptorSetLayoutBinding sceneBinding{};
    sceneBinding.binding = 0;
    sceneBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    sceneBinding.descriptorCount = 1;
    sceneBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    // Model UBO binding (DYNAMIC)
    VkDescriptorSetLayoutBinding modelBinding{};
    modelBinding.binding = 1;
    modelBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    modelBinding.descriptorCount = 1;
    modelBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    // Lighting UBO binding (ДОБАВИЛИ)
    VkDescriptorSetLayoutBinding lightingBinding{};
    lightingBinding.binding = 2;
    lightingBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightingBinding.descriptorCount = 1;
    lightingBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    // std::array<VkDescriptorSetLayoutBinding, 3> bindings = {sceneBinding, modelBinding, lightingBinding};
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.push_back(sceneBinding);
    bindings.push_back(modelBinding);
    bindings.push_back(lightingBinding);

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    return vkCreateDescriptorSetLayout(m_vkInfo.device, &layoutInfo, nullptr, &m_descriptorSetLayout) == VK_SUCCESS;
  }

  bool VKUniformManager::CreateDescriptorPool()
  {
    std::array<VkDescriptorPoolSize, 3> poolSizes{};  // Увеличили до 3
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT * 2;  // scene + lighting

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;

    poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;  // для lighting
    poolSizes[2].descriptorCount = MAX_FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

    return vkCreateDescriptorPool(m_vkInfo.device, &poolInfo, nullptr, &m_descriptorPool) == VK_SUCCESS;
  }

  bool VKUniformManager::CreateDescriptorSets()
  {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(m_vkInfo.device, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS)
    {
      return false;
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      // Scene UBO
      VkDescriptorBufferInfo sceneBufferInfo{};
      sceneBufferInfo.buffer = m_bufferManager.GetUniformBuffer("scene_ubo", static_cast<uint32_t>(i));
      sceneBufferInfo.offset = 0;
      sceneBufferInfo.range = sizeof(SceneUBO);

      // Model UBO (dynamic)
      VkDescriptorBufferInfo modelBufferInfo{};
      modelBufferInfo.buffer = m_bufferManager.GetUniformBuffer("model_ubo", static_cast<uint32_t>(i));
      modelBufferInfo.offset = 0;
      modelBufferInfo.range = sizeof(ModelUBO);  // Размер одного объекта

      // Lighting UBO
      VkDescriptorBufferInfo lightingBufferInfo{};
      lightingBufferInfo.buffer = m_bufferManager.GetUniformBuffer("lighting_ubo", static_cast<uint32_t>(i));
      lightingBufferInfo.offset = 0;
      lightingBufferInfo.range = sizeof(LightingUBO);

      std::cout << "Lighting buffer for frame " << i << ": "
                << (lightingBufferInfo.buffer != VK_NULL_HANDLE ? "VALID" : "NULL") << "\n";

      std::array<VkWriteDescriptorSet, 3> descriptorWrites{};  // Увеличили до 3

      // Scene UBO (binding 0)
      descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[0].dstSet = m_descriptorSets[i];
      descriptorWrites[0].dstBinding = 0;
      descriptorWrites[0].dstArrayElement = 0;
      descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrites[0].descriptorCount = 1;
      descriptorWrites[0].pBufferInfo = &sceneBufferInfo;

      // Model UBO (binding 1)
      descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[1].dstSet = m_descriptorSets[i];
      descriptorWrites[1].dstBinding = 1;
      descriptorWrites[1].dstArrayElement = 0;
      descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
      descriptorWrites[1].descriptorCount = 1;
      descriptorWrites[1].pBufferInfo = &modelBufferInfo;

      // Lighting UBO (binding 2)
      descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[2].dstSet = m_descriptorSets[i];
      descriptorWrites[2].dstBinding = 2;
      descriptorWrites[2].dstArrayElement = 0;
      descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrites[2].descriptorCount = 1;
      descriptorWrites[2].pBufferInfo = &lightingBufferInfo;

      vkUpdateDescriptorSets(m_vkInfo.device, static_cast<uint32_t>(descriptorWrites.size()),
                             descriptorWrites.data(), 0, nullptr);
    }

    return true;
  }

  void VKUniformManager::UpdateSceneUBO(uint32_t currentFrame, const SceneUBO& sceneUBO)
  {
    VkBuffer uniformBuffer = m_bufferManager.GetUniformBuffer("scene_ubo", currentFrame);
    VkDeviceMemory memory = m_bufferManager.GetUniformBufferMemory("scene_ubo", currentFrame);

    if (uniformBuffer == VK_NULL_HANDLE || memory == VK_NULL_HANDLE)
    {
      CE_CORE_ERROR("Failed to get scene UBO buffer/memory for frame {}", currentFrame);
      return;
    }

    void* data;
    VkResult result = vkMapMemory(m_vkInfo.device, memory, 0, sizeof(SceneUBO), 0, &data);
    if (result != VK_SUCCESS)
    {
      CE_CORE_ERROR("Failed to map scene UBO memory for frame {}", currentFrame);
      return;
    }

    memcpy(data, &sceneUBO, sizeof(SceneUBO));
    vkUnmapMemory(m_vkInfo.device, memory);

    CE_CORE_TRACE("Updated scene UBO for frame {}", currentFrame);
  }

  void VKUniformManager::UpdateModelUBO(uint32_t currentFrame, uint32_t objectIndex, const ModelUBO& modelUBO)
  {
    VkBuffer uniformBuffer = m_bufferManager.GetUniformBuffer("model_ubo", currentFrame);
    VkDeviceMemory memory = m_bufferManager.GetUniformBufferMemory("model_ubo", currentFrame);

    if (uniformBuffer == VK_NULL_HANDLE || memory == VK_NULL_HANDLE)
    {
      CE_CORE_ERROR("Failed to get model UBO buffer/memory for frame {}", currentFrame);
      return;
    }

    uint32_t offset = objectIndex * m_dynamicAlignment;

    void* data;
    VkResult result = vkMapMemory(m_vkInfo.device, memory, offset, sizeof(ModelUBO), 0, &data);
    if (result != VK_SUCCESS)
    {
      CE_CORE_ERROR("Failed to map model UBO memory for frame {}, object {}", currentFrame, objectIndex);
      return;
    }

    memcpy(data, &modelUBO, sizeof(ModelUBO));
    vkUnmapMemory(m_vkInfo.device, memory);
  }

  void VKUniformManager::UpdateLightingUBO(uint32_t currentFrame, const LightingUBO& lightingUBO)
  {
    VkBuffer uniformBuffer = m_bufferManager.GetUniformBuffer("lighting_ubo", currentFrame);
    VkDeviceMemory memory = m_bufferManager.GetUniformBufferMemory("lighting_ubo", currentFrame);

    if (uniformBuffer == VK_NULL_HANDLE || memory == VK_NULL_HANDLE)
    {
      CE_CORE_ERROR("Failed to get lighting UBO buffer/memory for frame {}", currentFrame);
      return;
    }

    // Детальная отладочная информация
    CE_CORE_INFO("UPDATING Lighting UBO - Frame: {}, LightCount: {}, AmbientIntensity: {}",
                 currentFrame, lightingUBO.lightCount, lightingUBO.ambientIntensity);

    CE_CORE_DEBUG("Light positions[0]: ({}, {}, {}, {})",
                  lightingUBO.lightPositions[0].x, lightingUBO.lightPositions[0].y,
                  lightingUBO.lightPositions[0].z, lightingUBO.lightPositions[0].w);
    CE_CORE_DEBUG("Ambient color: ({}, {}, {}, {})",
                  lightingUBO.ambientColor.x, lightingUBO.ambientColor.y,
                  lightingUBO.ambientColor.z, lightingUBO.ambientColor.w);

    void* data;
    VkResult result = vkMapMemory(m_vkInfo.device, memory, 0, sizeof(LightingUBO), 0, &data);
    if (result != VK_SUCCESS)
    {
      CE_CORE_ERROR("Failed to map lighting UBO memory for frame {}", currentFrame);
      return;
    }

    memcpy(data, &lightingUBO, sizeof(LightingUBO));
    vkUnmapMemory(m_vkInfo.device, memory);

    CE_CORE_TRACE("Updated lighting UBO for frame {}", currentFrame);
  }

}  // namespace CE