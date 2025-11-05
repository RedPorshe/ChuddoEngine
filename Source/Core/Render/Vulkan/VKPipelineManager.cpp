#include "Core/Render/Vulkan/VKPipelineManager.hpp"

#include "Core/Render/Vulkan/PipelineConfig.hpp"
#include "Core/Render/Vulkan/VKContext.hpp"
#include "Core/Render/vertex.hpp"

namespace CE
{
  PipelineManager::PipelineManager(VKContext& context) : m_context{context}
  {
    if (m_context.RenderPass == VK_NULL_HANDLE)
    {
      throw std::runtime_error("RenderPass must be created before PipelineManager!");
    }
    CreatetrianglePipeline();
    CreateMeshPipeline();

    if (GetPipeline("triangle") == VK_NULL_HANDLE ||
        GetPipeline("mesh") == VK_NULL_HANDLE)
    {
      throw std::runtime_error("Failed to create pipelines!");
    }
  }

  PipelineManager::~PipelineManager()
  {
  }

  void PipelineManager::CreatePipeline(const std::string& name, const PipelineConfig& config)
  {
    if (config.renderPass == VK_NULL_HANDLE)
      return;
    // Загрузка шейдеров
    auto vertShaderCode = ReadFile(config.vertexShaderPath);
    auto fragShaderCode = ReadFile(config.fragmentShaderPath);

    if (vertShaderCode.empty() || fragShaderCode.empty())
    {
      return;
    }

    VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

    // Настройка стадий шейдеров
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(config.bindingDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = config.bindingDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(config.attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = config.attributeDescriptions.data();

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = config.topology;
    inputAssembly.primitiveRestartEnable = config.primitiveRestartEnable;

    // Viewport state
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &config.viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &config.scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = config.depthClampEnable;
    rasterizer.rasterizerDiscardEnable = config.rasterizerDiscardEnable;
    rasterizer.polygonMode = config.polygonMode;
    rasterizer.lineWidth = config.lineWidth;
    rasterizer.cullMode = config.cullMode;
    rasterizer.frontFace = config.frontFace;
    rasterizer.depthBiasEnable = config.depthBiasEnable;
    rasterizer.depthBiasConstantFactor = config.depthBiasConstantFactor;
    rasterizer.depthBiasClamp = config.depthBiasClamp;
    rasterizer.depthBiasSlopeFactor = config.depthBiasSlopeFactor;

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = config.sampleShadingEnable;
    multisampling.rasterizationSamples = config.rasterizationSamples;
    multisampling.minSampleShading = config.minSampleShading;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = config.alphaToCoverageEnable;
    multisampling.alphaToOneEnable = config.alphaToOneEnable;

    // Depth stencil
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = config.depthTestEnable;
    depthStencil.depthWriteEnable = config.depthWriteEnable;
    depthStencil.depthCompareOp = config.depthCompareOp;
    depthStencil.depthBoundsTestEnable = config.depthBoundsTestEnable;
    depthStencil.stencilTestEnable = config.stencilTestEnable;
    depthStencil.front = config.frontStencilOp;
    depthStencil.back = config.backStencilOp;
    depthStencil.minDepthBounds = config.minDepthBounds;
    depthStencil.maxDepthBounds = config.maxDepthBounds;

    // Color blending
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = config.logicOpEnable;
    colorBlending.logicOp = config.logicOp;
    colorBlending.attachmentCount = static_cast<uint32_t>(config.colorBlendAttachments.size());
    colorBlending.pAttachments = config.colorBlendAttachments.data();
    colorBlending.blendConstants[0] = config.blendConstants[0];
    colorBlending.blendConstants[1] = config.blendConstants[1];
    colorBlending.blendConstants[2] = config.blendConstants[2];
    colorBlending.blendConstants[3] = config.blendConstants[3];

    // Dynamic state
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(config.dynamicStates.size());
    dynamicState.pDynamicStates = config.dynamicStates.data();

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(config.descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = config.descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(config.pushConstantRanges.size());
    pipelineLayoutInfo.pPushConstantRanges = config.pushConstantRanges.data();

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    if (vkCreatePipelineLayout(m_context.Device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
      vkDestroyShaderModule(m_context.Device, fragShaderModule, nullptr);
      vkDestroyShaderModule(m_context.Device, vertShaderModule, nullptr);
      return;
    }

    // Graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = config.renderPass;
    pipelineInfo.subpass = config.subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(m_context.Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
    {
      vkDestroyShaderModule(m_context.Device, fragShaderModule, nullptr);
      vkDestroyShaderModule(m_context.Device, vertShaderModule, nullptr);
      vkDestroyPipelineLayout(m_context.Device, pipelineLayout, nullptr);
      return;
    }

    // save pipeline and layout
    m_context.Pipelines[name] = pipeline;
    m_context.PipelineLayouts[name] = pipelineLayout;

    // clear shader modules
    vkDestroyShaderModule(m_context.Device, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_context.Device, vertShaderModule, nullptr);
  }

  void PipelineManager::CreatetrianglePipeline()
  {
    auto config = PipelineConfig::CreateDefaultConfig();
    config.name = "triangle";
    config.vertexShaderPath = "Assets/Shaders/basic_vert.spv";
    config.fragmentShaderPath = "Assets/Shaders/basic_frag.spv";
    config.viewport = {0, 0, static_cast<float>(m_context.swapchainExtent.width), static_cast<float>(m_context.swapchainExtent.height), 0, 1};
    config.scissor = {{0, 0}, {m_context.swapchainExtent.width, m_context.swapchainExtent.height}};
    config.renderPass = m_context.RenderPass;

    if (m_descriptorSetLayout != VK_NULL_HANDLE)
    {
      config.descriptorSetLayouts.push_back(m_descriptorSetLayout);
    }
    else
    {
      return;
    }

    // ВАЖНО: Устанавливаем правильные vertex attributes
    auto bindingDesc = Vertex::GetBindingDescription();
    auto attrDescs = Vertex::GetAttributeDescriptions();

    config.bindingDescriptions.push_back(bindingDesc);
    for (const auto& attr : attrDescs)
    {
      config.attributeDescriptions.push_back(attr);
    }

    config.cullMode = VK_CULL_MODE_BACK_BIT;
    config.frontFace = VK_FRONT_FACE_CLOCKWISE;

    CreatePipeline(config.name, config);
  }

  std::vector<char> PipelineManager::ReadFile(const std::string& filename)
  {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
      return {};
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
  }

  VkShaderModule PipelineManager::CreateShaderModule(const std::vector<char>& code)
  {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_context.Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create shader module!");
    }

    return shaderModule;
  }

  VkPipeline PipelineManager::GetPipeline(const std::string& name) const
  {
    auto it = m_context.Pipelines.find(name);
    return it != m_context.Pipelines.end() ? it->second : VK_NULL_HANDLE;
  }

  void PipelineManager::CreateMeshPipeline()
  {
    PipelineConfig config = PipelineConfig::CreateDefaultConfig();
    config.name = "mesh";

    config.vertexShaderPath = "Assets/Shaders/mesh_vert.spv";
    config.fragmentShaderPath = "Assets/Shaders/mesh_frag.spv";
    config.depthTestEnable = VK_TRUE;
    config.depthWriteEnable = VK_TRUE;
    config.depthCompareOp = VK_COMPARE_OP_LESS;
    config.renderPass = m_context.RenderPass;
    config.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    config.cullMode = VK_CULL_MODE_NONE;  // TODO: Fix cull mode...

    if (m_descriptorSetLayout != VK_NULL_HANDLE)
    {
      config.descriptorSetLayouts.push_back(m_descriptorSetLayout);
    }
    else
    {
      return;
    }

    config.viewport = {0, 0,
                       static_cast<float>(m_context.swapchainExtent.width),
                       static_cast<float>(m_context.swapchainExtent.height), 0, 1};
    config.scissor = {{0, 0}, m_context.swapchainExtent};

    auto bindingDesc = Vertex::GetBindingDescription();
    auto attrDescs = Vertex::GetAttributeDescriptions();

    config.bindingDescriptions.push_back(bindingDesc);
    for (const auto& attr : attrDescs)
    {
      config.attributeDescriptions.push_back(attr);
    }
    CreatePipeline("mesh", config);
  }

  VkPipelineLayout PipelineManager::GetPipelineLayout(const std::string& name) const
  {
    auto it = m_context.PipelineLayouts.find(name);
    return it != m_context.PipelineLayouts.end() ? it->second : VK_NULL_HANDLE;
  }

  void PipelineManager::Destroy()
  {
    for (auto& [name, pipeline] : m_context.Pipelines)
    {
      vkDestroyPipeline(m_context.Device, pipeline, nullptr);
    }
    for (auto& [name, layout] : m_context.PipelineLayouts)
    {
      vkDestroyPipelineLayout(m_context.Device, layout, nullptr);
    }
    m_context.Pipelines.clear();
    m_context.PipelineLayouts.clear();
  }
}  // namespace CE