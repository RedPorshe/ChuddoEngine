#pragma once
#include <vulkan/vulkan.h>

#include <fstream>
#include <string>
#include <vector>

#include "Core/Render/Vulkan/VKContext.hpp"

namespace CE
{

  struct PipelineConfig;

  class PipelineManager
  {
   public:
    PipelineManager(VKContext& context);
    ~PipelineManager();
    VkPipeline GetPipeline(const std::string& name) const;
    VkPipelineLayout GetPipelineLayout(const std::string& name) const;
    void SetDescriptorSetLayout(VkDescriptorSetLayout layout)
    {
      m_descriptorSetLayout = layout;
    }
    bool IsInitialized() const
    {
      return !m_context.Pipelines.empty();
    }

    void Destroy();

   private:
    VKContext& m_context;
    VkDescriptorSetLayout m_descriptorSetLayout{VK_NULL_HANDLE};

    void CreatePipeline(const std::string& name, const PipelineConfig& config);
    void CreatetrianglePipeline();
    void CreateMeshPipeline();
    std::vector<char> ReadFile(const std::string& filename);
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
  };
}  // namespace CE
