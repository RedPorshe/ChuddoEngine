#pragma once
#include "Render/Vulkan/VulkanInterface.h"
#include <vector>
#include <string>
#include <unordered_map>

struct FEngineInfo;

struct FShaderModule
    {
    VkShaderModule Module = VK_NULL_HANDLE;
    VkShaderStageFlagBits Stage;
    std::string EntryPoint = "main";

    bool IsValid () const { return Module != VK_NULL_HANDLE; }
    };

struct FVertexInputDescription
    {
    std::vector<VkVertexInputBindingDescription> Bindings;
    std::vector<VkVertexInputAttributeDescription> Attributes;
    };

struct FGraphicsPipelineConfig
    {
    // Vertex input
    FVertexInputDescription VertexInput;

    // Input assembly
    VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkBool32 PrimitiveRestartEnable = VK_FALSE;

    // Rasterizer
    VkPolygonMode PolygonMode = VK_POLYGON_MODE_FILL;
    float LineWidth = 1.0f;
    VkCullModeFlags CullMode = VK_CULL_MODE_BACK_BIT;
    VkFrontFace FrontFace = VK_FRONT_FACE_CLOCKWISE;
    VkBool32 DepthBiasEnable = VK_FALSE;
    float DepthBiasConstantFactor = 0.0f;
    float DepthBiasClamp = 0.0f;
    float DepthBiasSlopeFactor = 0.0f;

    // Multisampling
    VkSampleCountFlagBits Samples = VK_SAMPLE_COUNT_1_BIT;
    VkBool32 SampleShadingEnable = VK_FALSE;
    float MinSampleShading = 1.0f;

    // Depth/stencil
    VkBool32 DepthTestEnable = VK_TRUE;
    VkBool32 DepthWriteEnable = VK_TRUE;
    VkCompareOp DepthCompareOp = VK_COMPARE_OP_LESS;
    VkBool32 DepthBoundsTestEnable = VK_FALSE;
    VkBool32 StencilTestEnable = VK_FALSE;

    // Color blend
    VkBool32 BlendEnable = VK_FALSE;
    VkBlendFactor SrcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    VkBlendFactor DstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    VkBlendOp ColorBlendOp = VK_BLEND_OP_ADD;
    VkBlendFactor SrcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    VkBlendFactor DstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    VkBlendOp AlphaBlendOp = VK_BLEND_OP_ADD;

    // Dynamic states
    std::vector<VkDynamicState> DynamicStates;
    };

class CPipelineManager final : public IVulkanManager
    {
    public:
        CPipelineManager ( FEngineInfo & Info );
        virtual ~CPipelineManager ();

        // IVulkanManager
        bool Initialize () override;
        void Shutdown () override;
        const char * GetManagerName () const override;

        // Shader loading
        VkShaderModule CreateShaderModule ( const std::vector<char> & Code );
        VkShaderModule LoadShader ( const std::string & Filename );
        FShaderModule LoadShaderModule ( const std::string & Filename, VkShaderStageFlagBits Stage );

        // Pipeline layout
        VkPipelineLayout CreatePipelineLayout (
            const std::vector<VkDescriptorSetLayout> & DescSetLayouts = {},
            const std::vector<VkPushConstantRange> & PushConstants = {} );

        // Graphics pipeline
        VkPipeline CreateGraphicsPipeline (
            const std::vector<FShaderModule> & Shaders,
            VkPipelineLayout Layout,
            VkRenderPass RenderPass,
            const FGraphicsPipelineConfig & Config = FGraphicsPipelineConfig (),
            uint32_t Subpass = 0 );

        // Triangle pipeline helpers
        VkPipeline CreateTrianglePipeline ( VkRenderPass RenderPass );
        VkPipelineLayout GetTrianglePipelineLayout () const { return m_TrianglePipelineLayout; }

        // Cleanup
        void DestroyShaderModule ( VkShaderModule Module );
        void DestroyPipelineLayout ( VkPipelineLayout Layout );
        void DestroyPipeline ( VkPipeline Pipeline );
        VkPipeline CreateMinimalPipeline ( VkRenderPass RenderPass );
    private:
        std::vector<VkDynamicState> GetDefaultDynamicStates () const;
        VkPipelineShaderStageCreateInfo GetShaderStageInfo ( const FShaderModule & Module ) const;
        VkPipelineVertexInputStateCreateInfo GetVertexInputState ( const FVertexInputDescription & Desc ) const;
        VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyState ( const FGraphicsPipelineConfig & Config ) const;
        VkPipelineRasterizationStateCreateInfo GetRasterizationState ( const FGraphicsPipelineConfig & Config ) const;
        VkPipelineMultisampleStateCreateInfo GetMultisampleState ( const FGraphicsPipelineConfig & Config ) const;
        VkPipelineDepthStencilStateCreateInfo GetDepthStencilState ( const FGraphicsPipelineConfig & Config ) const;
        VkPipelineColorBlendAttachmentState GetColorBlendAttachment ( const FGraphicsPipelineConfig & Config ) const;
        VkPipelineColorBlendStateCreateInfo GetColorBlendState ( const FGraphicsPipelineConfig & Config ) const;
        VkPipelineDynamicStateCreateInfo GetDynamicState ( const FGraphicsPipelineConfig & Config ) const;
        VkPipeline CreatePipeline (
            VkRenderPass RenderPass,
            const std::string & VertShaderPath,
            const std::string & FragShaderPath,
            const FVertexInputDescription & VertexInput,
            const FGraphicsPipelineConfig & Config );
            
    private:
        // Default vertex input for triangle (position + color)
        FVertexInputDescription GetTriangleVertexInput () const;

        // Cache for loaded shaders
        std::unordered_map<std::string, VkShaderModule> m_ShaderCache;

        // Triangle pipeline resources (for testing)
        VkPipelineLayout m_TrianglePipelineLayout = VK_NULL_HANDLE;
        VkPipeline m_TrianglePipeline = VK_NULL_HANDLE;
    };