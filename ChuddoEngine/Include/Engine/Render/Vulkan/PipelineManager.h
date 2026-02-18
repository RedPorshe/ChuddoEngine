#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "Render/Vulkan/VulkanContext.h"

struct PipelineConfig
	{
	std::string Name;
	std::string VertexShaderPath;
	std::string FragmentShaderPath;
	// Add more fields as needed (e.g., input layout, rasterizer state, etc.)

	// Vertex input descriptions
	std::vector<VkVertexInputBindingDescription> Bindings;
	std::vector<VkVertexInputAttributeDescription> Attributes;

	// Fixed pipeline states that can be customized per-pipeline
	VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
	VkPipelineRasterizationStateCreateInfo Rasterization{};
	VkPipelineMultisampleStateCreateInfo Multisample{};
	VkPipelineDepthStencilStateCreateInfo DepthStencil{};
	VkPipelineColorBlendAttachmentState ColorBlendAttachment{};
	VkPipelineColorBlendStateCreateInfo ColorBlendState{};

	// Dynamic states (viewport, scissor, etc.)
	std::vector<VkDynamicState> DynamicStates;
	};

class PipelineManager
	{
	public:
		PipelineManager ( VulkanContextInfo & contextInfo );
		virtual ~PipelineManager ();
		bool Initialize ();
		VkPipeline GetPipeline ( const std::string & PipelineName = "Mesh" );

	private:
		VulkanContextInfo & ContextInfo ;
		std::unordered_map<std::string, VkPipeline> Pipelines;
		bool CreatePipeline ( const std::string & PipelineName );
		VkPipeline CreateGraphicsPipeline ( const PipelineConfig & config );
		PipelineConfig CreateDefaultPipelineConfig () const;
		
		std::string MeshVertexPath {};
		std::string MeshFragmentPath {};

	};