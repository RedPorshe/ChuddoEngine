#include "Render/Vulkan/VulkanContext.h"
#include "Render/Vulkan/PipelineManager.h"
#include "Render/Vulkan/VulkanRenderer.h"
#include "Render/Vulkan/SwapChainManager.h"
#include "Render/Vulkan/CommandManager.h"
#include "Render/Vulkan/BufferManager.h"
#include "Render/Vulkan/ImageManager.h"
#include "Render/Vulkan/SyncManager.h"
#include "Render/Vulkan/InstanceVK.h"
#include "Render/Vulkan/VulkanDevice.h"
#include "Core/Engine.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <limits>

// ============================================================================
// QueueFamilyIndices Implementation
// ============================================================================

void QueueFamilyIndices::Log () const
    {
    LOG_DEBUG ( "Queue Families:" );
    if (GraphicsFamily.has_value ())
        LOG_DEBUG ( "  Graphics: ", GraphicsFamily.value () );
    if (PresentFamily.has_value ())
        LOG_DEBUG ( "  Present: ", PresentFamily.value () );
    if (ComputeFamily.has_value ())
        LOG_DEBUG ( "  Compute: ", ComputeFamily.value () );
    if (TransferFamily.has_value ())
        LOG_DEBUG ( "  Transfer: ", TransferFamily.value () );
    }

    // ============================================================================
    // SwapChainSupportDetails Implementation
    // ============================================================================

VkSurfaceFormatKHR SwapChainSupportDetails::ChooseFormat () const
    {
    for (const auto & format : Formats)
        {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
             format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
            LOG_DEBUG ( "Selected swapchain format: B8G8R8A8_SRGB" );
            return format;
            }
        }
    LOG_DEBUG ( "Selected swapchain format: ", Formats[ 0 ].format );
    return Formats[ 0 ];
    }

    // ⚡ ИСПРАВЛЕНО: добавлен параметр bool enableVsync
VkPresentModeKHR SwapChainSupportDetails::ChoosePresentMode ( bool enableVsync ) const
    {
    if (!enableVsync)
        {
        for (const auto & mode : PresentModes)
            {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                LOG_DEBUG ( "Selected present mode: MAILBOX (triple buffering)" );
                return mode;
                }
            if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                {
                LOG_DEBUG ( "Selected present mode: IMMEDIATE (no vsync)" );
                return mode;
                }
            }
        }

    LOG_DEBUG ( "Selected present mode: FIFO (vsync)" );
    return VK_PRESENT_MODE_FIFO_KHR;
    }

VkExtent2D SwapChainSupportDetails::ChooseExtent ( int windowWidth, int windowHeight ) const
    {
    if (Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max ())
        {
        return Capabilities.currentExtent;
        }

    VkExtent2D actualExtent = {
        static_cast< uint32_t >( windowWidth ),
        static_cast< uint32_t >( windowHeight )
        };

    actualExtent.width = std::clamp ( actualExtent.width,
                                      Capabilities.minImageExtent.width,
                                      Capabilities.maxImageExtent.width );
    actualExtent.height = std::clamp ( actualExtent.height,
                                       Capabilities.minImageExtent.height,
                                       Capabilities.maxImageExtent.height );

    LOG_DEBUG ( "Selected swapchain extent: ", actualExtent.width, "x", actualExtent.height );
    return actualExtent;
    }

void SwapChainSupportDetails::Log () const
    {
    LOG_DEBUG ( "SwapChain Support:" );
    LOG_DEBUG ( "  Min Image Count: ", Capabilities.minImageCount );
    LOG_DEBUG ( "  Max Image Count: ", Capabilities.maxImageCount );
    LOG_DEBUG ( "  Current Extent: ", Capabilities.currentExtent.width, "x",
                Capabilities.currentExtent.height );
    LOG_DEBUG ( "  Min Extent: ", Capabilities.minImageExtent.width, "x",
                Capabilities.minImageExtent.height );
    LOG_DEBUG ( "  Max Extent: ", Capabilities.maxImageExtent.width, "x",
                Capabilities.maxImageExtent.height );
    LOG_DEBUG ( "  Max Image Array Layers: ", Capabilities.maxImageArrayLayers );
    LOG_DEBUG ( "  Supported transforms: ", Capabilities.supportedTransforms );
    LOG_DEBUG ( "  Current transform: ", Capabilities.currentTransform );
    LOG_DEBUG ( "  Supported composite alpha: ", Capabilities.supportedCompositeAlpha );
    LOG_DEBUG ( "  Supported usage flags: ", Capabilities.supportedUsageFlags );
    LOG_DEBUG ( "  Formats available: ", Formats.size () );
    LOG_DEBUG ( "  Present modes available: ", PresentModes.size () );
    }

    // ============================================================================
    // VulkanContext Implementation
    // ============================================================================

