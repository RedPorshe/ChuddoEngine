#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace CE
{
  struct PipelineConfig
  {
    std::string name = "NoName";
    // shaders
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    std::string geometryShaderPath;     // опционально
    std::string tessControlShaderPath;  // опционально
    std::string tessEvalShaderPath;     // опционально

    // Vertex Input
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    // Input Assembly
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkBool32 primitiveRestartEnable = VK_FALSE;

    // Viewport State
    VkViewport viewport{};
    VkRect2D scissor{};
    bool dynamicViewport = false;

    // Rasterization
    VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
    VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
    VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
    float lineWidth = 1.0f;
    VkBool32 depthClampEnable = VK_FALSE;
    VkBool32 rasterizerDiscardEnable = VK_FALSE;
    VkBool32 depthBiasEnable = VK_FALSE;
    float depthBiasConstantFactor = 0.0f;
    float depthBiasClamp = 0.0f;
    float depthBiasSlopeFactor = 0.0f;

    // Multisampling
    VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    VkBool32 sampleShadingEnable = VK_FALSE;
    float minSampleShading = 0.0f;
    VkBool32 alphaToCoverageEnable = VK_FALSE;
    VkBool32 alphaToOneEnable = VK_FALSE;

    // Depth/Stencil
    VkBool32 depthTestEnable = VK_FALSE;
    VkBool32 depthWriteEnable = VK_FALSE;
    VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;
    VkBool32 depthBoundsTestEnable = VK_FALSE;
    VkBool32 stencilTestEnable = VK_FALSE;
    VkStencilOpState frontStencilOp{};
    VkStencilOpState backStencilOp{};
    float minDepthBounds = 0.0f;
    float maxDepthBounds = 1.0f;

    // Color Blending
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    VkBool32 logicOpEnable = VK_FALSE;
    VkLogicOp logicOp = VK_LOGIC_OP_COPY;
    float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    // Dynamic State
    std::vector<VkDynamicState> dynamicStates;

    // Pipeline Layout
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    std::vector<VkPushConstantRange> pushConstantRanges;

    // Render Pass
    VkRenderPass renderPass = VK_NULL_HANDLE;
    uint32_t subpass = 0;

    // Единственный фабричный метод
    static PipelineConfig CreateDefaultConfig();
  };
}  // namespace CE