#pragma once
#include "CoreMinimal.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

struct GLFWwindow;
struct RenderScene;
class InstanceVK;
class VulkanDevice;

struct QueueFamilyIndices
    {
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;
    std::optional<uint32_t> ComputeFamily;
    std::optional<uint32_t> TransferFamily;

    bool IsComplete () const
        {
        return GraphicsFamily.has_value () && PresentFamily.has_value ();
        }

    bool HasCompute () const { return ComputeFamily.has_value (); }
    bool HasTransfer () const { return TransferFamily.has_value (); }

    void Log () const;
    };

struct SwapChainSupportDetails
    {
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;

    bool IsComplete () const { return !Formats.empty () && !PresentModes.empty (); }

    VkSurfaceFormatKHR ChooseFormat () const;
    VkPresentModeKHR ChoosePresentMode ( bool enableVsync = true ) const;
    VkExtent2D ChooseExtent ( int windowWidth, int windowHeight ) const;

    void Log () const;
    };

struct VulkanContextInfo
    {
        // Basic info
    std::string AppName = "ChuddoEngine";
    std::string EngineName = "ChuddoEngine";

    int AppVersionMajor = 1;
    int AppVersionMinor = 0;
    int AppVersionPatch = 0;

    int EngineVersionMajor = 1;
    int EngineVersionMinor = 0;
    int EngineVersionPatch = 0;

    int WindowWidth = 800;
    int WindowHeight = 600;

    // Vulkan handles
    VkInstance Instance = VK_NULL_HANDLE;
    VkSurfaceKHR Surface = VK_NULL_HANDLE;
    VkDevice Device = VK_NULL_HANDLE;
    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;

    // Queues
    VkQueue GraphicsQueue = VK_NULL_HANDLE;
    VkQueue PresentQueue = VK_NULL_HANDLE;
    VkQueue ComputeQueue = VK_NULL_HANDLE;
    VkQueue TransferQueue = VK_NULL_HANDLE;

    // Queue families
    QueueFamilyIndices QueueIndices;

    // Swap chain
    SwapChainSupportDetails SwapChainSupport;
    VkSwapchainKHR SwapChain = VK_NULL_HANDLE;
    VkFormat SwapChainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D SwapChainExtent = { 800, 600 };
    std::vector<VkImage> SwapChainImages;
    std::vector<VkImageView> SwapChainImageViews;

    // Command pools
    VkCommandPool GraphicsCommandPool = VK_NULL_HANDLE;
    VkCommandPool ComputeCommandPool = VK_NULL_HANDLE;
    VkCommandPool TransferCommandPool = VK_NULL_HANDLE;

    // Depth resources
    VkFormat DepthFormat = VK_FORMAT_UNDEFINED;
    VkImage DepthImage = VK_NULL_HANDLE;
    VkDeviceMemory DepthImageMemory = VK_NULL_HANDLE;
    VkImageView DepthImageView = VK_NULL_HANDLE;

    // Sync objects
    std::vector<VkSemaphore> ImageAvailableSemaphores;
    std::vector<VkSemaphore> RenderFinishedSemaphores;
    std::vector<VkFence> InFlightFences;
    std::vector<VkFence> ImagesInFlight;
    size_t CurrentFrame = 0;

    // Render pass & pipeline
    VkRenderPass RenderPass = VK_NULL_HANDLE;
    VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
    VkPipeline GraphicsPipeline = VK_NULL_HANDLE;

    // Framebuffers
    std::vector<VkFramebuffer> SwapChainFramebuffers;

    // Command buffers
    std::vector<VkCommandBuffer> CommandBuffers;

    // Settings
    int MaxFramesInFlight = 3;
    bool EnableVsync = true;
    bool EnableDepthTest = true;
    bool EnableMSAA = false;
    VkSampleCountFlagBits MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

    VulkanContextInfo () = default;

    // Helper methods
    void Clear ();
    bool IsSwapChainValid () const { return SwapChain != VK_NULL_HANDLE; }
    bool IsDeviceValid () const { return Device != VK_NULL_HANDLE; }
    };

class VulkanContext
    {
    public:
        VulkanContext ();
        ~VulkanContext ();
        bool Initialize ( VulkanContextInfo * info, GLFWwindow * inWindow );
        void Shutdown ();
        void Render ( const RenderScene & scene );
        VkInstance GetInstance () const;
        VkSurfaceKHR GetSurface () const { return Surface; }

    private:
        bool StartFrame ( VkCommandBuffer & cmdBuffer, const RenderScene & scene );
        bool EndFrame ( VkCommandBuffer & cmdBuffer );

        VulkanContextInfo ContextInfo {};
        std::unique_ptr<InstanceVK> VulkanInstance = nullptr;
        std::unique_ptr<VulkanDevice> m_Device = nullptr;
        VkSurfaceKHR Surface = VK_NULL_HANDLE;
        GLFWwindow * Window = nullptr;
    };