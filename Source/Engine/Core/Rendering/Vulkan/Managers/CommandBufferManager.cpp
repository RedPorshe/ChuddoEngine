#include "Engine/Core/Rendering/Vulkan/Managers/CommandBufferManager.h"

#include <stdexcept>

namespace CE
{
  CommandBufferManager::CommandBufferManager(std::shared_ptr<DeviceManager> deviceManager)
      : m_deviceManager(deviceManager)
  {
  }

  CommandBufferManager::~CommandBufferManager()
  {
    Shutdown();
  }

  bool CommandBufferManager::Initialize()
  {
    CE_RENDER_DEBUG("Initializing CommandBufferManager...");

    if (!CreateCommandPool())
    {
      CE_RENDER_ERROR("Failed to create command pool");
      return false;
    }

    // Получаем индекс семейства очередей графики
    QueueFamilyIndices indices = m_deviceManager->FindQueueFamilies(
        m_deviceManager->GetPhysicalDevice(), VK_NULL_HANDLE);
    m_graphicsQueueFamily = indices.graphicsFamily;

    CE_RENDER_DEBUG("CommandBufferManager initialized successfully");
    return true;
  }

  void CommandBufferManager::Shutdown()
  {
    if (!m_commandBuffers.empty())
    {
      vkFreeCommandBuffers(m_deviceManager->GetDevice(), m_commandPool,
                           static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
      m_commandBuffers.clear();
      CE_RENDER_DEBUG("Command buffers freed");
    }

    DestroyCommandPool();
    CE_RENDER_DEBUG("CommandBufferManager shutdown complete");
  }

  bool CommandBufferManager::CreateCommandBuffers(uint32_t count)
  {
    if (count == 0)
    {
      CE_RENDER_ERROR("Cannot create 0 command buffers");
      return false;
    }

    // Освобождаем существующие буферы
    if (!m_commandBuffers.empty())
    {
      vkFreeCommandBuffers(m_deviceManager->GetDevice(), m_commandPool,
                           static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
      m_commandBuffers.clear();
    }

    m_commandBuffers.resize(count);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    VkResult result = vkAllocateCommandBuffers(m_deviceManager->GetDevice(), &allocInfo, m_commandBuffers.data());
    VK_CHECK(result, "Failed to allocate command buffers");

    CE_RENDER_DEBUG("Created ", count, " command buffers");
    return true;
  }

  void CommandBufferManager::BeginRecording(uint32_t commandBufferIndex)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VkResult result = vkBeginCommandBuffer(m_commandBuffers[commandBufferIndex], &beginInfo);
    VK_CHECK(result, "Failed to begin recording command buffer");
  }

  void CommandBufferManager::EndRecording(uint32_t commandBufferIndex)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    VkResult result = vkEndCommandBuffer(m_commandBuffers[commandBufferIndex]);
    VK_CHECK(result, "Failed to end recording command buffer");
  }

  void CommandBufferManager::BeginRenderPass(uint32_t commandBufferIndex, VkRenderPass renderPass,
                                             VkFramebuffer framebuffer, VkExtent2D extent,
                                             const std::vector<VkClearValue>& clearValues)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_commandBuffers[commandBufferIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  }

