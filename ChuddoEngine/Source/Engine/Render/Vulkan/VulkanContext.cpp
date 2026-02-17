#include "Render/Vulkan/VulkanContext.h"
#include "Render/Vulkan/VulkanRenderer.h"
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

    m_Device = std::make_unique<VulkanDevice> ();
    if (!m_Device.get ()->Initialize ( *info ))  // ⚡ Здесь была ошибка! Должно быть если не успешно, то ошибка
        {
        LOG_ERROR ( "Failed to initialize Vulkan device" );
        return false;
        }

    LOG_INFO ( "VulkanRenderer initialized (basic)." );
    return true;
    }

void VulkanContext::Shutdown ()
    {
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

bool VulkanContext::StartFrame ( VkCommandBuffer & cmdBuffer, const RenderScene & scene )
    {
    ( void ) cmdBuffer;
    ( void ) scene;
    return true;
    }

bool VulkanContext::EndFrame ( VkCommandBuffer & cmdBuffer )
    {
    ( void ) cmdBuffer;
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