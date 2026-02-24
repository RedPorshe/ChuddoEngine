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

    // Cache managers for fast access
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

    // Create triangle pipeline - ТОЛЬКО ОДИН РАЗ!
    VkRenderPass renderPass = m_RenderPassManager->GetMainRenderPass ();
    m_TrianglePipeline = m_PipelineManager->CreateTrianglePipeline ( renderPass );
    m_TrianglePipelineLayout = m_PipelineManager->GetTrianglePipelineLayout (); // СОХРАНЯЕМ LAYOUT

    if (m_TrianglePipeline == VK_NULL_HANDLE)
        {
        LogError ( "Failed to create minimal pipeline" );
        return false;
        }

    // Create triangle vertex buffer
    m_TriangleVertexBuffer = m_BufferManager->CreateTriangleVertexBuffer ();
    if (!m_TriangleVertexBuffer.IsValid ())
        {
        LogError ( "Failed to create triangle vertex buffer" );
        return false;
        }

    // Create command buffers for each swap chain image
    if (!CreateCommandBuffers ())
        {
        LogError ( "Failed to create command buffers" );
        return false;
        }

    // Create sync objects for frame management
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

        // Удаляем pipeline layout если он не удалится автоматически
        if (m_TrianglePipelineLayout != VK_NULL_HANDLE)
            {
            vkDestroyPipelineLayout ( device, m_TrianglePipelineLayout, nullptr );
            m_TrianglePipelineLayout = VK_NULL_HANDLE;
            }
        }

    // Command buffers will be cleaned up by CommandManager
    m_CommandBuffers.clear ();

    // Sync objects are cleaned up by SyncManager
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
            LogDebug ( "StartFrame failed, will recreate resources" );

            // Ждем завершения всех операций
            auto * deviceMgr = static_cast< CDeviceManager * >( m_Info.Vulkan.DeviceManager.get () );
            vkDeviceWaitIdle ( deviceMgr->GetDevice () );

            // Пересоздаем ресурсы
            if (!RecreateSwapChainResources ())
                {
                LogError ( "Failed to recreate swapchain resources" );
                return false;
                }

            // Пробуем еще раз начать кадр
            if (!StartFrame ( imageIndex ))
                {
                LogError ( "Failed to start frame after recreation" );
                return false;
                }
            }

        // End frame
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
    // Get current frame index from sync manager
    uint32_t currentFrame = m_SyncManager->GetCurrentFrame ();

    // Wait for the fence of the current frame
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

    // Acquire next image from swap chain
    VkSemaphore imageAvailableSemaphore = m_SyncManager->GetImageAvailableSemaphore ( currentFrame );
    if (!m_SwapChainManager->AcquireNextImage ( imageAvailableSemaphore, ImageIndex ))
        {
        
        // Don't log error here - AcquireNextImage already logs
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
    LogDebug ( "EndFrame begin, ImageIndex: ", ImageIndex );

    uint32_t currentFrame = m_SyncManager->GetCurrentFrame ();
    LogDebug ( "  Current frame: ", currentFrame );

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

    // Get command buffer for this image
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

    LogDebug ( "  Submitting to queue..." );
    VkResult result = vkQueueSubmit ( graphicsQueue, 1, &submitInfo, fence );
    if (result != VK_SUCCESS)
        {
        LogError ( "Failed to submit command buffer: ", static_cast< int >( result ) );
        return false;
        }
    LogDebug ( "  Submit successful" );


    LogDebug ( "  Presenting..." );
    if (!m_SwapChainManager->Present ( renderFinishedSemaphore, ImageIndex ))
        {
        LogDebug ( "  Present failed" );
        return false;
        }
    LogDebug ( "  Present successful" );

    m_SyncManager->NextFrame ();
    LogDebug ( "EndFrame end" );
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

    LogDebug ( "Created ", m_CommandBuffers.size (), " command buffers" );
    return true;
    }