VulkanContext::VulkanContext ()
    {}

VulkanContext::~VulkanContext ()
    {}

bool VulkanContext::Initialize ( VulkanContextInfo * info, GLFWwindow * inWindow )
    {
        // Use the provided native window. Do not create another GLFW window here
    if (!inWindow)
        {
        LOG_FATAL ( "No native GLFW window provided to VulkanContext::Initialize" );
        return false;
        }

    Window = inWindow;
    VulkanInstance = std::make_unique<InstanceVK> ();
    if (!VulkanInstance.get ()->Initialize ( *info ))
        {
        LOG_ERROR ( "Failed to initialize Vulkan instance" );
        return false;
        }

    if (glfwCreateWindowSurface ( VulkanInstance.get ()->GetInstance (), Window, nullptr, &Surface ) != VK_SUCCESS)
        {
        LOG_FATAL ( "Failed to create window surface" );
        return false;
        }
    info->Surface = Surface;

    // Copy provided info into internal ContextInfo so managers can use it
    ContextInfo = *info;

    m_Device = std::make_unique<VulkanDevice> ();
    if (!m_Device.get ()->Initialize ( ContextInfo ))  // initialize device and fill ContextInfo with handles
        {
        LOG_ERROR ( "Failed to initialize Vulkan device" );
        return false;
        }

    // Initialize swapchain manager and create swapchain, render pass, framebuffers
    SwapChainMgr = std::make_unique<SwapChainManager> ();
    if (!SwapChainMgr->Initialize(ContextInfo, m_Device.get()))
    {
        LOG_ERROR("Failed to initialize SwapChainManager");
        return false;
    }

    // Create depth image and views
    ImgMgr = std::make_unique<ImageManager>();
    if (!ImgMgr->Initialize(ContextInfo, m_Device.get()))
    {
        LOG_ERROR("Failed to initialize ImageManager");
        return false;
    }

    // Now that depth exists, create render pass, pipeline layout and framebuffers
    // For simplicity, create a basic renderpass here
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = ContextInfo.SwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = ContextInfo.DepthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<VkAttachmentDescription,2> attachments = { colorAttachment, depthAttachment };

    VkRenderPassCreateInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    rpInfo.pAttachments = attachments.data();
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(ContextInfo.Device, &rpInfo, nullptr, &ContextInfo.RenderPass) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create render pass");
        return false;
    }

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 0;
    layoutInfo.pSetLayouts = nullptr;
    if (vkCreatePipelineLayout(ContextInfo.Device, &layoutInfo, nullptr, &ContextInfo.PipelineLayout) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create pipeline layout");
        return false;
    }

    // Create framebuffers now that renderpass and depth image view exist
    if (!SwapChainMgr->CreateFramebuffers(ContextInfo))
    {
        LOG_ERROR("Failed to create framebuffers");
        return false;
    }

    // Initialize sync objects
    SyncMgr = std::make_unique<SyncManager>();
    if (!SyncMgr.get()->Initialize (ContextInfo))
    {
        LOG_ERROR("Failed to initialize SyncManager");
        return false;
    }

    // Ensure sync image tracking matches swapchain images
    SyncMgr->ResizeImageTracking(ContextInfo, ContextInfo.SwapChainImages.size());

    // Allocate command buffers
    CmdMgr = std::make_unique<CommandManager>();
    if (!CmdMgr->AllocateCommandBuffers(ContextInfo))
    {
        LOG_ERROR("Failed to allocate command buffers");
        return false;
    }

    // Initialize pipeline manager after device and render pass are ready
    PipelineMgr = std::make_unique<PipelineManager> ( ContextInfo );
    if (!PipelineMgr->Initialize ())
    {
        LOG_WARN ( "PipelineManager failed to initialize" );
        // Not fatal - renderer may still function for debug
    }

    LOG_INFO ( "VulkanRenderer initialized (basic)." );
    return true;
    }

