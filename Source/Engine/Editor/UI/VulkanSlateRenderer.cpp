#include "Engine/Editor/UI/VulkanSlateRenderer.h"
#include "Engine/Core/Rendering/Vulkan/Managers/PipelineManager.h"
#include "Engine/Core/Rendering/Vulkan/Managers/BufferManager.h"
#include "Engine/Core/Rendering/Vulkan/Managers/DescriptorManager.h"
#include "Engine/Core/Rendering/Vulkan/Utils/VulkanUtils.h"
#include <vulkan/vulkan.h>
#include <stdexcept>

VulkanSlateRenderer::VulkanSlateRenderer(std::shared_ptr<VulkanContext> vulkanContext)
    : m_vulkanContext(vulkanContext)
{
}

VulkanSlateRenderer::~VulkanSlateRenderer()
{
    Shutdown();
}

void VulkanSlateRenderer::Initialize()
{
    CreateUIPipeline();
    CreateUIBuffers();
}

void VulkanSlateRenderer::Shutdown()
{
    VkDevice device = m_vulkanContext->GetDevice();

    if (device != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(device);

        if (m_uiIndexBuffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, m_uiIndexBuffer, nullptr);
            m_uiIndexBuffer = VK_NULL_HANDLE;
        }

        if (m_uiIndexBufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, m_uiIndexBufferMemory, nullptr);
            m_uiIndexBufferMemory = VK_NULL_HANDLE;
        }

        if (m_uiVertexBuffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, m_uiVertexBuffer, nullptr);
            m_uiVertexBuffer = VK_NULL_HANDLE;
        }

        if (m_uiVertexBufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, m_uiVertexBufferMemory, nullptr);
            m_uiVertexBufferMemory = VK_NULL_HANDLE;
        }

        if (m_uiDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(device, m_uiDescriptorPool, nullptr);
            m_uiDescriptorPool = VK_NULL_HANDLE;
        }

        if (m_uiDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, m_uiDescriptorSetLayout, nullptr);
            m_uiDescriptorSetLayout = VK_NULL_HANDLE;
        }

        if (m_uiPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, m_uiPipeline, nullptr);
            m_uiPipeline = VK_NULL_HANDLE;
        }

        if (m_uiPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, m_uiPipelineLayout, nullptr);
            m_uiPipelineLayout = VK_NULL_HANDLE;
        }
    }
}

void VulkanSlateRenderer::BeginFrame()
{
    // Prepare for UI rendering
    // This could involve setting up render pass, viewport, etc.
}

void VulkanSlateRenderer::EndFrame()
{
    // Finalize UI rendering
    // Submit UI draw commands
}

void VulkanSlateRenderer::DrawRect(const FVector2D& Position, const FVector2D& Size, const FLinearColor& Color)
{
    // Add rectangle vertices to the vertex buffer
    UIVertex topLeft = {Position, Color};
    UIVertex topRight = {FVector2D(Position.x + Size.x, Position.y), Color};
    UIVertex bottomRight = {FVector2D(Position.x + Size.x, Position.y + Size.y), Color};
    UIVertex bottomLeft = {FVector2D(Position.x, Position.y + Size.y), Color};

    uint32_t startIndex = static_cast<uint32_t>(m_uiVertices.size());
    m_uiVertices.push_back(topLeft);
    m_uiVertices.push_back(topRight);
    m_uiVertices.push_back(bottomRight);
    m_uiVertices.push_back(bottomLeft);

    // Add indices for two triangles
    m_uiIndices.push_back(startIndex);
    m_uiIndices.push_back(startIndex + 1);
    m_uiIndices.push_back(startIndex + 2);
    m_uiIndices.push_back(startIndex);
    m_uiIndices.push_back(startIndex + 2);
    m_uiIndices.push_back(startIndex + 3);
}

void VulkanSlateRenderer::DrawText(const FString& Text, const FVector2D& Position, const FLinearColor& Color)
{
    // TODO: Implement text rendering
    // This is a placeholder - actual implementation would require:
    // 1. Font loading and glyph data
    // 2. Text layout and positioning
    // 3. Generating textured quads for each character

    // For now, just draw a colored rectangle as placeholder
    DrawRect(Position, FVector2D(100.0f, 20.0f), Color);
}

void VulkanSlateRenderer::CreateUIPipeline()
{
    VkDevice device = m_vulkanContext->GetDevice();

    // Create descriptor set layout for UI
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_uiDescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create UI descriptor set layout!");
    }

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_uiDescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_uiPipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create UI pipeline layout!");
    }

    // TODO: Create actual UI shaders and pipeline
    // For now, this is a placeholder - we would need to:
    // 1. Load UI vertex and fragment shaders
    // 2. Create shader modules
    // 3. Set up pipeline create info with proper vertex input, etc.
    // 4. Create the graphics pipeline

    // Placeholder - using basic pipeline creation
    // This would be replaced with proper UI pipeline setup
}

void VulkanSlateRenderer::CreateUIBuffers()
{
    VkDevice device = m_vulkanContext->GetDevice();
    VkPhysicalDevice physicalDevice = m_vulkanContext->GetPhysicalDevice();

    // Create vertex buffer
    VkDeviceSize bufferSize = sizeof(UIVertex) * 1024; // Initial capacity for 1024 vertices

    VulkanUtils::CreateBuffer(physicalDevice, device, bufferSize,
                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              m_uiVertexBuffer, m_uiVertexBufferMemory);

    // Create index buffer
    VkDeviceSize indexBufferSize = sizeof(uint32_t) * 1536; // Initial capacity for 512 triangles

    VulkanUtils::CreateBuffer(physicalDevice, device, indexBufferSize,
                              VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              m_uiIndexBuffer, m_uiIndexBufferMemory);

    // Create descriptor pool
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_uiDescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create UI descriptor pool!");
    }

    // Allocate descriptor set
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_uiDescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_uiDescriptorSetLayout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &m_uiDescriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate UI descriptor set!");
    }
}

void VulkanSlateRenderer::UpdateUIBuffers()
{
    VkDevice device = m_vulkanContext->GetDevice();

    // Update vertex buffer
    void* data;
    vkMapMemory(device, m_uiVertexBufferMemory, 0, sizeof(UIVertex) * m_uiVertices.size(), 0, &data);
    memcpy(data, m_uiVertices.data(), sizeof(UIVertex) * m_uiVertices.size());
    vkUnmapMemory(device, m_uiVertexBufferMemory);

    // Update index buffer
    vkMapMemory(device, m_uiIndexBufferMemory, 0, sizeof(uint32_t) * m_uiIndices.size(), 0, &data);
    memcpy(data, m_uiIndices.data(), sizeof(uint32_t) * m_uiIndices.size());
    vkUnmapMemory(device, m_uiIndexBufferMemory);
}
