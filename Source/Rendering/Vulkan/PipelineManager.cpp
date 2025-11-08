#include "Rendering/Vulkan/PipelineManager.h"

#include <fstream>
#include <stdexcept>

namespace CE
{
  PipelineManager::PipelineManager(std::shared_ptr<DeviceManager> deviceManager)
      : m_deviceManager(deviceManager)
  {
  }

  PipelineManager::~PipelineManager()
  {
    Shutdown();
  }

  bool PipelineManager::Initialize()
  {
    CE_RENDER_DEBUG("Initializing PipelineManager...");

    if (!CreatePipelineLayout())
    {
      CE_RENDER_ERROR("Failed to create pipeline layout");
      return false;
    }

    CE_RENDER_DEBUG("PipelineManager initialized successfully");
    return true;
  }

  void PipelineManager::Shutdown()
  {
    VkDevice device = m_deviceManager->GetDevice();

    // Уничтожаем все пайплайны
    for (auto& [name, pipeline] : m_pipelines)
    {
      vkDestroyPipeline(device, pipeline, nullptr);
      CE_RENDER_DEBUG("Destroyed pipeline: ", name);
    }
    m_pipelines.clear();

    // Уничтожаем layout пайплайна
    DestroyPipelineLayout();

    CE_RENDER_DEBUG("PipelineManager shutdown complete");
  }

  VkPipeline PipelineManager::CreateMeshPipeline(const std::string& name, VkRenderPass renderPass)
  {
    // Проверяем, не создан ли уже такой пайплайн
    auto it = m_pipelines.find(name);
    if (it != m_pipelines.end())
    {
      CE_RENDER_WARN("Pipeline '", name, "' already exists");
      return it->second;
    }

    CE_RENDER_DEBUG("Creating mesh pipeline: ", name);

    try
    {
      // Загружаем шейдеры
      auto vertShaderCode = ReadShaderFile(VERTEX_SHADER_PATH);
      auto fragShaderCode = ReadShaderFile(FRAGMENT_SHADER_PATH);

      VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
      VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

      // Создаем информацию о стадиях шейдеров
      std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {
          {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
           nullptr,
           0,
           VK_SHADER_STAGE_VERTEX_BIT,
           vertShaderModule,
           "main",
           nullptr},
          {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
           nullptr,
           0,
           VK_SHADER_STAGE_FRAGMENT_BIT,
           fragShaderModule,
           "main",
           nullptr}};

      // Конфигурируем пайплайн
      PipelineConfigInfo configInfo{};
      DefaultPipelineConfigInfo(configInfo);
      configInfo.renderPass = renderPass;
      configInfo.pipelineLayout = m_pipelineLayout;
      configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
      configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
      configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;

      // Создаем графический пайплайн
      VkPipeline pipeline = CreateGraphicsPipeline(configInfo, shaderStages);

      // Очищаем шейдерные модули
      vkDestroyShaderModule(m_deviceManager->GetDevice(), vertShaderModule, nullptr);
      vkDestroyShaderModule(m_deviceManager->GetDevice(), fragShaderModule, nullptr);

      // Сохраняем пайплайн
      m_pipelines[name] = pipeline;

      CE_RENDER_DEBUG("Mesh pipeline '", name, "' created successfully");
      return pipeline;
    }
    catch (const std::exception& e)
    {
      CE_RENDER_ERROR("Failed to create mesh pipeline '", name, "': ", e.what());
      return VK_NULL_HANDLE;
    }
  }

  VkPipeline PipelineManager::GetPipeline(const std::string& name) const
  {
    auto it = m_pipelines.find(name);
    if (it != m_pipelines.end())
    {
      return it->second;
    }
    return VK_NULL_HANDLE;
  }

