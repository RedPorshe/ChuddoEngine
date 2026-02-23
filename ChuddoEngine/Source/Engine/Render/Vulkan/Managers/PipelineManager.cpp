#include "Render/Vulkan/Managers/PipelineManager.h"
#include "Core/EngineInfo.h"
#include "Render/Vulkan/Managers/DeviceManager.h"
#include "Render/Vulkan/Managers/RenderPassManager.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <shaderc/shaderc.hpp>

CPipelineManager::CPipelineManager ( FEngineInfo & Info )
	: IVulkanManager ( Info )
	{}

CPipelineManager::~CPipelineManager ()
	{
	Shutdown ();
	LogDebug ( GetManagerName (), " destroyed" );
	}

bool CPipelineManager::Initialize ()
	{
	LogDebug ( "Initializing PipelineManager..." );

	auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	if (!deviceMgr || !deviceMgr->IsInitialized ())
		{
		LogError ( "DeviceManager not initialized" );
		return false;
		}

	LogDebug ( "PipelineManager initialized successfully" );
	m_bInitialized = true;
	return true;
	}

void CPipelineManager::Shutdown ()
	{
	if (!m_bInitialized) return;

	LogDebug ( "Shutting down PipelineManager..." );

	auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	if (deviceMgr)
		{
		VkDevice device = deviceMgr->GetDevice ();

		// Destroy triangle pipeline
		if (m_TrianglePipeline != VK_NULL_HANDLE)
			{
			vkDestroyPipeline ( device, m_TrianglePipeline, nullptr );
			m_TrianglePipeline = VK_NULL_HANDLE;
			}

		// Destroy triangle pipeline layout
		if (m_TrianglePipelineLayout != VK_NULL_HANDLE)
			{
			vkDestroyPipelineLayout ( device, m_TrianglePipelineLayout, nullptr );
			m_TrianglePipelineLayout = VK_NULL_HANDLE;
			}

		// Destroy cached shaders
		for (auto & [name, module] : m_ShaderCache)
			{
			if (module != VK_NULL_HANDLE)
				{				
				vkDestroyShaderModule ( device, module, nullptr );
				}
			}
		m_ShaderCache.clear ();
		}

	m_bInitialized = false;
	LogDebug ( "PipelineManager shutdown complete" );
	}

const char * CPipelineManager::GetManagerName () const
	{
	return "PipelineManager";
	}

// Shader loading
VkShaderModule CPipelineManager::CreateShaderModule ( const std::vector<char> & Code )
	{
	auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	VkDevice device = deviceMgr->GetDevice ();

	VkShaderModuleCreateInfo createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = Code.size ();
	createInfo.pCode = reinterpret_cast< const uint32_t * >( Code.data () );

	VkShaderModule shaderModule;
	VkResult result = vkCreateShaderModule ( device, &createInfo, nullptr, &shaderModule );
	if (result != VK_SUCCESS)
		{
		LogError ( "Failed to create shader module: ", static_cast< int >( result ) );
		return VK_NULL_HANDLE;
		}

	return shaderModule;
	}
  
VkShaderModule CPipelineManager::LoadShader ( const std::string & Filename )
	{
	LogDebug ( "Loading compiled shader: ", Filename );

	// Check cache first
	auto it = m_ShaderCache.find ( Filename );
	if (it != m_ShaderCache.end ())
		{
		return it->second;
		}

	// Read file
	std::ifstream file ( Filename, std::ios::ate | std::ios::binary );
	if (!file.is_open ())
		{
		LogError ( "Failed to open shader file: ", Filename );
		return VK_NULL_HANDLE;
		}

	size_t fileSize = static_cast< size_t >( file.tellg () );
	std::vector<char> buffer ( fileSize );

	file.seekg ( 0 );
	file.read ( buffer.data (), fileSize );
	file.close ();

	// Create shader module
	VkShaderModule module = CreateShaderModule ( buffer );
	if (module != VK_NULL_HANDLE)
		{
		m_ShaderCache[ Filename ] = module;
		LogDebug ( "Loaded compiled shader: ", Filename );
		}

	return module;
	}

