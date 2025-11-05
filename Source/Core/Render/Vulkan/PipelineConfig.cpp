#include "Core/Render/Vulkan/PipelineConfig.hpp"

namespace CE
{
  PipelineConfig PipelineConfig::CreateDefaultConfig()
  {
    PipelineConfig config;

    config.name = "default";

    // Shaders
    config.vertexShaderPath = "";
    config.fragmentShaderPath = "";
    config.geometryShaderPath = "";
    config.tessControlShaderPath = "";
    config.tessEvalShaderPath = "";

    // Vertex Input
    config.bindingDescriptions.clear();
    config.attributeDescriptions.clear();

    // Input Assembly
    config.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config.primitiveRestartEnable = VK_FALSE;

    // Viewport State
    config.viewport = {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    config.scissor = {{0, 0}, {1, 1}};
    config.dynamicViewport = true;

    // Rasterization
    config.polygonMode = VK_POLYGON_MODE_FILL;
    config.cullMode = VK_CULL_MODE_BACK_BIT;
    config.frontFace = VK_FRONT_FACE_CLOCKWISE;
    config.lineWidth = 1.0f;
    config.depthClampEnable = VK_FALSE;
    config.rasterizerDiscardEnable = VK_FALSE;
    config.depthBiasEnable = VK_FALSE;
    config.depthBiasConstantFactor = 0.0f;
    config.depthBiasClamp = 0.0f;
    config.depthBiasSlopeFactor = 0.0f;

    // Multisampling
    config.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config.sampleShadingEnable = VK_FALSE;
    config.minSampleShading = 0.0f;
    config.alphaToCoverageEnable = VK_FALSE;
    config.alphaToOneEnable = VK_FALSE;

    // Depth/Stencil
    config.depthTestEnable = VK_FALSE;
    config.depthWriteEnable = VK_FALSE;
    config.depthCompareOp = VK_COMPARE_OP_LESS;
    config.depthBoundsTestEnable = VK_FALSE;
    config.stencilTestEnable = VK_FALSE;

    // Color Blending
    VkPipelineColorBlendAttachmentState blendAttachment{};
    blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                     VK_COLOR_COMPONENT_G_BIT |
                                     VK_COLOR_COMPONENT_B_BIT |
                                     VK_COLOR_COMPONENT_A_BIT;
    blendAttachment.blendEnable = VK_FALSE;
    blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    config.colorBlendAttachments = {blendAttachment};
    config.logicOpEnable = VK_FALSE;
    config.logicOp = VK_LOGIC_OP_COPY;

    // Dynamic State
    config.dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    // Pipeline Layout
    config.descriptorSetLayouts.clear();
    config.pushConstantRanges.clear();

    // Render Pass
    config.renderPass = VK_NULL_HANDLE;
    config.subpass = 0;

    return config;
  }
}  // namespace CE
