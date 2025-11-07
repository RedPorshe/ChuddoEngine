#pragma once

#include <memory>
#include <vector>

#include "CoreMinimal.h"
#include "Rendering/Data/Vertex.h"
#include "Rendering/Vulkan/DeviceManager.h"
#include "Rendering/Vulkan/SwapchainManager.h"
#include "vulkan/vulkan.h"

namespace CE
{
  class PipelineManager
  {
   public:
    PipelineManager(std::shared_ptr<DeviceManager> deviceManager, std::shared_ptr<SwapchainManager> swapchainManager);
    ~PipelineManager();

    bool Initialize();
    void Cleanup();

    // Getters
    VkPipeline GetGraphicsPipeline() const
    {
      return m_graphicsPipeline;
    }
    VkPipelineLayout GetPipelineLayout() const
    {
      return m_pipelineLayout;
    }
    VkDescriptorSetLayout GetDescriptorSetLayout() const
    {
      return m_descriptorSetLayout;
    }

   private:
    void CreateDescriptorSetLayout();
    void CreateGraphicsPipeline();

    VkShaderModule CreateShaderModule(const std::vector<char>& code);

   private:
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<SwapchainManager> m_swapchainManager;

    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
  };
}  // namespace CE