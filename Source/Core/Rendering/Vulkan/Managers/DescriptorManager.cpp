#include "Core/Rendering/Vulkan/Managers/DescriptorManager.h"

#include <stdexcept>

#include "Core/Rendering/Data/RenderData.h"

namespace CE
{
  DescriptorManager::DescriptorManager(std::shared_ptr<DeviceManager> deviceManager, std::shared_ptr<BufferManager> bufferManager)
      : m_deviceManager(deviceManager), m_bufferManager(bufferManager)
  {
  }

  DescriptorManager::~DescriptorManager()
  {
    Shutdown();
  }

  bool DescriptorManager::Initialize()
  {
    CE_RENDER_DEBUG("Initializing DescriptorManager...");

    // Создаем пул дескрипторов
    std::array<VkDescriptorPoolSize, 3> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 100;

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = 100;

    poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[2].descriptorCount = 100;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 100;
    VkResult result = vkCreateDescriptorPool(m_deviceManager->GetDevice(), &poolInfo, nullptr, &m_descriptorPool);
    VK_CHECK(result, "Failed to create descriptor pool");

    CE_RENDER_DEBUG("DescriptorManager initialized successfully");
    return true;
  }

  void DescriptorManager::Shutdown()
  {
    if (m_descriptorPool != VK_NULL_HANDLE)
    {
      vkDestroyDescriptorPool(m_deviceManager->GetDevice(), m_descriptorPool, nullptr);
      m_descriptorPool = VK_NULL_HANDLE;
      CE_RENDER_DEBUG("Descriptor pool destroyed");
    }

    CE_RENDER_DEBUG("DescriptorManager shutdown complete");
  }

  bool DescriptorManager::CreateDescriptorSets(VkDescriptorSetLayout layout, uint32_t count)
  {
    m_descriptorSets.resize(count);
    std::vector<VkDescriptorSetLayout> layouts(count, layout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = count;
    allocInfo.pSetLayouts = layouts.data();

    VkResult result = vkAllocateDescriptorSets(m_deviceManager->GetDevice(), &allocInfo, m_descriptorSets.data());
    if (result != VK_SUCCESS)
    {
      CE_RENDER_ERROR("Failed to allocate descriptor sets");
      return false;
    }

    CE_RENDER_DEBUG("Created ", count, " descriptor sets");
    return true;
  }

  VkDescriptorSet DescriptorManager::GetDescriptorSet(uint32_t imageIndex) const
  {
    if (imageIndex < m_descriptorSets.size())
    {
      return m_descriptorSets[imageIndex];
    }
    return VK_NULL_HANDLE;
  }

  bool DescriptorManager::CreateMeshDescriptorSet(const std::string& meshName, VkDescriptorSetLayout layout)
  {
    if (m_meshDescriptorSets.find(meshName) != m_meshDescriptorSets.end())
    {
      CE_RENDER_WARN("Descriptor set for mesh '", meshName, "' already exists");
      return true;
    }

    VkDescriptorSet descriptorSet;
    VkDescriptorSetLayout layouts[] = {layout};

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;

    VkResult result = vkAllocateDescriptorSets(m_deviceManager->GetDevice(), &allocInfo, &descriptorSet);
    if (result != VK_SUCCESS)
    {
      CE_RENDER_ERROR("Failed to allocate descriptor set for mesh '", meshName, "'");
      return false;
    }

    m_meshDescriptorSets[meshName] = descriptorSet;

    return true;
  }

  VkDescriptorSet DescriptorManager::GetMeshDescriptorSet(const std::string& meshName) const
  {
    auto it = m_meshDescriptorSets.find(meshName);
    return it != m_meshDescriptorSets.end() ? it->second : VK_NULL_HANDLE;
  }

  bool DescriptorManager::UpdateMeshDescriptorSet(const std::string& meshName,
                                                  const std::string& sceneUBOName,
                                                  const std::string& modelUBOName,
                                                  const std::string& lightingUBOName)
  {
    auto it = m_meshDescriptorSets.find(meshName);
    if (it == m_meshDescriptorSets.end())
    {
      CE_RENDER_ERROR("Descriptor set for mesh '", meshName, "' not found");
      return false;
    }

    std::vector<VkWriteDescriptorSet> descriptorWrites;

    VkBuffer sceneBuffer = m_bufferManager->GetBuffer(sceneUBOName);
    VkDeviceSize sceneBufferSize = m_bufferManager->GetBufferSize(sceneUBOName);
    if (sceneBuffer == VK_NULL_HANDLE)
      return false;

    VkDescriptorBufferInfo sceneBufferInfo{};
    sceneBufferInfo.buffer = sceneBuffer;
    sceneBufferInfo.offset = 0;
    sceneBufferInfo.range = sceneBufferSize;

    VkWriteDescriptorSet sceneWrite{};
    sceneWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    sceneWrite.dstSet = it->second;
    sceneWrite.dstBinding = 0;
    sceneWrite.dstArrayElement = 0;
    sceneWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    sceneWrite.descriptorCount = 1;
    sceneWrite.pBufferInfo = &sceneBufferInfo;
    descriptorWrites.push_back(sceneWrite);

    VkBuffer modelBuffer = m_bufferManager->GetBuffer(modelUBOName);
    VkDeviceSize modelBufferSize = m_bufferManager->GetBufferSize(modelUBOName);
    if (modelBuffer == VK_NULL_HANDLE)
      return false;

    VkDescriptorBufferInfo modelBufferInfo{};
    modelBufferInfo.buffer = modelBuffer;
    modelBufferInfo.offset = 0;
    modelBufferInfo.range = modelBufferSize;

    VkWriteDescriptorSet modelWrite{};
    modelWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    modelWrite.dstSet = it->second;
    modelWrite.dstBinding = 1;
    modelWrite.dstArrayElement = 0;
    modelWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    modelWrite.descriptorCount = 1;
    modelWrite.pBufferInfo = &modelBufferInfo;
    descriptorWrites.push_back(modelWrite);

    VkBuffer lightingBuffer = m_bufferManager->GetBuffer(lightingUBOName);
    VkDeviceSize lightingBufferSize = m_bufferManager->GetBufferSize(lightingUBOName);
    if (lightingBuffer == VK_NULL_HANDLE)
      return false;

    VkDescriptorBufferInfo lightingBufferInfo{};
    lightingBufferInfo.buffer = lightingBuffer;
    lightingBufferInfo.offset = 0;
    lightingBufferInfo.range = lightingBufferSize;

    VkWriteDescriptorSet lightingWrite{};
    lightingWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    lightingWrite.dstSet = it->second;
    lightingWrite.dstBinding = 2;
    lightingWrite.dstArrayElement = 0;
    lightingWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightingWrite.descriptorCount = 1;
    lightingWrite.pBufferInfo = &lightingBufferInfo;
    descriptorWrites.push_back(lightingWrite);

    vkUpdateDescriptorSets(m_deviceManager->GetDevice(),
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(), 0, nullptr);

    return true;
  }
}  // namespace CE