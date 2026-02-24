#include "Render/Renderer.h"
#include "Core/EngineInfo.h"
#include "Render/Vulkan/VulkanContext.h"
#include "Render/Vulkan/Managers/SwapChainManager.h"
#include "Render/Vulkan/Managers/CommandManager.h"
#include "Render/Vulkan/Managers/SyncManager.h"
#include "Render/Vulkan/Managers/RenderPassManager.h"
#include "Render/Vulkan/Managers/PipelineManager.h"
#include "Render/Vulkan/Managers/BufferManager.h"
#include "Render/RenderInfo.h"

CRenderer::CRenderer ( FEngineInfo & inInfo )
	: IVulkanManager ( inInfo )
	{}

CRenderer::~CRenderer ()
	{
	Shutdown ();
	LOG_DEBUG ( "Renderer Destroyed" );
	}

bool CRenderer::Initialize ()
	{
	LogDebug ( "Initializing Renderer..." );

	// Create Vulkan context
	m_Info.Vulkan.VulkanContext = std::make_unique<CVulkanContext> ( m_Info );
	if (!m_Info.Vulkan.VulkanContext->Initialize ())
		{
		LogError ( "Failed to initialize Vulkan Context" );
		return false;
		}

	m_SwapChainManager = static_cast< CSwapChainManager * >( m_Info.Vulkan.SwapChainManager.get () );
	m_CommandManager = static_cast< CCommandManager * >( m_Info.Vulkan.CommandManager.get () );
	m_SyncManager = static_cast< CSyncManager * >( m_Info.Vulkan.SyncManager.get () );
	m_RenderPassManager = static_cast< CRenderPassManager * >( m_Info.Vulkan.RenderPassManager.get () );
	m_PipelineManager = static_cast< CPipelineManager * >( m_Info.Vulkan.PipelineManager.get () );
	m_BufferManager = static_cast< CBufferManager * >( m_Info.Vulkan.BufferManager.get () );

	if (!m_SwapChainManager || !m_CommandManager || !m_SyncManager ||
		 !m_RenderPassManager || !m_PipelineManager || !m_BufferManager)
		{
		LogError ( "Failed to cache Vulkan managers" );
		return false;
		}

	VkRenderPass renderPass = m_RenderPassManager->GetMainRenderPass ();
	if (!m_PipelineManager->RegisterDefaultPipelines ( renderPass ))
		{
		LogError ( "Failed to register default pipelines" );
		return false;
		}

	m_TrianglePipeline = m_PipelineManager->GetPipeline ( "TrianglePipeline" );
	m_TrianglePipelineLayout = m_PipelineManager->GetPipelineLayout ( "TriangleLayout" );

	if (m_TrianglePipeline == VK_NULL_HANDLE)
		{
		LogError ( "Failed to get triangle pipeline" );
		return false;
		}

	m_TriangleVertexBuffer = m_BufferManager->CreateTriangleVertexBuffer ();
	if (!m_TriangleVertexBuffer.IsValid ())
		{
		LogError ( "Failed to create triangle vertex buffer" );
		return false;
		}

	if (!CreateCommandBuffers ())
		{
		LogError ( "Failed to create command buffers" );
		return false;
		}

	if (!CreateSyncObjects ())
		{
		LogError ( "Failed to create sync objects" );
		return false;
		}

	LogDebug ( "Renderer initialized successfully" );
	return true;
	}

void CRenderer::Shutdown ()
	{
	LogDebug ( "Renderer shutting down..." );

	// Wait for device idle before destroying resources
	auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	if (deviceMgr)
		{
		VkDevice device = deviceMgr->GetDevice ();
		vkDeviceWaitIdle ( device );
		m_TrianglePipelineLayout = VK_NULL_HANDLE;
		}


	m_CommandBuffers.clear ();

	m_SyncObjectsCreated = false;

	// Clear cached managers
	m_SwapChainManager = nullptr;
	m_CommandManager = nullptr;
	m_SyncManager = nullptr;
	m_RenderPassManager = nullptr;
	m_PipelineManager = nullptr;
	m_BufferManager = nullptr;

	// Shutdown Vulkan context
	m_Info.Vulkan.Shutdown ();

	LogDebug ( "Renderer shutdown complete" );
	}

