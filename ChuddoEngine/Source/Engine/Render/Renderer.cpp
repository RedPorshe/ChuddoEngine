#include "Render/Renderer.h"
#include "Core/EngineInfo.h"
#include "Render/Vulkan/VulkanContext.h"
#include "Render/Vulkan/Managers/SwapChainManager.h"
#include "Render/Vulkan/Managers/CommandManager.h"
#include "Render/Vulkan/Managers/SyncManager.h"
#include "Render/Vulkan/Managers/RenderPassManager.h"
#include "Render/Vulkan/Managers/PipelineManager.h"
#include "Render/Vulkan/Managers/BufferManager.h"
#include "Render/Vulkan/Managers/WireframePipeline.h"
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
			LOG_WARN ( "[", GetManagerName (), "] StartFrame failed, will recreate resources" );

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
		vkCmdEndRenderPass ( CommandBuffer );
		m_CommandManager->EndCommandBuffer ( CommandBuffer );
		}
	else
		{
		RenderWorld ( CommandBuffer, ImageIndex );
		}
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
	static int WarnCount = 0;
	if (WarnCount < 1)
		{
		LOG_WARN ( "Nothing to render Call Fallback Triangle" );
		WarnCount++;
		}
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

void CRenderer::RenderWorld ( VkCommandBuffer CommandBuffer, uint32_t ImageIndex )
	{
	if (!m_RenderInfo.IsValid ())
		{
		LOG_WARN ( "RenderWorld called but no valid render info" );
		return;
		}

	VkExtent2D extent = m_SwapChainManager->GetExtent ();
	if (extent.width == 0 || extent.height == 0)
		{
		LogError ( "Invalid swapchain extent" );
		return;
		}

	
	

	m_CommandManager->BeginCommandBuffer ( CommandBuffer );

	// Begin render pass
	VkRenderPassBeginInfo renderPassInfo {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_RenderPassManager->GetMainRenderPass ();
	renderPassInfo.framebuffer = m_RenderPassManager->GetFramebuffer ( ImageIndex );
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;

	VkClearValue clearValues[ 2 ];
	clearValues[ 0 ].color = { {0.2f, 0.3f, 0.4f, 1.0f} };
	clearValues[ 1 ].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clearValues;

	vkCmdBeginRenderPass ( CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

	// Устанавливаем viewport и scissor
	VkViewport viewport {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast< float >( extent.width );
	viewport.height = static_cast< float >( extent.height );
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport ( CommandBuffer, 0, 1, &viewport );

	VkRect2D scissor {};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	vkCmdSetScissor ( CommandBuffer, 0, 1, &scissor );

	// Структуры для push constants
	struct FCommonPushConstants
		{
		glm::mat4x4 view;
		glm::mat4x4 projection;
		glm::mat4x4 model;
		// Дополнительные параметры в зависимости от типа
		float params[ 8 ];
		} pushConstants;

	struct FMeshPushConstants
		{
		glm::mat4x4 view;
		glm::mat4x4 projection;
		glm::mat4x4 model;
		} meshPushConstants;

	pushConstants.view = CEMath::ToGLM( m_RenderInfo.Camera.GetViewMatrix ());
	pushConstants.projection = CEMath::ToGLM (m_RenderInfo.Camera.GetProjectionMatrix ());
	meshPushConstants.view = CEMath::ToGLM(m_RenderInfo.Camera.GetViewMatrix ());
	meshPushConstants.projection = CEMath::ToGLM( m_RenderInfo.Camera.GetProjectionMatrix ());

	// ========== РЕНДЕР СТАТИЧЕСКИХ МЕШЕЙ ==========
	VkPipeline currentMeshPipeline = VK_NULL_HANDLE;
	VkPipelineLayout currentMeshLayout = VK_NULL_HANDLE;
	std::string currentMeshPipelineName;

	const auto & meshes = m_RenderInfo.RenderMeshes;
	for (size_t i = 0; i < meshes.size (); i++)
		{
		const FMeshInfo & mesh = meshes[ i ];

		if (!mesh.IsValid ())
			{
			LOG_WARN ( "Skipping invalid mesh at index ", i );
			continue;
			}
		currentMeshPipelineName = mesh.PipelineName;


		currentMeshPipeline = m_PipelineManager->GetPipeline ( currentMeshPipelineName );
		currentMeshLayout = m_PipelineManager->GetPipelineLayout ( currentMeshPipelineName + "Layout" );

		if (currentMeshPipeline == VK_NULL_HANDLE || currentMeshLayout == VK_NULL_HANDLE)
			{
			LOG_ERROR ( "Pipeline or layout not found for: ", currentMeshPipelineName );
			continue;
			}


		vkCmdBindPipeline ( CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentMeshPipeline );

		meshPushConstants.model = CEMath::ToGLM( mesh.Model);
		

		vkCmdPushConstants (
			CommandBuffer,
			currentMeshLayout,  // ВАЖНО: используем layout для мешей!
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof ( meshPushConstants ),
			&meshPushConstants
		);

		VkBuffer vertexBuffers [] = { mesh.VertexBuffer };
		VkDeviceSize offsets [] = { 0 };
		vkCmdBindVertexBuffers ( CommandBuffer, 0, 1, vertexBuffers, offsets );

		if (mesh.IndexBuffer != VK_NULL_HANDLE && mesh.IndexCount > 0)
			{
			vkCmdBindIndexBuffer ( CommandBuffer, mesh.IndexBuffer, 0, VK_INDEX_TYPE_UINT32 );
			vkCmdDrawIndexed ( CommandBuffer, mesh.IndexCount, 1, 0, 0, 0 );
			}
		else
			{
			vkCmdDraw ( CommandBuffer, mesh.VertexCount, 1, 0, 0 );
			}
		}



	// ========== РЕНДЕР ТЕРРЕЙНОВ ==========
	// Используем отдельные переменные для террейна
	VkPipeline terrainPipeline = m_PipelineManager->GetPipeline ( "TerrainPipeline" );
	VkPipelineLayout terrainLayout = m_PipelineManager->GetPipelineLayout ( "TerrainLayout" );



	if (terrainPipeline != VK_NULL_HANDLE && terrainLayout != VK_NULL_HANDLE)
		{
		vkCmdBindPipeline ( CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, terrainPipeline );

		const auto & terrains = m_RenderInfo.Terrains;
		for (size_t i = 0; i < terrains.size (); i++)
			{
			const FTerrainRenderInfo & terrain = terrains[ i ];

			if (!terrain.IsValid ())
				{
				LOG_WARN ( "Skipping invalid terrain at index ", i );
				continue;
				}

			pushConstants.model = CEMath::ToGLM(terrain.Model);

			// Заполняем параметры террейна
			pushConstants.params[ 0 ] = terrain.Params.TilingFactor;
			pushConstants.params[ 1 ] = terrain.Params.HeightScale;
			pushConstants.params[ 2 ] = terrain.Params.FogDensity;
			pushConstants.params[ 3 ] = terrain.Params.UseTexture;
			pushConstants.params[ 4 ] = terrain.Params.SandHeight;
			pushConstants.params[ 5 ] = terrain.Params.GrassHeight;
			pushConstants.params[ 6 ] = terrain.Params.RockHeight;
			pushConstants.params[ 7 ] = terrain.Params.SnowHeight;

			vkCmdPushConstants (
				CommandBuffer,
				terrainLayout,  // ВАЖНО: используем layout для террейна!
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof ( pushConstants ),
				&pushConstants
			);

			VkBuffer vertexBuffers [] = { terrain.VertexBuffer };
			VkDeviceSize offsets [] = { 0 };
			vkCmdBindVertexBuffers ( CommandBuffer, 0, 1, vertexBuffers, offsets );

			if (terrain.IndexBuffer != VK_NULL_HANDLE && terrain.IndexCount > 0)
				{
				vkCmdBindIndexBuffer ( CommandBuffer, terrain.IndexBuffer, 0, VK_INDEX_TYPE_UINT32 );
				vkCmdDrawIndexed ( CommandBuffer, terrain.IndexCount, 1, 0, 0, 0 );
				}
			else
				{
				vkCmdDraw ( CommandBuffer, terrain.VertexCount, 1, 0, 0 );
				}
			}
		}

	 // ========== РЕНДЕР ОТЛАДКИ КОЛЛИЗИЙ ==========
	if (m_RenderInfo.bDrawCollisions && !m_RenderInfo.DebugCollisions.empty ())
		{
		VkPipeline wireframePipeline = m_PipelineManager->GetPipeline ( "WireframePipeline" );
		VkPipelineLayout wireframeLayout = m_PipelineManager->GetPipelineLayout ( "WireframePipelineLayout" );

		if (wireframePipeline != VK_NULL_HANDLE && wireframeLayout != VK_NULL_HANDLE)
			{
			vkCmdBindPipeline ( CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, wireframePipeline );

			// Push constants для wireframe
			struct FWireframePushConstants
				{
				FMat4 view;
				FMat4 projection;
				FMat4 model;
				} wireframePush;

			wireframePush.view = m_RenderInfo.Camera.GetViewMatrix ();
			wireframePush.projection = m_RenderInfo.Camera.GetProjectionMatrix ();
			wireframePush.model = FMat4::IdentityMatrix (); // Используем identity, т.к. позиции уже в мировых координатах

			vkCmdPushConstants (
				CommandBuffer,
				wireframeLayout,
				VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof ( wireframePush ),
				&wireframePush
			);

			// Генератор wireframe геометрии
			CWireframeGenerator generator;

			// Для каждой коллизионной формы генерируем временные буферы
			for (const auto & collision : m_RenderInfo.DebugCollisions)
				{
				if (!collision.IsValid ()) continue;

				std::vector<FWireframeVertex> vertices;

				// Генерируем вершины в зависимости от типа
				switch (collision.ShapeType)
					{
						case ECollisionShape::SPHERE:
							generator.GenerateSphere ( vertices,
													   collision.WorldLocation,
													   collision.Params.Sphere.Radius,
													   collision.DebugColor );
							break;

						case ECollisionShape::BOX:
							generator.GenerateBox ( vertices,
													collision.WorldLocation,
													collision.WorldRotation,
													collision.Params.Box.HalfExtents,
													collision.DebugColor );
							break;

						case ECollisionShape::CAPSULE:
							generator.GenerateCapsule ( vertices,
														collision.WorldLocation,
														collision.WorldRotation,
														collision.Params.Capsule.Radius,
														collision.Params.Capsule.HalfHeight,
														collision.DebugColor );
							break;

						case ECollisionShape::CYLINDER:
							generator.GenerateCylinder ( vertices,
														 collision.WorldLocation,
														 collision.WorldRotation,
														 collision.Params.Cylinder.Radius,
														 collision.Params.Cylinder.Height,
														 collision.DebugColor );
							break;

						case ECollisionShape::CONE:
							generator.GenerateCone ( vertices,
													 collision.WorldLocation,
													 collision.WorldRotation,
													 collision.Params.Cone.Radius,
													 collision.Params.Cone.Height,
													 collision.DebugColor );
							break;

						default:
							continue;
					}

				if (vertices.empty ()) continue;

				// Создаём временный вершинный буфер
				FBuffer tempBuffer = m_BufferManager->CreateVertexBuffer ( vertices );

				if (tempBuffer.IsValid ())
					{
					VkBuffer vertexBuffers [] = { tempBuffer.Buffer };
					VkDeviceSize offsets [] = { 0 };
					vkCmdBindVertexBuffers ( CommandBuffer, 0, 1, vertexBuffers, offsets );

					// Рисуем линии
					vkCmdDraw ( CommandBuffer, static_cast< uint32_t >( vertices.size () ), 1, 0, 0 );

					// Буфер будет уничтожен BufferManager'ом при следующем кадре
					// или можно добавить временное хранилище
					}
				}
			}
		}


	vkCmdEndRenderPass ( CommandBuffer );
	m_CommandManager->EndCommandBuffer ( CommandBuffer );
	}