  bool PipelineManager::CreatePipelineLayout()
  {
    // Описания связывания дескрипторов для UBO
    std::array<VkDescriptorSetLayoutBinding, 3> bindings{};

    // SceneUBO (binding 0) - используется в vertex и fragment шейдерах
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    // ModelUBO (binding 1) - используется только в vertex шейдере
    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings[1].pImmutableSamplers = nullptr;

    // LightingUBO (binding 2) - используется в vertex и fragment шейдерах
    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[2].descriptorCount = 1;
    bindings[2].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[2].pImmutableSamplers = nullptr;

    // Создаем layout дескрипторного набора
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VkResult result = vkCreateDescriptorSetLayout(
        m_deviceManager->GetDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout);

    if (result != VK_SUCCESS)
    {
      CE_RENDER_ERROR("Failed to create descriptor set layout");
      return false;
    }

    // Создаем layout пайплайна
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    result = vkCreatePipelineLayout(
        m_deviceManager->GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout);

    VK_CHECK(result, "Failed to create pipeline layout");

    CE_RENDER_DEBUG("Pipeline layout created successfully");
    return true;
  }

  void PipelineManager::DestroyPipelineLayout()
  {
    VkDevice device = m_deviceManager->GetDevice();

    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
      vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
      m_pipelineLayout = VK_NULL_HANDLE;
      CE_RENDER_DEBUG("Pipeline layout destroyed");
    }

    if (m_descriptorSetLayout != VK_NULL_HANDLE)
    {
      vkDestroyDescriptorSetLayout(device, m_descriptorSetLayout, nullptr);
      m_descriptorSetLayout = VK_NULL_HANDLE;
      CE_RENDER_DEBUG("Descriptor set layout destroyed");
    }
  }

  VkPipeline PipelineManager::CreateGraphicsPipeline(
      const PipelineConfigInfo& configInfo,
      const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages)
  {
    // Вершинный input
    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    // Viewport и scissor
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(configInfo.viewportInfo.viewportCount > 0 ? 800 : 0);  // Временное значение
    viewport.height = static_cast<float>(configInfo.viewportInfo.viewportCount > 0 ? 600 : 0);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)};

    VkPipelineViewportStateCreateInfo viewportInfo = configInfo.viewportInfo;
    viewportInfo.pViewports = &viewport;
    viewportInfo.pScissors = &scissor;

    // Собираем информацию для создания пайплайна
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
    pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
    pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
    pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
    pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;
    pipelineInfo.layout = configInfo.pipelineLayout;
    pipelineInfo.renderPass = configInfo.renderPass;
    pipelineInfo.subpass = configInfo.subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    VkPipeline pipeline;
    VkResult result = vkCreateGraphicsPipelines(
        m_deviceManager->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);

    VK_CHECK(result, "Failed to create graphics pipeline");

    return pipeline;
  }

  VkShaderModule PipelineManager::CreateShaderModule(const std::vector<char>& code)
  {
    return VulkanUtils::CreateShaderModule(m_deviceManager->GetDevice(), code);
  }

  std::vector<char> PipelineManager::ReadShaderFile(const std::string& filename)
  {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
      throw std::runtime_error("Failed to open shader file: " + filename);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
  }

  void PipelineManager::DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo)
  {
    // Input Assembly
    configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    // Rasterization
    configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    configInfo.rasterizationInfo.lineWidth = 1.0f;
    configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    configInfo.rasterizationInfo.depthBiasEnable = VK_TRUE;
    configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
    configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
    configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

    // Multisampling
    configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    configInfo.multisampleInfo.minSampleShading = 1.0f;
    configInfo.multisampleInfo.pSampleMask = nullptr;
    configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
    configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;

    // Depth Stencil
    configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.minDepthBounds = 0.0f;
    configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
    configInfo.depthStencilInfo.stencilTestEnable = VK_TRUE;

    // Color Blending
    configInfo.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
    configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    configInfo.colorBlendInfo.attachmentCount = 1;
    configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
    configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

    // Dynamic State
    configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
    configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();

    // Viewport
    configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    configInfo.viewportInfo.viewportCount = 1;
    configInfo.viewportInfo.scissorCount = 1;
  }

  void PipelineManager::EnableAlphaBlending(PipelineConfigInfo& configInfo)
  {
    configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
    configInfo.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
  }
}  // namespace CE