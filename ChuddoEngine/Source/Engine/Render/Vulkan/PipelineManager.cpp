#include "Render/Vulkan/PipelineManager.h"
#include "Render/Mesh.h"

#include <vector>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <windows.h>
#include <cstring>
#include <cstdlib>

namespace
	{
	static std::vector<char> ReadFile ( const std::string & filename )
		{
		std::ifstream file ( filename, std::ios::ate | std::ios::binary );
		if (!file.is_open ())
			{
			throw std::runtime_error ( "failed to open file: " + filename );
			}
		size_t fileSize = static_cast< size_t > ( file.tellg () );
		std::vector<char> buffer ( fileSize );
		file.seekg ( 0 );
		file.read ( buffer.data (), fileSize );
		file.close ();
		return buffer;
		}

static bool EnsureSpvForSource ( const std::string & srcPath, std::string & outSpv )
	{
	namespace fs = std::filesystem;
	if (fs::exists ( srcPath ) && srcPath.size () > 4)
		{
		std::string ext = srcPath.substr ( srcPath.find_last_of ( '.' ) + 1 );
		if (ext == "spv")
			{
			outSpv = srcPath;
			return true;
			}

		// If GLSL source exists, attempt to compile to SPV using glslangValidator if available
        fs::path srcP = srcPath;
		// Place compiled SPV next to the source (e.g. Assets/Shaders) and name by stage to avoid collisions
		fs::path outDir = srcP.parent_path();
		if (outDir.empty()) outDir = "shaders";
		if (!fs::exists ( outDir ))
			fs::create_directories ( outDir );

		// Create distinct output name: <stem>_<ext>.spv -> Mesh_vert.spv, Mesh_frag.spv
		std::string outName = srcP.stem ().string () + "_" + ext + ".spv";
		fs::path outP = outDir / outName;
		outSpv = outP.string ();

		if (fs::exists ( outP ))
			return true;

        // Specify shader stage explicitly so SPV entry point/stage is correct
		std::string stageFlag;
		if (ext == "vert") stageFlag = "-S vert";
		else if (ext == "frag") stageFlag = "-S frag";
		else stageFlag = "";

        // Force output SPV next to executable folder (bin/Debug) so runtime loads predictable location
		fs::path exePath;
		{
			// Try to get folder of current executable from environment variable (__argc/__argv not available here), fallback to src parent
			char buf[MAX_PATH];
			if (GetModuleFileNameA(NULL, buf, MAX_PATH) > 0)
			{
				exePath = fs::path(buf).parent_path();
			}
			else
			{
				exePath = outDir; // fallback
			}
		}

        // Preserve original shader folder under the executable folder, e.g. <exe_dir>/Assets/Shaders/Mesh_vert.spv
		fs::path relFolder = srcP.parent_path();
		fs::path targetDir = exePath / relFolder;
		if (!fs::exists(targetDir)) fs::create_directories(targetDir);
		fs::path outSpvPath = targetDir / outName;
		outSpv = outSpvPath.string();

        std::string cmd = "glslangValidator -V " + stageFlag + " -o \"" + outSpv + "\" \"" + srcPath + "\"";
		LOG_INFO ( "Compiling shader: ", srcPath, " -> ", outSpv );
		int rc = std::system ( cmd.c_str () );
		if (rc == 0 && fs::exists ( outSpv ))
			return true;
		LOG_WARN ( "Shader compilation failed or glslangValidator not found (rc=)", rc );
		return false;
		}

	return false;
	}

	static bool CreateShaderModule ( VkDevice device, const std::vector<char> & code, VkShaderModule & outModule )
		{
		if (code.empty ())
			return false;

		VkShaderModuleCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size ();
		createInfo.pCode = reinterpret_cast< const uint32_t * >( code.data () );

		if (vkCreateShaderModule ( device, &createInfo, nullptr, &outModule ) != VK_SUCCESS)
			return false;

		return true;
		}
	}

PipelineManager::PipelineManager ( VulkanContextInfo & contextInfo ) : ContextInfo ( contextInfo )
	{}

PipelineManager::~PipelineManager ()
	{
	// Destroy created pipelines
	if (ContextInfo.Device != VK_NULL_HANDLE)
		{
		for (auto & kv : Pipelines)
			{
			if (kv.second != VK_NULL_HANDLE)
				vkDestroyPipeline ( ContextInfo.Device, kv.second, nullptr );
			}
		}
	Pipelines.clear ();
	}

