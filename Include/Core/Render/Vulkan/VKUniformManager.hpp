#pragma once
#include "Renderer/Vulkan/VKBufferManager.hpp"
#include "VKConfig.hpp"

namespace CE
{

  class VKUniformManager
  {
   public:
    VKUniformManager(VKInfo& vkInfo, VKBufferManager& bufferManager);

    bool Initialize();
    // ДОБАВЛЯЕМ методы для работы с несколькими объектами
    void UpdateSceneUBO(uint32_t currentFrame, const SceneUBO& sceneUBO);
    void UpdateModelUBO(uint32_t currentFrame, uint32_t objectIndex, const ModelUBO& modelUBO);
    void UpdateLightingUBO(uint32_t currentFrame, const LightingUBO& lightingUBO);
    bool CreateLightingUniformBuffers();
    // ДОБАВЛЯЕМ методы для динамических offsets
    uint32_t GetDynamicAlignment() const
    {
      return m_dynamicAlignment;
    }
    uint32_t GetMaxObjects() const
    {
      return m_maxObjects;
    }
    VkDescriptorSetLayout GetDescriptorSetLayout() const
    {
      return m_descriptorSetLayout;
    }
    VkDescriptorSet GetDescriptorSet(uint32_t frameIndex) const
    {
      return m_descriptorSets[frameIndex];
    }
    void Destroy();

   private:
    VKInfo& m_vkInfo;
    VKBufferManager& m_bufferManager;

    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_descriptorSets;

    bool CreateSceneUniformBuffers();
    bool CreateModelUniformBuffers();

    uint32_t m_dynamicAlignment;
    uint32_t m_maxObjects = 100;  // Максимум объектов
    size_t m_modelUBOSize;

    bool CreateDescriptorSetLayout();
    bool CreateDescriptorPool();
    bool CreateDescriptorSets();
  };
}  // namespace CE