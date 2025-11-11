#pragma once
#include <memory>
#include <vector>

#include "Core/Rendering/Vulkan/Managers/DeviceManager.h"
#include "Core/Rendering/Vulkan/Managers/SwapchainManager.h"
#include "CoreMinimal.h"
#include "vulkan/vulkan.h"

namespace CE
{
  class CommandBufferManager
  {
   public:
    CommandBufferManager(std::shared_ptr<DeviceManager> deviceManager);
    ~CommandBufferManager();

    // Удаляем копирование
    CommandBufferManager(const CommandBufferManager&) = delete;
    CommandBufferManager& operator=(const CommandBufferManager&) = delete;

    bool Initialize();
    void Shutdown();

    // Создание командных буферов
    bool CreateCommandBuffers(uint32_t count);

    // Начало/конец записи команд
    void BeginRecording(uint32_t commandBufferIndex);
    void EndRecording(uint32_t commandBufferIndex);

    // Начало рендер пасса
    void BeginRenderPass(uint32_t commandBufferIndex, VkRenderPass renderPass,
                         VkFramebuffer framebuffer, VkExtent2D extent,
                         const std::vector<VkClearValue>& clearValues);
    void EndRenderPass(uint32_t commandBufferIndex);

    // Привязка пайплайна и дескрипторов
    void BindPipeline(uint32_t commandBufferIndex, VkPipeline pipeline, VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);
    void BindDescriptorSets(uint32_t commandBufferIndex, VkPipelineLayout layout,
                            uint32_t firstSet, const std::vector<VkDescriptorSet>& descriptorSets);

    // Привязка вершинных и индексных буферов
    void BindVertexBuffers(uint32_t commandBufferIndex, uint32_t firstBinding,
                           const std::vector<VkBuffer>& buffers, const std::vector<VkDeviceSize>& offsets);
    void BindIndexBuffer(uint32_t commandBufferIndex, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);

    // Рисование
    void Draw(uint32_t commandBufferIndex, uint32_t vertexCount, uint32_t instanceCount = 1,
              uint32_t firstVertex = 0, uint32_t firstInstance = 0);
    void DrawIndexed(uint32_t commandBufferIndex, uint32_t indexCount, uint32_t instanceCount = 1,
                     uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0);

    // Установка viewport и scissor
    void SetViewport(uint32_t commandBufferIndex, const VkViewport& viewport);
    void SetScissor(uint32_t commandBufferIndex, const VkRect2D& scissor);

    // Отправка на выполнение
    void SubmitToGraphicsQueue(uint32_t commandBufferIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE,
                               VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                               VkSemaphore signalSemaphore = VK_NULL_HANDLE,
                               VkFence fence = VK_NULL_HANDLE);

    // Получение командных буферов
    VkCommandBuffer GetCommandBuffer(uint32_t index) const;
    const std::vector<VkCommandBuffer>& GetCommandBuffers() const
    {
      return m_commandBuffers;
    }

    // Создание одноразового командного буфера
    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

   private:
    bool CreateCommandPool();
    void DestroyCommandPool();

   private:
    std::shared_ptr<DeviceManager> m_deviceManager;

    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;

    uint32_t m_graphicsQueueFamily = UINT32_MAX;
  };
}  // namespace CE