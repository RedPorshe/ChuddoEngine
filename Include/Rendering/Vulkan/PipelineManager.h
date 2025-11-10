#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

#include "CoreMinimal.h"
#include "Rendering/Data/Vertex.h"
#include "Rendering/Vulkan/DeviceManager.h"
#include "Rendering/Vulkan/SwapchainManager.h"
#include "vulkan/vulkan.h"

namespace CE
{
  // Конфигурация пайплайна по умолчанию
  struct PipelineConfigInfo
  {
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    VkPipelineViewportStateCreateInfo viewportInfo{};
    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    uint32_t subpass = 0;
  };

  class PipelineManager
  {
   public:
    PipelineManager(std::shared_ptr<DeviceManager> deviceManager);
    ~PipelineManager();

    // Удаляем копирование
    PipelineManager(const PipelineManager&) = delete;
    PipelineManager& operator=(const PipelineManager&) = delete;

    bool Initialize();
    void Shutdown();

    // Создание пайплайна для мешей
    VkPipeline CreateMeshPipeline(const std::string& name, VkRenderPass renderPass);

    // Получение пайплайна по имени
    VkPipeline GetPipeline(const std::string& name) const;
    VkPipelineLayout GetPipelineLayout() const
    {
      return m_pipelineLayout;
    }
    VkDescriptorSetLayout GetDescriptorSetLayout() const
    {
      return m_descriptorSetLayout;
    }
    // Статические методы для конфигурации
    static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
    static void EnableAlphaBlending(PipelineConfigInfo& configInfo);

   private:
    bool CreatePipelineLayout();
    void DestroyPipelineLayout();

    VkPipeline CreateGraphicsPipeline(
        const PipelineConfigInfo& configInfo,
        const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages);

    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    std::vector<char> ReadShaderFile(const std::string& filename);

   private:
    std::shared_ptr<DeviceManager> m_deviceManager;

    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    std::unordered_map<std::string, VkPipeline> m_pipelines;

    // Дескрипторные наборы для UBO
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;

    // Стандартные шейдеры для мешей
    // Use the same casing the build copies shaders to (Assets/Shaders)
    static constexpr const char* VERTEX_SHADER_PATH = "Assets/Shaders/mesh_vert.spv";
    static constexpr const char* FRAGMENT_SHADER_PATH = "Assets/Shaders/mesh_frag.spv";
  };
}  // namespace CE