const char * CRenderer::GetManagerName () const
	{
	return "RENDERER";
	}

void CRenderer::SetInfoForRender ( const FRenderInfo & RenderInfo )
	{
	m_RenderInfo = RenderInfo;
	}

bool CRenderer::RenderScene ()
	{
	if (m_TrianglePipeline == VK_NULL_HANDLE)
		{
		LogDebug ( "  Pipeline not ready, skipping render" );
		return true;
		}

	try
		{
		uint32_t imageIndex;

		// Start frame
		if (!StartFrame ( imageIndex ))
			{
			LOG_WARN ("[",GetManagerName (), "] StartFrame failed, will recreate resources");

			// Ждем завершения всех операций
			auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
			vkDeviceWaitIdle ( deviceMgr->GetDevice () );

			// Пересоздаем ресурсы
			if (!RecreateSwapChainResources ())
				{
				LogError ( "Failed to recreate swapchain resources" );
				return false;
				}

			if (!StartFrame ( imageIndex ))
				{
				LogError ( "Failed to start frame after recreation" );
				return false;
				}
			}

		if (!EndFrame ( imageIndex ))
			{
			LogDebug ( "EndFrame failed" );
			}

		return true;
		}
		catch (const std::exception & e)
			{
			LogError ( "Exception in RenderScene: ", e.what () );
			return false;
			}
		catch (...)
			{
			LogError ( "Unknown exception in RenderScene" );
			return false;
			}
	}

bool CRenderer::StartFrame ( uint32_t & ImageIndex )
	{

	uint32_t currentFrame = m_SyncManager->GetCurrentFrame ();

	VkFence fence = m_SyncManager->GetInFlightFence ( currentFrame );
	if (fence == VK_NULL_HANDLE)
		{
		LogError ( "Invalid fence for frame ", currentFrame );
		return false;
		}

	VkResult result = m_SyncManager->WaitForFence ( fence, UINT64_MAX );
	if (result != VK_SUCCESS)
		{
		LogError ( "Failed to wait for fence: ", static_cast< int >( result ) );
		return false;
		}

	m_SyncManager->ResetFence ( fence );


	VkSemaphore imageAvailableSemaphore = m_SyncManager->GetImageAvailableSemaphore ( currentFrame );
	if (!m_SwapChainManager->AcquireNextImage ( imageAvailableSemaphore, ImageIndex ))
		{

		return false;
		}
	if (m_SwapChainManager->IsSwapChainRecreated ())
		{
		if (RecreateSwapChainResources ()) m_SwapChainManager->SetSwapChainRecreated ( false );
		}
	return true;
	}