bool PipelineManager::Initialize ()
	{
	// For now create default mesh pipeline eagerly
	if (!CreatePipeline ( "Mesh" ))
		{
		LOG_ERROR ( "Failed to create default Mesh pipeline" );
		return false;
		}
	return true;
	}

VkPipeline PipelineManager::GetPipeline ( const std::string & PipelineName )
	{
	auto it = Pipelines.find ( PipelineName );
	if (it != Pipelines.end ())
		return it->second;

	if (CreatePipeline ( PipelineName ))
		return Pipelines[ PipelineName ];

	return VK_NULL_HANDLE;
	}

bool PipelineManager::CreatePipeline ( const std::string & PipelineName )
	{

	if (PipelineName == "Mesh" || PipelineName == "mesh")
		{
		PipelineConfig cfg = CreateDefaultPipelineConfig ();
		cfg.Name = PipelineName;
     // Mesh pipeline uses a procedural draw (vkCmdDraw with gl_VertexIndex),
		// so leave vertex input empty (no bindings/attributes) so the pipeline
		// does not expect vertex buffers to be bound.
		cfg.Bindings.clear();
		cfg.Attributes.clear();
		VkPipeline pipeline = CreateGraphicsPipeline ( cfg );
		if (pipeline == VK_NULL_HANDLE)
			{
			LOG_ERROR ( "Failed to create ", PipelineName, " pipeline" );
			return false;
			}
		Pipelines.emplace ( PipelineName, pipeline );
		return true;
		}
	if (PipelineName == "Light" || PipelineName == "light")
		{
		LOG_WARN ( "Light pipeline not implemented yet, using Mesh as fallback" );
		return CreatePipeline ( "Mesh" );
		}

	LOG_DEBUG ( "WIP: Implement pipeline creation for ", PipelineName );
	return false;
	}

PipelineConfig PipelineManager::CreateDefaultPipelineConfig () const
	{
	PipelineConfig cfg;
	cfg.Name = "Mesh";
	// Default shader paths - expect SPV files next to executable or project
    // Prefer compiled SPV in assets; if not present, PipelineManager will try to compile GLSL
	cfg.VertexShaderPath = "Assets/Shaders/Mesh.vert";
	cfg.FragmentShaderPath = "Assets/Shaders/Mesh.frag";
	// Vertex input: none by default (user can fill Bindings/Attributes)
	cfg.Bindings.clear ();
	cfg.Attributes.clear ();

	// Input assembly
	cfg.InputAssembly = {};
	cfg.InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	cfg.InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	cfg.InputAssembly.primitiveRestartEnable = VK_FALSE;

	// Rasterization
	cfg.Rasterization = {};
	cfg.Rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	cfg.Rasterization.depthClampEnable = VK_FALSE;
	cfg.Rasterization.rasterizerDiscardEnable = VK_FALSE;
	cfg.Rasterization.polygonMode = VK_POLYGON_MODE_FILL;
	cfg.Rasterization.lineWidth = 1.0f;
 // Disable face culling for the simple test pipeline so the procedural triangle is always visible
	cfg.Rasterization.cullMode = VK_CULL_MODE_NONE;
	cfg.Rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	cfg.Rasterization.depthBiasEnable = VK_FALSE;

    // Multisample
	cfg.Multisample = {};
	cfg.Multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	cfg.Multisample.sampleShadingEnable = VK_FALSE;
	cfg.Multisample.rasterizationSamples = ContextInfo.EnableMSAA ? ContextInfo.MsaaSamples : VK_SAMPLE_COUNT_1_BIT;

	// Depth stencil
	cfg.DepthStencil = {};
	cfg.DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	cfg.DepthStencil.depthTestEnable = ContextInfo.EnableDepthTest ? VK_TRUE : VK_FALSE;
	cfg.DepthStencil.depthWriteEnable = ContextInfo.EnableDepthTest ? VK_TRUE : VK_FALSE;
	cfg.DepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	cfg.DepthStencil.depthBoundsTestEnable = VK_FALSE;
	cfg.DepthStencil.stencilTestEnable = VK_FALSE;

	// Color blend
	cfg.ColorBlendAttachment = {};
	cfg.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	cfg.ColorBlendAttachment.blendEnable = VK_FALSE;

	cfg.ColorBlendState = {};
	cfg.ColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cfg.ColorBlendState.logicOpEnable = VK_FALSE;
	cfg.ColorBlendState.attachmentCount = 1;
	cfg.ColorBlendState.pAttachments = &cfg.ColorBlendAttachment;

	// Dynamic states
	cfg.DynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	return cfg;
	}

