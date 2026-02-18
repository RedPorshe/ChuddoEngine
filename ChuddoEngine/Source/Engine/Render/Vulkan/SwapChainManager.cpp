#include "Render/Vulkan/SwapChainManager.h"
#include "Render/Vulkan/VulkanDevice.h"
#include <vulkan/vulkan.h>
#include <vector>

bool SwapChainManager::Initialize(VulkanContextInfo &info, VulkanDevice * device)
{
    // Create swapchain
    if (!device)
        return false;

    // Query swapchain support from device
    SwapChainSupportDetails details = device->QuerySwapChainSupport(info.Surface);
    VkSurfaceFormatKHR surfaceFormat = details.ChooseFormat();
    VkPresentModeKHR presentMode = details.ChoosePresentMode(info.EnableVsync);
    VkExtent2D extent = details.ChooseExtent(info.WindowWidth, info.WindowHeight);

    uint32_t imageCount = details.Capabilities.minImageCount + 1;
    if (details.Capabilities.maxImageCount > 0 && imageCount > details.Capabilities.maxImageCount)
        imageCount = details.Capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = info.Surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto indices = device->GetQueueIndices();
    uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

    if (indices.GraphicsFamily != indices.PresentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = details.Capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(info.Device, &createInfo, nullptr, &info.SwapChain) != VK_SUCCESS)
        return false;

    // Retrieve images
    uint32_t swapImageCount = 0;
    vkGetSwapchainImagesKHR(info.Device, info.SwapChain, &swapImageCount, nullptr);
    info.SwapChainImages.resize(swapImageCount);
    vkGetSwapchainImagesKHR(info.Device, info.SwapChain, &swapImageCount, info.SwapChainImages.data());

    info.SwapChainImageFormat = surfaceFormat.format;
    info.SwapChainExtent = extent;

    // Create image views
    info.SwapChainImageViews.resize(info.SwapChainImages.size());
    for (size_t i = 0; i < info.SwapChainImages.size(); ++i)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = info.SwapChainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = info.SwapChainImageFormat;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(info.Device, &viewInfo, nullptr, &info.SwapChainImageViews[i]) != VK_SUCCESS)
            return false;
    }

    // Note: render pass, pipeline layout and framebuffers are created later when depth/MSAA images are available
    // This Initialize only creates swapchain and image views.
    // After creating swapchain images and image views, we might need to recreate depth/msaa resources
    // ImageManager will handle depth and MSAA images; caller should ensure ImageManager initialized.

    return true;
}

void SwapChainManager::Shutdown(VulkanContextInfo &info)
{
    for (auto fb : info.SwapChainFramebuffers)
    {
        if (fb != VK_NULL_HANDLE)
            vkDestroyFramebuffer(info.Device, fb, nullptr);
    }
    info.SwapChainFramebuffers.clear();

    if (info.PipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(info.Device, info.PipelineLayout, nullptr);
        info.PipelineLayout = VK_NULL_HANDLE;
    }

    if (info.RenderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(info.Device, info.RenderPass, nullptr);
        info.RenderPass = VK_NULL_HANDLE;
    }

    for (auto view : info.SwapChainImageViews)
    {
        if (view != VK_NULL_HANDLE)
            vkDestroyImageView(info.Device, view, nullptr);
    }
    info.SwapChainImageViews.clear();

    if (info.SwapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(info.Device, info.SwapChain, nullptr);
        info.SwapChain = VK_NULL_HANDLE;
    }

    info.SwapChainImages.clear();
}

bool SwapChainManager::CreateFramebuffers(VulkanContextInfo &info)
{
    // Destroy old framebuffers if present
    for (auto fb : info.SwapChainFramebuffers)
    {
        if (fb != VK_NULL_HANDLE)
            vkDestroyFramebuffer(info.Device, fb, nullptr);
    }
    info.SwapChainFramebuffers.clear();

    info.SwapChainFramebuffers.resize(info.SwapChainImageViews.size());
    for (size_t i = 0; i < info.SwapChainImageViews.size(); ++i)
    {
        // If render pass not created yet, don't attempt to create framebuffers - leave placeholders
        if (info.RenderPass == VK_NULL_HANDLE)
        {
            info.SwapChainFramebuffers[i] = VK_NULL_HANDLE;
            continue;
        }

        std::vector<VkImageView> attachments;
        // Color attachment first
        attachments.push_back(info.SwapChainImageViews[i]);
        // Depth attachment if available (must match render pass attachment order)
        if (info.DepthImageView != VK_NULL_HANDLE)
            attachments.push_back(info.DepthImageView);

        VkFramebufferCreateInfo fbInfo{};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = info.RenderPass;
        fbInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        fbInfo.pAttachments = attachments.data();
        fbInfo.width = info.SwapChainExtent.width;
        fbInfo.height = info.SwapChainExtent.height;
        fbInfo.layers = 1;

        if (vkCreateFramebuffer(info.Device, &fbInfo, nullptr, &info.SwapChainFramebuffers[i]) != VK_SUCCESS)
            return false;
    }

    return true;
}