  void CommandBufferManager::EndRenderPass(uint32_t commandBufferIndex)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    vkCmdEndRenderPass(m_commandBuffers[commandBufferIndex]);
  }

  void CommandBufferManager::BindPipeline(uint32_t commandBufferIndex, VkPipeline pipeline, VkPipelineBindPoint bindPoint)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    vkCmdBindPipeline(m_commandBuffers[commandBufferIndex], bindPoint, pipeline);
  }

  void CommandBufferManager::BindDescriptorSets(uint32_t commandBufferIndex, VkPipelineLayout layout,
                                                uint32_t firstSet, const std::vector<VkDescriptorSet>& descriptorSets)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    vkCmdBindDescriptorSets(m_commandBuffers[commandBufferIndex],
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            firstSet,
                            static_cast<uint32_t>(descriptorSets.size()),
                            descriptorSets.data(),
                            0, nullptr);
  }

  void CommandBufferManager::BindVertexBuffers(uint32_t commandBufferIndex, uint32_t firstBinding,
                                               const std::vector<VkBuffer>& buffers, const std::vector<VkDeviceSize>& offsets)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    if (buffers.size() != offsets.size())
    {
      CE_RENDER_ERROR("Buffers and offsets size mismatch");
      return;
    }

    vkCmdBindVertexBuffers(m_commandBuffers[commandBufferIndex],
                           firstBinding,
                           static_cast<uint32_t>(buffers.size()),
                           buffers.data(),
                           offsets.data());
  }

  void CommandBufferManager::BindIndexBuffer(uint32_t commandBufferIndex, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    vkCmdBindIndexBuffer(m_commandBuffers[commandBufferIndex], buffer, offset, indexType);
  }

  void CommandBufferManager::Draw(uint32_t commandBufferIndex, uint32_t vertexCount, uint32_t instanceCount,
                                  uint32_t firstVertex, uint32_t firstInstance)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    vkCmdDraw(m_commandBuffers[commandBufferIndex], vertexCount, instanceCount, firstVertex, firstInstance);
  }

  void CommandBufferManager::DrawIndexed(uint32_t commandBufferIndex, uint32_t indexCount, uint32_t instanceCount,
                                         uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    vkCmdDrawIndexed(m_commandBuffers[commandBufferIndex], indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
  }

  void CommandBufferManager::SetViewport(uint32_t commandBufferIndex, const VkViewport& viewport)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    vkCmdSetViewport(m_commandBuffers[commandBufferIndex], 0, 1, &viewport);
  }

  void CommandBufferManager::SetScissor(uint32_t commandBufferIndex, const VkRect2D& scissor)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    vkCmdSetScissor(m_commandBuffers[commandBufferIndex], 0, 1, &scissor);
  }

  void CommandBufferManager::SubmitToGraphicsQueue(uint32_t commandBufferIndex, VkSemaphore waitSemaphore,
                                                   VkPipelineStageFlags waitStage, VkSemaphore signalSemaphore,
                                                   VkFence fence)
  {
    if (commandBufferIndex >= m_commandBuffers.size())
    {
      CE_RENDER_ERROR("Invalid command buffer index: ", commandBufferIndex);
      return;
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    if (waitSemaphore != VK_NULL_HANDLE)
    {
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = &waitSemaphore;
      submitInfo.pWaitDstStageMask = &waitStage;
    }

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[commandBufferIndex];

    if (signalSemaphore != VK_NULL_HANDLE)
    {
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = &signalSemaphore;
    }

    VkResult result = vkQueueSubmit(m_deviceManager->GetGraphicsQueue(), 1, &submitInfo, fence);
    VK_CHECK(result, "Failed to submit command buffer to graphics queue");
  }

  VkCommandBuffer CommandBufferManager::GetCommandBuffer(uint32_t index) const
  {
    return index < m_commandBuffers.size() ? m_commandBuffers[index] : VK_NULL_HANDLE;
  }

  VkCommandBuffer CommandBufferManager::BeginSingleTimeCommands()
  {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_deviceManager->GetDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
  }

  void CommandBufferManager::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
  {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_deviceManager->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_deviceManager->GetGraphicsQueue());

    vkFreeCommandBuffers(m_deviceManager->GetDevice(), m_commandPool, 1, &commandBuffer);
  }

  bool CommandBufferManager::CreateCommandPool()
  {
    QueueFamilyIndices queueFamilyIndices = m_deviceManager->FindQueueFamilies(
        m_deviceManager->GetPhysicalDevice(), VK_NULL_HANDLE);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult result = vkCreateCommandPool(m_deviceManager->GetDevice(), &poolInfo, nullptr, &m_commandPool);
    VK_CHECK(result, "Failed to create command pool");

    return true;
  }

  void CommandBufferManager::DestroyCommandPool()
  {
    if (m_commandPool != VK_NULL_HANDLE)
    {
      vkDestroyCommandPool(m_deviceManager->GetDevice(), m_commandPool, nullptr);
      m_commandPool = VK_NULL_HANDLE;
      CE_RENDER_DEBUG("Command pool destroyed");
    }
  }
}  // namespace CE