void VulkanContext::Shutdown ()
    {
    // Ensure the device is idle before destroying any Vulkan objects to avoid validation errors
    if (ContextInfo.Device != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(ContextInfo.Device);
    }
    // Shutdown managers in reverse order
    if (PipelineMgr)
        PipelineMgr.reset();

    if (CmdMgr)
        CmdMgr->Shutdown(ContextInfo);

    if (ImgMgr)
        ImgMgr->Shutdown(ContextInfo);

    if (SyncMgr)
        SyncMgr->Shutdown(ContextInfo);

    if (SwapChainMgr)
        SwapChainMgr->Shutdown(ContextInfo);

    if (m_Device)
        m_Device.get ()->Shutdown ( ContextInfo );
        // Log current handles to help diagnose shutdown ordering issues
    LOG_DEBUG ( "VulkanContext::Shutdown() - InstanceVk=", ( void * ) VulkanInstance.get ()->GetInstance (),
                " Surface=", ( void * ) Surface, " Window=", ( void * ) Window );

      // Destroy Vulkan surface and instance if they exist. Do not touch GLFW window here
      // (the window owner is responsible for destroying it).
    if (Surface != VK_NULL_HANDLE)
        {
            // Destroy the surface first
        vkDestroySurfaceKHR ( VulkanInstance.get ()->GetInstance (), Surface, nullptr );
        Surface = VK_NULL_HANDLE;
        }

    VulkanInstance.get ()->Shutdown ();

    LOG_DEBUG ( "Vulkan Context ShutDown" );
    }

void VulkanContext::Render ( const RenderScene & scene )
    {
    static int FrameCount = 0;
    static float timer = 0.f;
    static float totalTime = 0.f;

    FrameCount++;
    timer += scene.DeltaTime;
    totalTime += scene.DeltaTime;

    VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;
    if (!StartFrame ( cmdBuffer, scene ))
        {
        LOG_ERROR ( "Failed to start frame" );
        return;
        }

    if (!EndFrame ( cmdBuffer ))
        {
        LOG_ERROR ( "Failed to end frame" );
        return;
        }

    if (timer > 3.0f)
        {
        float instantFPS = 1.0f / scene.DeltaTime;
        float avgFPS = FrameCount / totalTime;

        LOG_DEBUG ( "[VULKAN RENDERER] Frame ", FrameCount,
                    " FPS: ", instantFPS,
                    " Delta: ", scene.DeltaTime * 1000.0f, "ms",
                    " Avg FPS: ", avgFPS,
                    " Time: ", totalTime, "s" );
        timer = 0.f;
        }
    }

VkInstance VulkanContext::GetInstance () const
    {
    return VulkanInstance ? VulkanInstance->GetInstance () : VK_NULL_HANDLE;
    }