bool CRenderer::RecordCommandBuffer ( VkCommandBuffer CommandBuffer, uint32_t ImageIndex )
    {
    LogDebug ( "RecordCommandBuffer begin, ImageIndex: ", ImageIndex );

    // ПРОВЕРКА 1: CommandBuffer
    if (CommandBuffer == VK_NULL_HANDLE)
        {
        LogError ( "RecordCommandBuffer: CommandBuffer is null" );
        return false;
        }
    LogDebug ( "  CommandBuffer: ", ( void * ) CommandBuffer );

    // ПРОВЕРКА 2: Vertex buffer
    if (!m_TriangleVertexBuffer.IsValid ())
        {
        LogError ( "RecordCommandBuffer: m_TriangleVertexBuffer is invalid" );
        return false;
        }
    LogDebug ( "  Vertex buffer: ", ( void * ) m_TriangleVertexBuffer.Buffer );
    LogDebug ( "  Vertex buffer size: ", m_TriangleVertexBuffer.Size );

    // ПРОВЕРКА 3: Pipeline - ТОЛЬКО ПРОВЕРЯЕМ, НЕ ПЕРЕСОЗДАЁМ!
    if (m_TrianglePipeline == VK_NULL_HANDLE)
        {
        LogError ( "RecordCommandBuffer: m_TrianglePipeline is null" );
        return false;
        }
    LogDebug ( "  Pipeline: ", ( void * ) m_TrianglePipeline );

    // ПРОВЕРКА 4: RenderPass
    VkRenderPass renderPass = m_RenderPassManager->GetMainRenderPass ();
    if (renderPass == VK_NULL_HANDLE)
        {
        LogError ( "RecordCommandBuffer: Main render pass is null" );
        return false;
        }
    LogDebug ( "  RenderPass: ", ( void * ) renderPass );

    // ПРОВЕРКА 5: Framebuffer
    VkFramebuffer framebuffer = m_RenderPassManager->GetFramebuffer ( ImageIndex );
    if (framebuffer == VK_NULL_HANDLE)
        {
        LogError ( "RecordCommandBuffer: Framebuffer for image ", ImageIndex, " is null" );
        return false;
        }
    LogDebug ( "  Framebuffer: ", ( void * ) framebuffer );

    // ПРОВЕРКА 6: SwapChain extent
    VkExtent2D extent = m_SwapChainManager->GetExtent ();
    if (extent.width == 0 || extent.height == 0)
        {
        LogError ( "RecordCommandBuffer: SwapChain extent is invalid: ", extent.width, "x", extent.height );
        return false;
        }
    LogDebug ( "  Extent: ", extent.width, "x", extent.height );

    // Begin command buffer
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

    LogDebug ( "  Calling vkCmdBeginRenderPass..." );
    LogDebug ( "    renderPass: ", ( void * ) renderPassInfo.renderPass );
    LogDebug ( "    framebuffer: ", ( void * ) renderPassInfo.framebuffer );
    LogDebug ( "    renderArea: ", renderPassInfo.renderArea.extent.width, "x", renderPassInfo.renderArea.extent.height );
    LogDebug ( "    clearValueCount: ", renderPassInfo.clearValueCount );
    LogDebug ( "    pClearValues: ", ( void * ) renderPassInfo.pClearValues );

        // ДОПОЛНИТЕЛЬНАЯ ПРОВЕРКА: совместим ли render pass с framebuffer?
    
    

    // В Vulkan API нет прямой проверки, но мы можем залогировать
    LogDebug ( "  Using render pass: ", ( void * ) renderPass, " with framebuffer: ", ( void * ) framebuffer );

    try
        {
        LogDebug ( "Before call vkCmdBeginRenderPass..." );
        vkCmdBeginRenderPass ( CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
        LogDebug ( "  vkCmdBeginRenderPass successful" );
        }
        catch (...)
            {
            LogError ( "  EXCEPTION in vkCmdBeginRenderPass" );
            m_CommandManager->EndCommandBuffer ( CommandBuffer );
            return false;
            }

        if (!m_RenderInfo.HasInfo)
            {
            TriangleStub ( CommandBuffer );
            }
        else
            {
            LOG_DEBUG ( "Rendering world" );
            }

        vkCmdEndRenderPass ( CommandBuffer );
        m_CommandManager->EndCommandBuffer ( CommandBuffer );

        LogDebug ( "Command buffer recorded successfully" );
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


void CRenderer::TriangleStub ( VkCommandBuffer CommandBuffer )
    {
    LogDebug ( "TriangleStub: Drawing triangle" );

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
    LogDebug ( "  Bound pipeline: ", ( void * ) m_TrianglePipeline );

    // Bind vertex buffer
    VkBuffer vertexBuffers [] = { m_TriangleVertexBuffer.Buffer };
    VkDeviceSize offsets [] = { 0 };
    vkCmdBindVertexBuffers ( CommandBuffer, 0, 1, vertexBuffers, offsets );
    LogDebug ( "  Bound vertex buffer: ", ( void * ) m_TriangleVertexBuffer.Buffer );

    // Set viewport
    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast< float >( m_SwapChainManager->GetExtent ().width );
    viewport.height = static_cast< float >( m_SwapChainManager->GetExtent ().height );
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport ( CommandBuffer, 0, 1, &viewport );
    LogDebug ( "  Viewport: ", viewport.width, "x", viewport.height );

    // Set scissor
    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = m_SwapChainManager->GetExtent ();
    vkCmdSetScissor ( CommandBuffer, 0, 1, &scissor );

    // Draw triangle
    vkCmdDraw ( CommandBuffer, 3, 1, 0, 0 );
    LogDebug ( "  Draw call issued" );
    }