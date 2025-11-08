#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

#include "CoreMinimal.h"
#include "Rendering/Data/Vertex.h"
#include "Rendering/Vulkan/DeviceManager.h"
#include "Rendering/Vulkan/VulkanUtils.h"
#include "vulkan/vulkan.h"

namespace CE
{
  // Типы буферов
  enum class BufferType
  {
    VERTEX,
    INDEX,
    UNIFORM,
    STAGING
  };

  // Информация о буфере
  struct BufferInfo
  {
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize size = 0;
    BufferType type;
    void* mappedData = nullptr;
  };

  class BufferManager
  {
   public:
    BufferManager(std::shared_ptr<DeviceManager> deviceManager);
    ~BufferManager();

    // Удаляем копирование
    BufferManager(const BufferManager&) = delete;
    BufferManager& operator=(const BufferManager&) = delete;

    bool Initialize();
    void Shutdown();

    // Создание буферов
    bool CreateVertexBuffer(const std::string& name, const std::vector<Vertex>& vertices);
    bool CreateIndexBuffer(const std::string& name, const std::vector<uint32_t>& indices);
    bool CreateUniformBuffer(const std::string& name, VkDeviceSize size);
    bool CreateStagingBuffer(const std::string& name, VkDeviceSize size);

    // Обновление данных буферов
    bool UpdateVertexBuffer(const std::string& name, const std::vector<Vertex>& vertices);
    bool UpdateIndexBuffer(const std::string& name, const std::vector<uint32_t>& indices);
    bool UpdateUniformBuffer(const std::string& name, const void* data, VkDeviceSize size);

    // Копирование данных между буферами
    void CopyBuffer(const std::string& srcName, const std::string& dstName, VkDeviceSize size);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);  // Добавлен перегруженный метод

    // Получение буферов
    VkBuffer GetBuffer(const std::string& name) const;
    VkDeviceSize GetBufferSize(const std::string& name) const;
    BufferType GetBufferType(const std::string& name) const;
    VkDescriptorBufferInfo GetBufferInfo(const std::string& name) const;

    // Управление памятью
    void* MapBuffer(const std::string& name);
    void UnmapBuffer(const std::string& name);

    // Удаление буферов
    void DestroyBuffer(const std::string& name);
    void DestroyAllBuffers();

   private:
    bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties, VkBuffer& buffer,
                      VkDeviceMemory& bufferMemory);
    void DestroyBuffer(BufferInfo& bufferInfo);

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

   private:
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::unordered_map<std::string, BufferInfo> m_buffers;

    VkCommandPool m_commandPool = VK_NULL_HANDLE;
  };
}  // namespace CE