FShaderModule CPipelineManager::LoadShaderModule ( const std::string & Filename, VkShaderStageFlagBits Stage )
	{
	FShaderModule result;
	result.Stage = Stage;
	result.EntryPoint = "main";

	// Определяем пути для исходного и скомпилированного файлов
	std::string spvFilename = Filename + ".spv";

	// Проверяем, нужно ли компилировать
	bool needCompile = true;

	// Проверяем существование .spv файла
	std::ifstream spvFile ( spvFilename );
	if (spvFile.good ())
		{
		// .spv существует, проверяем дату изменения исходника
		auto srcTime = std::filesystem::last_write_time ( Filename );
		auto spvTime = std::filesystem::last_write_time ( spvFilename );

		if (spvTime >= srcTime)
			{
			needCompile = false;  // .spv новее или равен исходнику
			}
		spvFile.close ();
		}

	// Компилируем если нужно
	if (needCompile)
		{
		LogDebug ( "Compiling shader: ", Filename );

		std::string command = "glslc \"" + Filename + "\" -o \"" + spvFilename + "\"";
		int compileResult = system ( command.c_str () );

		if (compileResult != 0)
			{
			LogError ( "Failed to compile shader: ", Filename );
			return result;
			}

		LogDebug ( "Shader compiled successfully: ", spvFilename );
		}

	// Загружаем скомпилированный .spv файл
	result.Module = LoadShader ( spvFilename );

	if (result.Module == VK_NULL_HANDLE)
		{
		LogError ( "Failed to load compiled shader: ", spvFilename );
		}

	return result;
	}