VkPipeline PipelineManager::CreateGraphicsPipeline ( const PipelineConfig & config )
	{
	if (ContextInfo.Device == VK_NULL_HANDLE || ContextInfo.RenderPass == VK_NULL_HANDLE || ContextInfo.PipelineLayout == VK_NULL_HANDLE)
		{
		LOG_ERROR ( "Invalid Vulkan context for pipeline creation" );
		return VK_NULL_HANDLE;
		}

	// Read shader files
	std::vector<char> vertCode;
	std::vector<char> fragCode;
    // Ensure we have SPV files; if given path is GLSL source, attempt to compile to SPV
	try
		{
		std::string vertSpv, fragSpv;
        if (!EnsureSpvForSource ( config.VertexShaderPath, vertSpv ))
			throw std::runtime_error("Failed to prepare vertex SPV");
		if (!EnsureSpvForSource ( config.FragmentShaderPath, fragSpv ))
			throw std::runtime_error("Failed to prepare fragment SPV");

		vertCode = ReadFile ( vertSpv );
		fragCode = ReadFile ( fragSpv );
		}
		catch (const std::exception & e)
			{
			LOG_ERROR ( "Failed to read shader SPV: ", e.what () );
			return VK_NULL_HANDLE;
			}

		VkShaderModule vertModule = VK_NULL_HANDLE;
		VkShaderModule fragModule = VK_NULL_HANDLE;

		if (!CreateShaderModule ( ContextInfo.Device, vertCode, vertModule ) || !CreateShaderModule ( ContextInfo.Device, fragCode, fragModule ))
			{
			if (vertModule != VK_NULL_HANDLE) vkDestroyShaderModule ( ContextInfo.Device, vertModule, nullptr );
			if (fragModule != VK_NULL_HANDLE) vkDestroyShaderModule ( ContextInfo.Device, fragModule, nullptr );
			LOG_ERROR ( "Failed to create shader modules" );
			return VK_NULL_HANDLE;
			}

		VkPipelineShaderStageCreateInfo vertStageInfo {};
		vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertStageInfo.module = vertModule;
		vertStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragStageInfo {};
		fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragStageInfo.module = fragModule;
		fragStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages [] = { vertStageInfo, fragStageInfo };

		// Vertex input - use config Bindings/Attributes (copy locally)
		auto localBindings = config.Bindings;
		auto localAttributes = config.Attributes;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = static_cast< uint32_t > ( localBindings.size () );
		vertexInputInfo.pVertexBindingDescriptions = localBindings.empty () ? nullptr : localBindings.data ();
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast< uint32_t > ( localAttributes.size () );
		vertexInputInfo.pVertexAttributeDescriptions = localAttributes.empty () ? nullptr : localAttributes.data ();

		// Copy pipeline state structs from config into locals
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = config.InputAssembly;

		VkPipelineViewportStateCreateInfo viewportState {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer = config.Rasterization;
		VkPipelineMultisampleStateCreateInfo multisampling = config.Multisample;

		VkPipelineDepthStencilStateCreateInfo depthStencil = config.DepthStencil;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = config.ColorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlending = config.ColorBlendState;
		// Ensure attachments pointer points to our local attachment
		colorBlending.pAttachments = &colorBlendAttachment;

		// Dynamic states
		auto localDynamic = config.DynamicStates;
		VkPipelineDynamicStateCreateInfo dynamicState {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast< uint32_t > ( localDynamic.size () );
		dynamicState.pDynamicStates = localDynamic.empty () ? nullptr : localDynamic.data ();

		VkGraphicsPipelineCreateInfo pipelineInfo {};
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
		pipelineInfo.layout = ContextInfo.PipelineLayout;
		pipelineInfo.renderPass = ContextInfo.RenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		VkPipeline pipeline = VK_NULL_HANDLE;
		if (vkCreateGraphicsPipelines ( ContextInfo.Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline ) != VK_SUCCESS)
			{
			LOG_ERROR ( "Failed to create graphics pipeline" );
			vkDestroyShaderModule ( ContextInfo.Device, vertModule, nullptr );
			vkDestroyShaderModule ( ContextInfo.Device, fragModule, nullptr );
			return VK_NULL_HANDLE;
			}

		// Cleanup shader modules - pipeline keeps internal copy
		vkDestroyShaderModule ( ContextInfo.Device, vertModule, nullptr );
		vkDestroyShaderModule ( ContextInfo.Device, fragModule, nullptr );

		return pipeline;
	}