void VulkanContext::RecreateSwapChain()
{
    vkDeviceWaitIdle(ContextInfo.Device);

    // Cleanup previous
    if (ImgMgr)
        ImgMgr->Shutdown(ContextInfo);
    if (SwapChainMgr)
        SwapChainMgr->Shutdown(ContextInfo);

    // Recreate
    if (SwapChainMgr && !SwapChainMgr->Initialize(ContextInfo, m_Device.get()))
    {
        LOG_ERROR("Failed to recreate swapchain");
        return;
    }

    if (ImgMgr && !ImgMgr->Initialize(ContextInfo, m_Device.get()))
    {
        LOG_ERROR("Failed to recreate depth image after swapchain recreation");
        return;
    }

    // Recreate render pass and pipeline layout (they are destroyed during shutdown)
    // Create a basic render pass matching attachments: color then depth
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = ContextInfo.SwapChainImageFormat;
    colorAttachment.samples = ContextInfo.EnableMSAA ? ContextInfo.MsaaSamples : VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = ContextInfo.DepthFormat;
    depthAttachment.samples = ContextInfo.EnableMSAA ? ContextInfo.MsaaSamples : VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    rpInfo.pAttachments = attachments.data();
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(ContextInfo.Device, &rpInfo, nullptr, &ContextInfo.RenderPass) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to recreate render pass");
        return;
    }

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 0;
    layoutInfo.pSetLayouts = nullptr;
    if (vkCreatePipelineLayout(ContextInfo.Device, &layoutInfo, nullptr, &ContextInfo.PipelineLayout) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to recreate pipeline layout");
        return;
    }

    // Recreate framebuffers now that renderpass and depth image view exist
    if (SwapChainMgr && !SwapChainMgr->CreateFramebuffers(ContextInfo))
    {
        LOG_ERROR("Failed to recreate framebuffers");
        return;
    }

    // Reallocate command buffers
    if (CmdMgr)
        CmdMgr->AllocateCommandBuffers(ContextInfo);

    // Resize sync tracking
    if (SyncMgr)
        SyncMgr->ResizeImageTracking(ContextInfo, ContextInfo.SwapChainImages.size());
}

