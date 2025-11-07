#include "Rendering/Vulkan/PipelineManager.h"

#include <fstream>
#include <stdexcept>

namespace CE
{
  PipelineManager::PipelineManager(std::shared_ptr<DeviceManager> deviceManager, std::shared_ptr<SwapchainManager> swapchainManager)
      : m_deviceManager(deviceManager), m_swapchainManager(swapchainManager)
  {
  }

  PipelineManager::~PipelineManager()
  {
    Cleanup();
  }

  bool PipelineManager::Initialize()
  {
    CE_RENDER_DEBUG("Initializing PipelineManager...");

    try
    {
      CreateDescriptorSetLayout();
      CreateGraphicsPipeline();

      CE_RENDER_DEBUG("PipelineManager initialized successfully");
      return true;
    }
    catch (const std::exception& e)
    {
      CE_RENDER_ERROR("Failed to initialize PipelineManager: %s", e.what());
      return false;
    }
  }

  void PipelineManager::Cleanup()
  {
    VkDevice device = m_deviceManager->GetDevice();

    if (m_graphicsPipeline != VK_NULL_HANDLE)
    {
      vkDestroyPipeline(device, m_graphicsPipeline, nullptr);
      m_graphicsPipeline = VK_NULL_HANDLE;
    }

    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
      vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
      m_pipelineLayout = VK_NULL_HANDLE;
    }

    if (m_descriptorSetLayout != VK_NULL_HANDLE)
    {
      vkDestroyDescriptorSetLayout(device, m_descriptorSetLayout, nullptr);
      m_descriptorSetLayout = VK_NULL_HANDLE;
    }

    CE_RENDER_DEBUG("PipelineManager cleaned up");
  }

  void PipelineManager::CreateDescriptorSetLayout()
  {
    // Scene UBO (binding 0)
    VkDescriptorSetLayoutBinding sceneUboLayoutBinding{};
    sceneUboLayoutBinding.binding = 0;
    sceneUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    sceneUboLayoutBinding.descriptorCount = 1;
    sceneUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    sceneUboLayoutBinding.pImmutableSamplers = nullptr;

    // Model UBO (binding 1) - динамический uniform buffer для отдельных моделей
    VkDescriptorSetLayoutBinding modelUboLayoutBinding{};
    modelUboLayoutBinding.binding = 1;
    modelUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    modelUboLayoutBinding.descriptorCount = 1;
    modelUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    modelUboLayoutBinding.pImmutableSamplers = nullptr;

    // Lighting UBO (binding 2)
    VkDescriptorSetLayoutBinding lightingUboLayoutBinding{};
    lightingUboLayoutBinding.binding = 2;
    lightingUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightingUboLayoutBinding.descriptorCount = 1;
    lightingUboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    lightingUboLayoutBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 3> bindings = {
        sceneUboLayoutBinding,
        modelUboLayoutBinding,
        lightingUboLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VkResult result = vkCreateDescriptorSetLayout(m_deviceManager->GetDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout);
    VK_CHECK(result, "Failed to create descriptor set layout!");

    CE_RENDER_DEBUG("Descriptor set layout created");
  }

  void PipelineManager::CreateGraphicsPipeline()
  {
    // Загружаем шейдеры (пока заглушки - нужно будет заменить на реальные шейдеры)
    std::vector<char> vertShaderCode;  // = LoadShader("shaders/vert.spv");
    std::vector<char> fragShaderCode;  // = LoadShader("shaders/frag.spv");

    // Создаем временные простые шейдеры для компиляции
    // В реальном коде нужно загружать скомпилированные SPIR-V шейдеры
    VkShaderModule vertShaderModule = VK_NULL_HANDLE;
    VkShaderModule fragShaderModule = VK_NULL_HANDLE;

    // Pipeline stages
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
    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewport and scissor
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapchainManager->GetExtent().width;
    viewport.height = (float)m_swapchainManager->GetExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapchainManager->GetExtent();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Depth stencil
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    // Color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    VkResult result = vkCreatePipelineLayout(m_deviceManager->GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
    VK_CHECK(result, "Failed to create pipeline layout!");

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
    pipelineInfo.pDynamicState = nullptr;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_swapchainManager->GetRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    result = vkCreateGraphicsPipelines(m_deviceManager->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline);
    VK_CHECK(result, "Failed to create graphics pipeline!");

    // Cleanup shader modules if they were created
    if (vertShaderModule != VK_NULL_HANDLE)
    {
      vkDestroyShaderModule(m_deviceManager->GetDevice(), vertShaderModule, nullptr);
    }
    if (fragShaderModule != VK_NULL_HANDLE)
    {
      vkDestroyShaderModule(m_deviceManager->GetDevice(), fragShaderModule, nullptr);
    }

    CE_RENDER_DEBUG("Graphics pipeline created");
  }

  VkShaderModule PipelineManager::CreateShaderModule(const std::vector<char>& code)
  {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    VkResult result = vkCreateShaderModule(m_deviceManager->GetDevice(), &createInfo, nullptr, &shaderModule);
    VK_CHECK(result, "Failed to create shader module!");

    return shaderModule;
  }
}  // namespace CE