bool CRenderer::EndFrame ( uint32_t ImageIndex )
	{
	uint32_t currentFrame = m_SyncManager->GetCurrentFrame ();
	
	VkSemaphore imageAvailableSemaphore = m_SyncManager->GetImageAvailableSemaphore ( currentFrame );
	VkSemaphore renderFinishedSemaphore = m_SyncManager->GetRenderFinishedSemaphore ( currentFrame );
	VkFence fence = m_SyncManager->GetInFlightFence ( currentFrame );

	// ПРОВЕРКИ
	if (imageAvailableSemaphore == VK_NULL_HANDLE)
		{
		LogError ( "  imageAvailableSemaphore is NULL" );
		return false;
		}
	if (renderFinishedSemaphore == VK_NULL_HANDLE)
		{
		LogError ( "  renderFinishedSemaphore is NULL" );
		return false;
		}
	if (fence == VK_NULL_HANDLE)
		{
		LogError ( "  fence is NULL" );
		return false;
		}


	if (ImageIndex >= m_CommandBuffers.size ())
		{
		LogError ( "Image index out of range" );
		return false;
		}

	VkCommandBuffer commandBuffer = m_CommandBuffers[ ImageIndex ];
	if (commandBuffer == VK_NULL_HANDLE)
		{
		LogError ( "  commandBuffer is NULL" );
		return false;
		}

	// Record commands
	if (!RecordCommandBuffer ( commandBuffer, ImageIndex ))
		{
		LogError ( "Failed to record command buffer" );
		return false;
		}

	// Submit command buffer
	auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	if (!deviceMgr)
		{
		LogError ( "  deviceMgr is NULL" );
		return false;
		}

	VkQueue graphicsQueue = deviceMgr->GetGraphicsQueue ();
	if (graphicsQueue == VK_NULL_HANDLE)
		{
		LogError ( "  graphicsQueue is NULL" );
		return false;
		}

	VkSubmitInfo submitInfo {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages [] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

	VkResult result = vkQueueSubmit ( graphicsQueue, 1, &submitInfo, fence );
	if (result != VK_SUCCESS)
		{
		LogError ( "Failed to submit command buffer: ", static_cast< int >( result ) );
		return false;
		}

	if (!m_SwapChainManager->Present ( renderFinishedSemaphore, ImageIndex ))
		{
		LogDebug ( "  Present failed" );
		return false;
		}


	m_SyncManager->NextFrame ();

	return true;
	}

bool CRenderer::CreateSyncObjects ()
	{
	if (m_SyncObjectsCreated) return true;

	// Create sync objects for each frame in flight (using swap chain image count)
	uint32_t imageCount = m_SwapChainManager->GetImageCount ();
	if (!m_SyncManager->CreateFrameSyncObjects ( imageCount ))
		{
		LogError ( "Failed to create frame sync objects" );
		return false;
		}

	m_SyncObjectsCreated = true;
	LogDebug ( "Created sync objects for ", imageCount, " frames" );
	return true;
	}

bool CRenderer::CreateCommandBuffers ()
	{
	uint32_t imageCount = m_SwapChainManager->GetImageCount ();

	m_CommandBuffers = m_CommandManager->CreateCommandBuffers ( imageCount );
	if (m_CommandBuffers.size () != imageCount)
		{
		LogError ( "Failed to create command buffers" );
		return false;
		}

	return true;
	}

bool CRenderer::RecordCommandBuffer ( VkCommandBuffer CommandBuffer, uint32_t ImageIndex )
	{
	if (!m_RenderInfo.HasInfo)
		{
		TriangleStub ( CommandBuffer, ImageIndex );
		}
	else
		{
		RenderWorld ( CommandBuffer );
		LOG_DEBUG ( "Rendering world" );
		}

	vkCmdEndRenderPass ( CommandBuffer );
	m_CommandManager->EndCommandBuffer ( CommandBuffer );
	 
	return true;
	}


bool CRenderer::RecreateSwapChainResources ()
	{
	LogDebug ( "Recreating swapchain resources..." );

	auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
	if (!deviceMgr) return false;

	vkDeviceWaitIdle ( deviceMgr->GetDevice () );

	// Очищаем старые command buffers
	if (!m_CommandBuffers.empty ())
		{
		m_CommandManager->FreeCommandBuffers ( m_CommandBuffers );
		m_CommandBuffers.clear ();
		}

		// ВАЖНО: Пересоздаём render pass с новым форматом из swapchain
	LogDebug ( "  Calling RenderPassManager->RecreateForSwapChain()..." );
	if (!m_RenderPassManager->RecreateForSwapChain ())
		{
		LogError ( "Failed to recreate render pass" );
		return false;
		}

		// Пересоздаём command buffers
	uint32_t imageCount = m_SwapChainManager->GetImageCount ();
	LogDebug ( "  Recreating command buffers for ", imageCount, " images..." );
	m_CommandBuffers = m_CommandManager->CreateCommandBuffers ( imageCount );

	if (m_CommandBuffers.size () != imageCount)
		{
		LogError ( "Failed to recreate command buffers" );
		return false;
		}

	LogDebug ( "Swapchain resources recreated successfully" );
	return true;
	}


void CRenderer::TriangleStub ( VkCommandBuffer CommandBuffer, uint32_t ImageIndex )
	{
	if (CommandBuffer == VK_NULL_HANDLE)
		{
		LogError ( "RecordCommandBuffer: CommandBuffer is null" );
		return;
		} 

	if (!m_TriangleVertexBuffer.IsValid ())
		{
		LogError ( "RecordCommandBuffer: m_TriangleVertexBuffer is invalid" );
		return;
		}


	if (m_TrianglePipeline == VK_NULL_HANDLE)
		{
		LogError ( "RecordCommandBuffer: m_TrianglePipeline is null" );
		return;
		}

	VkRenderPass renderPass = m_RenderPassManager->GetMainRenderPass ();
	if (renderPass == VK_NULL_HANDLE)
		{
		LogError ( "RecordCommandBuffer: Main render pass is null" );
		return;
		}

	VkFramebuffer framebuffer = m_RenderPassManager->GetFramebuffer ( ImageIndex );
	if (framebuffer == VK_NULL_HANDLE)
		{
		LogError ( "RecordCommandBuffer: Framebuffer for image ", ImageIndex, " is null" );
		return;
		}



	VkExtent2D extent = m_SwapChainManager->GetExtent ();
	if (extent.width == 0 || extent.height == 0)
		{
		LogError ( "RecordCommandBuffer: SwapChain extent is invalid: ", extent.width, "x", extent.height );
		return;
		}

	m_CommandManager->BeginCommandBuffer ( CommandBuffer );

	// Begin render pass
	VkRenderPassBeginInfo renderPassInfo {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;

	// Clear values
	VkClearValue clearValues[ 2 ];
	clearValues[ 0 ].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[ 1 ].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clearValues;


	vkCmdBeginRenderPass ( CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
	

	if (!m_TriangleVertexBuffer.IsValid ())
		{
		LogError ( "TriangleStub: Vertex buffer invalid!" );
		return;
		}

	if (m_TrianglePipeline == VK_NULL_HANDLE)
		{
		LogError ( "TriangleStub: Pipeline null!" );
		return;
		}

	// Bind pipeline
	vkCmdBindPipeline ( CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline );
	
	VkBuffer vertexBuffers [] = { m_TriangleVertexBuffer.Buffer };
	VkDeviceSize offsets [] = { 0 };
	vkCmdBindVertexBuffers ( CommandBuffer, 0, 1, vertexBuffers, offsets );
	
	VkViewport viewport {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast< float >( m_SwapChainManager->GetExtent ().width );
	viewport.height = static_cast< float >( m_SwapChainManager->GetExtent ().height );
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport ( CommandBuffer, 0, 1, &viewport );
	
	VkRect2D scissor {};
	scissor.offset = { 0, 0 };
	scissor.extent = m_SwapChainManager->GetExtent ();
	vkCmdSetScissor ( CommandBuffer, 0, 1, &scissor );

	// Draw triangle
	vkCmdDraw ( CommandBuffer, 3, 1, 0, 0 );	
	}

void CRenderer::RenderWorld ( VkCommandBuffer CommandBuffer )
	{
	if (!m_RenderInfo.IsValid ()) return;
	auto StaticMeshPipeline = m_PipelineManager->GetPipeline ( "StaticMesh" );
	if (StaticMeshPipeline == VK_NULL_HANDLE) return;

	static int count = 0;
	if (count >= 100) { LogDebug ( "Rendering works!!" ); count = 0; }
	count++;
	}