// Pipeline layout
VkPipelineLayout CPipelineManager::CreatePipelineLayout (
	const std::vector<VkDescriptorSetLayout> & DescSetLayouts,
	const std::vector<VkPushConstantRange> & PushConstants )
	{
	auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	VkDevice device = deviceMgr->GetDevice ();

	VkPipelineLayoutCreateInfo layoutInfo {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.setLayoutCount = static_cast< uint32_t >( DescSetLayouts.size () );
	layoutInfo.pSetLayouts = DescSetLayouts.data ();
	layoutInfo.pushConstantRangeCount = static_cast< uint32_t >( PushConstants.size () );
	layoutInfo.pPushConstantRanges = PushConstants.data ();

	VkPipelineLayout layout;
	VkResult result = vkCreatePipelineLayout ( device, &layoutInfo, nullptr, &layout );
	if (result != VK_SUCCESS)
		{
		LogError ( "Failed to create pipeline layout: ", static_cast< int >( result ) );
		return VK_NULL_HANDLE;
		}

	return layout;
	}

VkPipeline CPipelineManager::CreateGraphicsPipeline (
	const std::vector<FShaderModule> & Shaders,
	VkPipelineLayout Layout,
	VkRenderPass RenderPass,
	const FGraphicsPipelineConfig & Config,
	uint32_t Subpass )
	{
	if (Shaders.empty ())
		{
		LogError ( "No shaders provided for graphics pipeline" );
		return VK_NULL_HANDLE;
		}

	auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	VkDevice device = deviceMgr->GetDevice ();

	// ПРОВЕРКА 1: Pipeline layout
	if (Layout == VK_NULL_HANDLE)
		{
		LogError ( "Pipeline layout is null" );
		return VK_NULL_HANDLE;
		}
	LogDebug ( "  Pipeline layout is valid" );

	// ПРОВЕРКА 2: Render pass
	if (RenderPass == VK_NULL_HANDLE)
		{
		LogError ( "Render pass is null" );
		return VK_NULL_HANDLE;
		}
	LogDebug ( "  Render pass is valid" );

	// Shader stages
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	for (const auto & shader : Shaders)
		{
		if (!shader.IsValid ())
			{
			LogError ( "Invalid shader module" );
			return VK_NULL_HANDLE;
			}
		shaderStages.push_back ( GetShaderStageInfo ( shader ) );
		}
	LogDebug ( "  Shader stages: ", shaderStages.size () );

	// Проверка шейдерных модулей
	for (size_t i = 0; i < shaderStages.size (); i++)
		{
		if (shaderStages[ i ].module == VK_NULL_HANDLE)
			{
			LogError ( "  Shader stage ", i, " has null module" );
			return VK_NULL_HANDLE;
			}
		LogDebug ( "  Shader stage ", i, " module: ", ( void * ) shaderStages[ i ].module );
		}

	// Fixed function states
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = GetVertexInputState ( Config.VertexInput );
	
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = GetInputAssemblyState ( Config );
	VkPipelineRasterizationStateCreateInfo rasterizer = GetRasterizationState ( Config );
	VkPipelineMultisampleStateCreateInfo multisampling = GetMultisampleState ( Config );
	VkPipelineDepthStencilStateCreateInfo depthStencil = GetDepthStencilState ( Config );

	VkPipelineColorBlendAttachmentState colorBlendAttachment = GetColorBlendAttachment ( Config );
	VkPipelineColorBlendStateCreateInfo colorBlending = GetColorBlendState ( Config );

	VkPipelineDynamicStateCreateInfo dynamicState = GetDynamicState ( Config );

	// Viewport state (dynamic)
	VkPipelineViewportStateCreateInfo viewportState {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	LogDebug ( "  Created all pipeline state structures" );

	VkGraphicsPipelineCreateInfo pipelineInfo {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = static_cast< uint32_t >( shaderStages.size () );
	pipelineInfo.pStages = shaderStages.data ();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = Layout;
	pipelineInfo.renderPass = RenderPass;
	pipelineInfo.subpass = Subpass;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	LogDebug ( "  Created pipelineInfo, calling vkCreateGraphicsPipelines..." );
	LogDebug ( "    pStages: ", ( void * ) pipelineInfo.pStages );
	LogDebug ( "    pVertexInputState: ", ( void * ) pipelineInfo.pVertexInputState );
	LogDebug ( "    pInputAssemblyState: ", ( void * ) pipelineInfo.pInputAssemblyState );
	LogDebug ( "    pViewportState: ", ( void * ) pipelineInfo.pViewportState );
	LogDebug ( "    pRasterizationState: ", ( void * ) pipelineInfo.pRasterizationState );
	LogDebug ( "    pMultisampleState: ", ( void * ) pipelineInfo.pMultisampleState );
	LogDebug ( "    pDepthStencilState: ", ( void * ) pipelineInfo.pDepthStencilState );
	LogDebug ( "    pColorBlendState: ", ( void * ) pipelineInfo.pColorBlendState );
	LogDebug ( "    pDynamicState: ", ( void * ) pipelineInfo.pDynamicState );
	LogDebug ( "    layout: ", ( void * ) pipelineInfo.layout );
	LogDebug ( "    renderPass: ", ( void * ) pipelineInfo.renderPass );

	// Проверка на валидность всех указателей перед вызовом
	if (!pipelineInfo.pStages || !pipelineInfo.pVertexInputState || !pipelineInfo.pInputAssemblyState ||
		 !pipelineInfo.pViewportState || !pipelineInfo.pRasterizationState || !pipelineInfo.pMultisampleState ||
		 !pipelineInfo.pDepthStencilState || !pipelineInfo.pColorBlendState || !pipelineInfo.pDynamicState)
		{
		LogError ( "  One or more pipeline state pointers are null!" );
		return VK_NULL_HANDLE;
		}

	LogDebug ( "Checking shader interface compatibility..." );
	for (const auto & attr : Config.VertexInput.Attributes)
		{
		LogDebug ( "  Input attribute: location=", attr.location,
				   ", format=", attr.format,
				   ", offset=", attr.offset );
		}

	VkPipeline pipeline = VK_NULL_HANDLE;
	
	VkResult result = vkCreateGraphicsPipelines ( device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline );

	if (result != VK_SUCCESS)
		{
		LogError ( "Failed to create graphics pipeline: ", static_cast< int >( result ) );

		// Подробная диагностика ошибки
		switch (result)
			{
				case VK_ERROR_OUT_OF_HOST_MEMORY:
					LogError ( "  Out of host memory" );
					break;
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:
					LogError ( "  Out of device memory" );
					break;
				case VK_ERROR_INVALID_SHADER_NV:
					LogError ( "  Invalid shader - check shader compilation and entry points" );
					break;
				default:
					LogError ( "  Unknown error code" );
					break;
			}
		return VK_NULL_HANDLE;
		}

	LogDebug ( "  Pipeline created successfully: ", ( void * ) pipeline );
	return pipeline;
	}

// Triangle pipeline helpers
   
	VkPipeline CPipelineManager::CreateTrianglePipeline ( VkRenderPass RenderPass )
		{
		LogDebug ( "Creating SIMPLE triangle pipeline..." );
		VkDevice device = VK_NULL_HANDLE;
		if (CDeviceManager * devmng = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () ))
			{
			device = devmng->GetDevice ();
			}
		if (device == VK_NULL_HANDLE)
			{
			LogError ( "Device is null" );
			return VK_NULL_HANDLE;
			}
		// 1. Создаем pipeline layout
		VkPipelineLayout layout = CreatePipelineLayout ();
		if (layout == VK_NULL_HANDLE)
			{
			LogError ( "Failed to create pipeline layout" );
			return VK_NULL_HANDLE;
			}

		// 2. Загружаем шейдеры
		FShaderModule vertShader = LoadShaderModule ( "Assets/Shaders/Mesh.vert", VK_SHADER_STAGE_VERTEX_BIT );
		FShaderModule fragShader = LoadShaderModule ( "Assets/Shaders/Mesh.frag", VK_SHADER_STAGE_FRAGMENT_BIT );

		if (!vertShader.IsValid () || !fragShader.IsValid ())
			{
			LogError ( "Failed to load shaders" );
			vkDestroyPipelineLayout ( device, layout, nullptr );
			return VK_NULL_HANDLE;
			}

		// 3. Vertex input (максимально простой - один атрибут)
		VkVertexInputBindingDescription binding {};
		binding.binding = 0;
		binding.stride = 6 * sizeof ( float );
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription attributes[ 2 ];
		attributes[ 0 ].binding = 0;
		attributes[ 0 ].location = 0;
		attributes[ 0 ].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributes[ 0 ].offset = 0;

		attributes[ 1 ].binding = 0;
		attributes[ 1 ].location = 1;
		attributes[ 1 ].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributes[ 1 ].offset = 3 * sizeof ( float );

		VkPipelineVertexInputStateCreateInfo vertexInput {};
		vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInput.vertexBindingDescriptionCount = 1;
		vertexInput.pVertexBindingDescriptions = &binding;
		vertexInput.vertexAttributeDescriptionCount = 2;
		vertexInput.pVertexAttributeDescriptions = attributes;

		// 4. Input assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		// 5. Viewport state
		VkPipelineViewportStateCreateInfo viewportState {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// 6. Rasterization
		VkPipelineRasterizationStateCreateInfo rasterizer {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.lineWidth = 1.0f;

		// 7. Multisampling
		VkPipelineMultisampleStateCreateInfo multisampling {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// 8. Depth/stencil
		VkPipelineDepthStencilStateCreateInfo depthStencil {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

		// 9. Color blend
		VkPipelineColorBlendAttachmentState colorBlend {};
		colorBlend.colorWriteMask = 0xF;
		colorBlend.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlend;

		// 10. Dynamic states
		VkDynamicState dynamicStates [] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		// 11. Shader stages
		VkPipelineShaderStageCreateInfo shaderStages [] = {
			GetShaderStageInfo ( vertShader ),
			GetShaderStageInfo ( fragShader )
			};

			// 12. Pipeline info
		VkGraphicsPipelineCreateInfo pipelineInfo {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInput;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = layout;
		pipelineInfo.renderPass = RenderPass;

		LogDebug ( "Calling vkCreateGraphicsPipelines..." );

		VkPipeline pipeline;
		VkResult result = vkCreateGraphicsPipelines ( device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline );

		if (result != VK_SUCCESS)
			{
			LogError ( "Failed to create triangle pipeline: ", ( int ) result );
			vkDestroyPipelineLayout ( device, layout, nullptr );
			return VK_NULL_HANDLE;
			}

		LogDebug ( "Triangle pipeline created successfully!" );
		return pipeline;
		}

// State creation helpers
VkPipelineShaderStageCreateInfo CPipelineManager::GetShaderStageInfo ( const FShaderModule & Module ) const
	{
	VkPipelineShaderStageCreateInfo info {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.stage = Module.Stage;
	info.module = Module.Module;
	info.pName = Module.EntryPoint.c_str ();
	return info;
	}

VkPipelineVertexInputStateCreateInfo CPipelineManager::GetVertexInputState ( const FVertexInputDescription & Desc ) const
	{
	LogDebug ( "    VertexInput: ", Desc.Bindings.size (), " bindings, ", Desc.Attributes.size (), " attributes" );

	for (size_t i = 0; i < Desc.Bindings.size (); i++)
		{
		LogDebug ( "      Binding ", i, ": stride=", Desc.Bindings[ i ].stride );
		}

	for (size_t i = 0; i < Desc.Attributes.size (); i++)
		{
		LogDebug ( "      Attribute ", i, ": location=", Desc.Attributes[ i ].location,
				   ", offset=", Desc.Attributes[ i ].offset,
				   ", format=", Desc.Attributes[ i ].format );
		}

	VkPipelineVertexInputStateCreateInfo info {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	info.vertexBindingDescriptionCount = static_cast< uint32_t > ( Desc.Bindings.size () );
	info.pVertexBindingDescriptions = Desc.Bindings.data ();
	info.vertexAttributeDescriptionCount = static_cast< uint32_t >( Desc.Attributes.size () );
	info.pVertexAttributeDescriptions = Desc.Attributes.data ();
	return info;
	}

VkPipelineInputAssemblyStateCreateInfo CPipelineManager::GetInputAssemblyState ( const FGraphicsPipelineConfig & Config ) const
	{
	VkPipelineInputAssemblyStateCreateInfo info {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info.topology = Config.Topology;
	info.primitiveRestartEnable = Config.PrimitiveRestartEnable;
	return info;
	}

VkPipelineRasterizationStateCreateInfo CPipelineManager::GetRasterizationState ( const FGraphicsPipelineConfig & Config ) const
	{
	LogDebug ( "    Rasterization: polygonMode=", Config.PolygonMode,
			   ", cullMode=", Config.CullMode,
			   ", frontFace=", Config.FrontFace );

	VkPipelineRasterizationStateCreateInfo info {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	info.depthClampEnable = VK_FALSE;
	info.rasterizerDiscardEnable = VK_FALSE;
	info.polygonMode = Config.PolygonMode;
	info.lineWidth = Config.LineWidth;
	info.cullMode = Config.CullMode;
	info.frontFace = Config.FrontFace;
	info.depthBiasEnable = Config.DepthBiasEnable;
	info.depthBiasConstantFactor = Config.DepthBiasConstantFactor;
	info.depthBiasClamp = Config.DepthBiasClamp;
	info.depthBiasSlopeFactor = Config.DepthBiasSlopeFactor;
	return info;
	}

VkPipelineMultisampleStateCreateInfo CPipelineManager::GetMultisampleState ( const FGraphicsPipelineConfig & Config ) const
	{
	VkPipelineMultisampleStateCreateInfo info {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	info.sampleShadingEnable = Config.SampleShadingEnable;
	info.rasterizationSamples = Config.Samples;
	info.minSampleShading = Config.MinSampleShading;
	info.pSampleMask = nullptr;
	info.alphaToCoverageEnable = VK_FALSE;
	info.alphaToOneEnable = VK_FALSE;
	return info;
	}

VkPipelineDepthStencilStateCreateInfo CPipelineManager::GetDepthStencilState ( const FGraphicsPipelineConfig & Config ) const
	{
	LogDebug ( "    DepthStencil: test=", Config.DepthTestEnable,
			   ", write=", Config.DepthWriteEnable,
			   ", compareOp=", Config.DepthCompareOp );

	VkPipelineDepthStencilStateCreateInfo info {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	info.depthTestEnable = Config.DepthTestEnable;
	info.depthWriteEnable = Config.DepthWriteEnable;
	info.depthCompareOp = Config.DepthCompareOp;
	info.depthBoundsTestEnable = Config.DepthBoundsTestEnable;
	info.stencilTestEnable = Config.StencilTestEnable;
	info.front = {};
	info.back = {};
	info.minDepthBounds = 0.0f;
	info.maxDepthBounds = 1.0f;
	return info;
	}

VkPipelineColorBlendAttachmentState CPipelineManager::GetColorBlendAttachment ( const FGraphicsPipelineConfig & Config ) const
	{
	VkPipelineColorBlendAttachmentState attachment {};
	attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	attachment.blendEnable = Config.BlendEnable;
	attachment.srcColorBlendFactor = Config.SrcColorBlendFactor;
	attachment.dstColorBlendFactor = Config.DstColorBlendFactor;
	attachment.colorBlendOp = Config.ColorBlendOp;
	attachment.srcAlphaBlendFactor = Config.SrcAlphaBlendFactor;
	attachment.dstAlphaBlendFactor = Config.DstAlphaBlendFactor;
	attachment.alphaBlendOp = Config.AlphaBlendOp;
	return attachment;
	}

VkPipelineColorBlendStateCreateInfo CPipelineManager::GetColorBlendState ( const FGraphicsPipelineConfig & Config ) const
	{
	VkPipelineColorBlendAttachmentState attachment = GetColorBlendAttachment ( Config );

	VkPipelineColorBlendStateCreateInfo info {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	info.logicOpEnable = VK_FALSE;
	info.logicOp = VK_LOGIC_OP_COPY;
	info.attachmentCount = 1;
	info.pAttachments = &attachment;
	info.blendConstants[ 0 ] = 0.0f;
	info.blendConstants[ 1 ] = 0.0f;
	info.blendConstants[ 2 ] = 0.0f;
	info.blendConstants[ 3 ] = 0.0f;
	return info;
	}

VkPipelineDynamicStateCreateInfo CPipelineManager::GetDynamicState ( const FGraphicsPipelineConfig & Config ) const
	{
	VkPipelineDynamicStateCreateInfo info {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

	std::vector<VkDynamicState> dynamicStates = Config.DynamicStates;
	if (dynamicStates.empty ())
		{
		dynamicStates = GetDefaultDynamicStates ();
		}

	info.dynamicStateCount = static_cast< uint32_t >( dynamicStates.size () );
	info.pDynamicStates = dynamicStates.data ();
	return info;
	}

std::vector<VkDynamicState> CPipelineManager::GetDefaultDynamicStates () const
	{
	return { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	}

// Vertex input for triangle (position + color)
FVertexInputDescription CPipelineManager::GetTriangleVertexInput () const
	{
	FVertexInputDescription desc;

	// Binding description
	VkVertexInputBindingDescription binding {};
	binding.binding = 0;
	binding.stride = 6 * sizeof ( float );  // position: vec2, color: vec3 = 5 floats
	binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	desc.Bindings.push_back ( binding );

	// Attribute descriptions - ИСПРАВЛЕНО!
	VkVertexInputAttributeDescription positionAttr {};
	positionAttr.binding = 0;
	positionAttr.location = 0;
	positionAttr.format = VK_FORMAT_R32G32B32_SFLOAT;  // Было R32G32B32_SFLOAT, но stride не совпадает!
	positionAttr.offset = 0;
	desc.Attributes.push_back ( positionAttr );

	VkVertexInputAttributeDescription colorAttr {};
	colorAttr.binding = 0;
	colorAttr.location = 1;
	colorAttr.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorAttr.offset = 3 * sizeof ( float );  // после vec2 (2 floats)
	desc.Attributes.push_back ( colorAttr );

	return desc;
	}

VkPipeline CPipelineManager::CreatePipeline (
	VkRenderPass RenderPass,
	const std::string & VertShaderPath,
	const std::string & FragShaderPath,
	const FVertexInputDescription & VertexInput,
	const FGraphicsPipelineConfig & Config )
	{
	LogDebug ( "Creating pipeline with shaders: ", VertShaderPath, ", ", FragShaderPath );

	// Create pipeline layout
	VkPipelineLayout layout = CreatePipelineLayout ();
	if (layout == VK_NULL_HANDLE)
		{
		LogError ( "Failed to create pipeline layout" );
		return VK_NULL_HANDLE;
		}

	// Load shaders
	FShaderModule vertShader = LoadShaderModule ( VertShaderPath, VK_SHADER_STAGE_VERTEX_BIT );
	FShaderModule fragShader = LoadShaderModule ( FragShaderPath, VK_SHADER_STAGE_FRAGMENT_BIT );
	CDeviceManager * devicManager = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	auto device = devicManager->GetDevice ();

	if (!vertShader.IsValid () || !fragShader.IsValid ())
		{
		LogError ( "Failed to load shaders" );
		vkDestroyPipelineLayout ( device, layout, nullptr );
		return VK_NULL_HANDLE;
		}

	// Create pipeline
	std::vector<FShaderModule> shaders = { vertShader, fragShader };
	VkPipeline pipeline = CreateGraphicsPipeline ( shaders, layout, RenderPass, Config );

	if (pipeline == VK_NULL_HANDLE)
		{
		LogError ( "Failed to create pipeline" );
		vkDestroyPipelineLayout ( device, layout, nullptr );
		return VK_NULL_HANDLE;
		}

	LogDebug ( "Pipeline created successfully" );
	return pipeline;
	}
	// Cleanup
void CPipelineManager::DestroyShaderModule ( VkShaderModule Module )
	{
	if (Module == VK_NULL_HANDLE) return;

	auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	VkDevice device = deviceMgr->GetDevice ();

	vkDestroyShaderModule ( device, Module, nullptr );
	}

void CPipelineManager::DestroyPipelineLayout ( VkPipelineLayout Layout )
	{
	if (Layout == VK_NULL_HANDLE) return;

	auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	VkDevice device = deviceMgr->GetDevice ();

	vkDestroyPipelineLayout ( device, Layout, nullptr );
	}

void CPipelineManager::DestroyPipeline ( VkPipeline Pipeline )
	{
	if (Pipeline == VK_NULL_HANDLE) return;

	auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	VkDevice device = deviceMgr->GetDevice ();

	vkDestroyPipeline ( device, Pipeline, nullptr );
	}


VkPipeline CPipelineManager::CreateMinimalPipeline ( VkRenderPass RenderPass )
	{
	LogDebug ( "Creating SIMPLE pipeline..." );

	// 1. Сначала создаем pipeline layout
	VkPipelineLayoutCreateInfo layoutInfo {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.setLayoutCount = 0;
	layoutInfo.pushConstantRangeCount = 0;

	VkPipelineLayout layout;
	CDeviceManager * devicemanager = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get() );
	VkResult result = vkCreatePipelineLayout ( devicemanager->GetDevice (), &layoutInfo, nullptr, &layout );
	if (result != VK_SUCCESS)
		{
		LogError ( "Failed to create pipeline layout: ", result );
		return VK_NULL_HANDLE;
		}

	// 2. Загружаем шейдеры
	VkShaderModule vertModule = LoadShaderModule ( "Assets/Shaders/Mesh.vert",VK_SHADER_STAGE_VERTEX_BIT ).Module;
	VkShaderModule fragModule = LoadShaderModule ( "Assets/Shaders/Mesh.frag", VK_SHADER_STAGE_FRAGMENT_BIT ).Module;

	if (vertModule == VK_NULL_HANDLE || fragModule == VK_NULL_HANDLE)
		{
		LogError ( "Failed to load shaders" );
		vkDestroyPipelineLayout ( devicemanager->GetDevice (), layout, nullptr );
		return VK_NULL_HANDLE;
		}

	// 3. Shader stages
	VkPipelineShaderStageCreateInfo vertStage {};
	vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStage.module = vertModule;
	vertStage.pName = "main";

	VkPipelineShaderStageCreateInfo fragStage {};
	fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStage.module = fragModule;
	fragStage.pName = "main";

	VkPipelineShaderStageCreateInfo stages [] = { vertStage, fragStage };

	// 4. Vertex input state - ПУСТОЙ!
	VkPipelineVertexInputStateCreateInfo vertexInput {};
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	// 5. Input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	// 6. Viewport state (dynamic)
	VkPipelineViewportStateCreateInfo viewportState {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	// 7. Rasterization state - МАКСИМАЛЬНО ПРОСТОЙ
	VkPipelineRasterizationStateCreateInfo rasterizer {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_NONE;  // NO CULLING!
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.lineWidth = 1.0f;

	// 8. Multisampling - отключаем
	VkPipelineMultisampleStateCreateInfo multisampling {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// 9. Depth/stencil - отключаем
	VkPipelineDepthStencilStateCreateInfo depthStencil {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_FALSE;
	depthStencil.depthWriteEnable = VK_FALSE;

	// 10. Color blend
	VkPipelineColorBlendAttachmentState colorBlend {};
	colorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorBlending {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlend;

	// 11. Dynamic states
	VkDynamicState dynamicStates [] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicState {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	// 12. Финальный pipeline info
	VkGraphicsPipelineCreateInfo pipelineInfo {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = stages;
	pipelineInfo.pVertexInputState = &vertexInput;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = layout;
	pipelineInfo.renderPass = RenderPass;
	pipelineInfo.subpass = 0;

	LogDebug ( "Calling vkCreateGraphicsPipelines with SIMPLE config..." );

	VkPipeline pipeline;
	result = vkCreateGraphicsPipelines ( devicemanager->GetDevice (), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline );

//cleanup shadermodules in destructor (or delete clean shader modules in destructor)

	if (result != VK_SUCCESS)
		{
		LogError ( "Failed to create simple pipeline: ", result );
		vkDestroyPipelineLayout ( devicemanager->GetDevice (), layout, nullptr );
		return VK_NULL_HANDLE;
		}

	LogDebug ( "SIMPLE pipeline created successfully!" );
	return pipeline;
	}