bool VulkanContext::StartFrame ( VkCommandBuffer & cmdBuffer, const RenderScene & scene )
    {
    ( void ) scene;

    // Ensure swapchain and resources exist
    if (!ContextInfo.IsSwapChainValid ())
        {
        LOG_ERROR ( "SwapChain not valid in StartFrame" );
        return false;
        }

    // Acquire next image
    uint32_t imageIndex = 0;
    // Wait for the previous frame to finish using this frame's fence
    if (!ContextInfo.InFlightFences.empty())
    {
        vkWaitForFences(ContextInfo.Device, 1, &ContextInfo.InFlightFences[ContextInfo.CurrentFrame], VK_TRUE, UINT64_MAX);
    }
    VkResult result = vkAcquireNextImageKHR ( ContextInfo.Device,
                                               ContextInfo.SwapChain,
                                               UINT64_MAX,
                                               ContextInfo.ImageAvailableSemaphores[ ContextInfo.CurrentFrame ],
                                               VK_NULL_HANDLE,
                                               &imageIndex );

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
        LOG_WARN ( "Swapchain out of date on acquire" );
        // Try to recreate swapchain
        RecreateSwapChain();
        return false;
        }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
        LOG_ERROR ( "Failed to acquire swapchain image" );
        return false;
        }

    LastImageIndex = static_cast<int> ( imageIndex );

    // Get command buffer for this frame
    if (ContextInfo.CommandBuffers.empty ())
        {
        LOG_ERROR ( "No command buffers available" );
        return false;
        }

    cmdBuffer = ContextInfo.CommandBuffers[ imageIndex ];

    // If the image is still in flight, wait for its fence
    if (!ContextInfo.ImagesInFlight.empty())
    {
        VkFence imageFence = ContextInfo.ImagesInFlight[imageIndex];
        if (imageFence != VK_NULL_HANDLE)
        {
            vkWaitForFences(ContextInfo.Device, 1, &imageFence, VK_TRUE, UINT64_MAX);
        }
        // Mark this image as now being in use by this frame
        ContextInfo.ImagesInFlight[imageIndex] = ContextInfo.InFlightFences[ContextInfo.CurrentFrame];
    }

    // Begin recording
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer ( cmdBuffer, &beginInfo ) != VK_SUCCESS)
        {
        LOG_ERROR ( "Failed to begin command buffer" );
        return false;
        }

    // Begin render pass
    VkClearValue clearValues[2] {};
    clearValues[0].color = { { 0.1f, 0.1f, 0.1f, 1.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = ContextInfo.RenderPass;
    renderPassInfo.framebuffer = ContextInfo.SwapChainFramebuffers[ imageIndex ];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = ContextInfo.SwapChainExtent;
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass ( cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    // Bind a default pipeline if available
    VkPipeline pipe = PipelineMgr ? PipelineMgr->GetPipeline ( "Mesh" ) : VK_NULL_HANDLE;
    if (pipe != VK_NULL_HANDLE)
        {
        vkCmdBindPipeline ( cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe );
        // Ensure viewport and scissor are set if pipeline uses dynamic state
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(ContextInfo.SwapChainExtent.width);
        viewport.height = static_cast<float>(ContextInfo.SwapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = ContextInfo.SwapChainExtent;
        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
        // Draw a simple procedural triangle (vertex shader uses gl_VertexIndex)
        vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
        }

    return true;
    }

bool VulkanContext::EndFrame ( VkCommandBuffer & cmdBuffer )
    {
    if (cmdBuffer == VK_NULL_HANDLE)
        return false;

    // End render pass and finish command buffer
    vkCmdEndRenderPass ( cmdBuffer );

    if (vkEndCommandBuffer ( cmdBuffer ) != VK_SUCCESS)
        {
        LOG_ERROR ( "Failed to record command buffer" );
        return false;
        }

    // Submit
    VkSemaphore waitSemaphores[] = { ContextInfo.ImageAvailableSemaphores[ ContextInfo.CurrentFrame ] };
    VkSemaphore signalSemaphores[] = { ContextInfo.RenderFinishedSemaphores[ ContextInfo.CurrentFrame ] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // Reset fence and submit
    vkResetFences ( ContextInfo.Device, 1, &ContextInfo.InFlightFences[ ContextInfo.CurrentFrame ] );

    if (vkQueueSubmit ( ContextInfo.GraphicsQueue, 1, &submitInfo, ContextInfo.InFlightFences[ ContextInfo.CurrentFrame ] ) != VK_SUCCESS)
        {
        LOG_ERROR ( "Failed to submit draw command buffer" );
        return false;
        }

    // Present
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { ContextInfo.SwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    uint32_t imageIndex = static_cast<uint32_t> ( LastImageIndex );
    presentInfo.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR ( ContextInfo.PresentQueue, &presentInfo );
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
        LOG_WARN ( "Swapchain out of date or suboptimal on present" );
        RecreateSwapChain();
        return false;
        }
    else if (result != VK_SUCCESS)
        {
        LOG_ERROR ( "Failed to present swapchain image" );
        return false;
        }

    // Advance frame
    ContextInfo.CurrentFrame = ( ContextInfo.CurrentFrame + 1 ) % ContextInfo.MaxFramesInFlight;

    return true;
    }

void VulkanContextInfo::Clear ()
    {
        // Don't destroy anything here - just clear handles
        // Actual destruction should be done in proper order by VulkanDevice and VulkanContext

    Instance = VK_NULL_HANDLE;
    Surface = VK_NULL_HANDLE;
    Device = VK_NULL_HANDLE;
    PhysicalDevice = VK_NULL_HANDLE;

    GraphicsQueue = VK_NULL_HANDLE;
    PresentQueue = VK_NULL_HANDLE;
    ComputeQueue = VK_NULL_HANDLE;
    TransferQueue = VK_NULL_HANDLE;

    QueueIndices = QueueFamilyIndices {};
    SwapChainSupport = SwapChainSupportDetails {};

    SwapChain = VK_NULL_HANDLE;
    SwapChainImageFormat = VK_FORMAT_UNDEFINED;
    SwapChainExtent = { 800, 600 };
    SwapChainImages.clear ();
    SwapChainImageViews.clear ();

    GraphicsCommandPool = VK_NULL_HANDLE;
    ComputeCommandPool = VK_NULL_HANDLE;
    TransferCommandPool = VK_NULL_HANDLE;

    DepthFormat = VK_FORMAT_UNDEFINED;
    DepthImage = VK_NULL_HANDLE;
    DepthImageMemory = VK_NULL_HANDLE;
    DepthImageView = VK_NULL_HANDLE;

    ImageAvailableSemaphores.clear ();
    RenderFinishedSemaphores.clear ();
    InFlightFences.clear ();
    ImagesInFlight.clear ();
    CurrentFrame = 0;

    RenderPass = VK_NULL_HANDLE;
    PipelineLayout = VK_NULL_HANDLE;
    GraphicsPipeline = VK_NULL_HANDLE;

    SwapChainFramebuffers.clear ();
    CommandBuffers.clear ();
    }