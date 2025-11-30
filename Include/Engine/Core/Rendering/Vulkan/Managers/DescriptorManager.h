#pragma once
#include <memory>
#include <vector>

#include "CoreMinimal.h"
#include "Engine/Core/Rendering/Vulkan/Managers/BufferManager.h"
#include "Engine/Core/Rendering/Vulkan/Managers/DeviceManager.h"
#include "vulkan/vulkan.h"

namespace CE
{
  class DescriptorManager
  {
   public:
    DescriptorManager(std::shared_ptr<DeviceManager> deviceManager, std::shared_ptr<BufferManager> bufferManager);
    ~DescriptorManager();

    // Удаляем копирование
    DescriptorManager(const DescriptorManager&) = delete;
    DescriptorManager& operator=(const DescriptorManager&) = delete;

    bool Initialize();
    void Shutdown();

    // Создание дескрипторных наборов
    bool CreateDescriptorSets(VkDescriptorSetLayout layout, uint32_t count);
    VkDescriptorSet GetDescriptorSet(uint32_t imageIndex) const;

    bool CreateMeshDescriptorSet(const std::string& meshName, VkDescriptorSetLayout layout);
    VkDescriptorSet GetMeshDescriptorSet(const std::string& meshName) const;
    bool UpdateMeshDescriptorSet(const std::string& meshName,
                                 const std::string& sceneUBOName,
                                 const std::string& modelUBOName,
                                 const std::string& lightingUBOName);
    VkDescriptorPool GetDescriptorPool() const { return m_descriptorPool; }

   private:
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<BufferManager> m_bufferManager;
    std::unordered_map<std::string, VkDescriptorSet> m_meshDescriptorSets;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_descriptorSets;
  };
}  // namespace CE