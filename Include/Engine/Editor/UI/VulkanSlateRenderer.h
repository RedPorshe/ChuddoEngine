#pragma once

#include "Engine/Editor/UI/SlateCore.h"
#include "Engine/Core/Rendering/Vulkan/Core/VulkanContext.h"
#include <memory>
#include <vector>

struct VkPipeline_T;
struct VkPipelineLayout_T;
struct VkDescriptorSetLayout_T;
struct VkDescriptorPool_T;
struct VkDescriptorSet_T;
struct VkBuffer_T;
struct VkDeviceMemory_T;

class VulkanSlateRenderer : public CSlateRenderer
{
public:
    VulkanSlateRenderer(std::shared_ptr<VulkanContext> vulkanContext);
    ~VulkanSlateRenderer() override;

    void Initialize();
    void Shutdown();

    void BeginFrame() override;
    void EndFrame() override;

    void DrawRect(const FVector2D& Position, const FVector2D& Size, const FLinearColor& Color) override;
    void DrawText(const FString& Text, const FVector2D& Position, const FLinearColor& Color) override;

private:
    void CreateUIPipeline();
    void CreateUIBuffers();
    void UpdateUIBuffers();

    std::shared_ptr<VulkanContext> m_vulkanContext;

    VkPipeline m_uiPipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_uiPipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_uiDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_uiDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet m_uiDescriptorSet = VK_NULL_HANDLE;

    VkBuffer m_uiVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_uiVertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer m_uiIndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_uiIndexBufferMemory = VK_NULL_HANDLE;

    struct UIVertex
    {
        FVector2D Position;
        FLinearColor Color;
    };

    std::vector<UIVertex> m_uiVertices;
    std::vector<uint32_t> m_uiIndices;

    // Font data (placeholder for now)
    // TODO: Implement font loading
    // VkImage m_fontTexture = VK_NULL_HANDLE;
    // VkDeviceMemory m_fontTextureMemory = VK_NULL_HANDLE;
    // VkImageView m_fontTextureView = VK_NULL_HANDLE;
    // VkSampler m_fontSampler = VK_NULL_HANDLE;
};
