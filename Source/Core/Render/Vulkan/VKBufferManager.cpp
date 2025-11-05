#include "Core/Render/Vulkan/VKBufferManager.hpp"

#include <stdexcept>

namespace CE
{
  VKBufferManager::VKBufferManager(VKContext& pContext) : m_Context{pContext}
  {
  }

  VKBufferManager::~VKBufferManager()
  {
    DestroyAll();
  }

  bool VKBufferManager::CreateVertexBuffer(const std::string& name, const std::vector<Vertex>& vertices)
  {
    if (vertices.empty())
    {
      return false;
    }

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      stagingBuffer, stagingBufferMemory))
    {
      return false;
    }

    void* data;
    vkMapMemory(m_Context.Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_Context.Device, stagingBufferMemory);

    BufferInfo bufferInfo;
    if (!CreateBuffer(bufferSize,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      bufferInfo.buffer, bufferInfo.memory))
    {
      vkDestroyBuffer(m_Context.Device, stagingBuffer, nullptr);
      vkFreeMemory(m_Context.Device, stagingBufferMemory, nullptr);

      return false;
    }

    CopyBuffer(stagingBuffer, bufferInfo.buffer, bufferSize);

    vkDestroyBuffer(m_Context.Device, stagingBuffer, nullptr);
    vkFreeMemory(m_Context.Device, stagingBufferMemory, nullptr);

    bufferInfo.size = bufferSize;
    m_buffers[name] = bufferInfo;

    return true;
  }

  bool VKBufferManager::CreateIndexBuffer(const std::string& name, const std::vector<uint32_t>& indices)
  {
    if (indices.empty())
    {
      return false;
    }

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    // Создаем staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      stagingBuffer, stagingBufferMemory))
    {
      return false;
    }

    // Копируем данные в staging buffer
    void* data;
    vkMapMemory(m_Context.Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_Context.Device, stagingBufferMemory);

    // Создаем index buffer
    BufferInfo bufferInfo;
    if (!CreateBuffer(bufferSize,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      bufferInfo.buffer, bufferInfo.memory))
    {
      vkDestroyBuffer(m_Context.Device, stagingBuffer, nullptr);
      vkFreeMemory(m_Context.Device, stagingBufferMemory, nullptr);

      return false;
    }

    // Копируем из staging в index buffer
    CopyBuffer(stagingBuffer, bufferInfo.buffer, bufferSize);

    // Очищаем staging buffer
    vkDestroyBuffer(m_Context.Device, stagingBuffer, nullptr);
    vkFreeMemory(m_Context.Device, stagingBufferMemory, nullptr);

    bufferInfo.size = bufferSize;
    m_buffers[name] = bufferInfo;

    return true;
  }

  bool VKBufferManager::CreateUniformBuffers(const std::string& name, size_t dataSize)
  {
    if (dataSize == 0)
    {
      return false;
    }

    BufferInfo bufferInfo;
    bufferInfo.uniformBuffers.resize(m_Context.MAX_FRAMES_IN_FLIGHT);
    bufferInfo.uniformMemories.resize(m_Context.MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < m_Context.MAX_FRAMES_IN_FLIGHT; i++)
    {
      if (!CreateBuffer(dataSize,
                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        bufferInfo.uniformBuffers[i], bufferInfo.uniformMemories[i]))
      {
        // Cleanup already created buffers
        for (size_t j = 0; j < i; j++)
        {
          vkDestroyBuffer(m_Context.Device, bufferInfo.uniformBuffers[j], nullptr);
          vkFreeMemory(m_Context.Device, bufferInfo.uniformMemories[j], nullptr);
        }

        return false;
      }
    }

    bufferInfo.size = dataSize;
    m_buffers[name] = bufferInfo;

    return true;
  }

  VkBuffer VKBufferManager::GetVertexBuffer(const std::string& name)
  {
    auto it = m_buffers.find(name);
    if (it != m_buffers.end())
    {
      return it->second.buffer;
    }

    return VK_NULL_HANDLE;
  }

  VkBuffer VKBufferManager::GetIndexBuffer(const std::string& name)
  {
    auto it = m_buffers.find(name);
    if (it != m_buffers.end())
    {
      return it->second.buffer;
    }

    return VK_NULL_HANDLE;
  }

  VkBuffer VKBufferManager::GetUniformBuffer(const std::string& name, uint32_t frameIndex)
  {
    auto it = m_buffers.find(name);
    if (it != m_buffers.end() && frameIndex < it->second.uniformBuffers.size())
    {
      return it->second.uniformBuffers[frameIndex];
    }

    return VK_NULL_HANDLE;
  }

  void VKBufferManager::DestroyBuffer(const std::string& name)
  {
    auto it = m_buffers.find(name);
    if (it != m_buffers.end())
    {
      if (it->second.buffer != VK_NULL_HANDLE)
      {
        vkDestroyBuffer(m_Context.Device, it->second.buffer, nullptr);
      }
      if (it->second.memory != VK_NULL_HANDLE)
      {
        vkFreeMemory(m_Context.Device, it->second.memory, nullptr);
      }
      for (size_t i = 0; i < it->second.uniformBuffers.size(); i++)
      {
        if (it->second.uniformBuffers[i] != VK_NULL_HANDLE)
        {
          vkDestroyBuffer(m_Context.Device, it->second.uniformBuffers[i], nullptr);
        }
      }
      for (size_t i = 0; i < it->second.uniformMemories.size(); i++)
      {
        if (it->second.uniformMemories[i] != VK_NULL_HANDLE)
        {
          vkFreeMemory(m_Context.Device, it->second.uniformMemories[i], nullptr);
        }
      }
      m_buffers.erase(it);
    }
  }

  void VKBufferManager::DestroyAll()
  {
    for (auto& [name, bufferInfo] : m_buffers)
    {
      if (bufferInfo.buffer != VK_NULL_HANDLE)
      {
        vkDestroyBuffer(m_Context.Device, bufferInfo.buffer, nullptr);
      }
      if (bufferInfo.memory != VK_NULL_HANDLE)
      {
        vkFreeMemory(m_Context.Device, bufferInfo.memory, nullptr);
      }
      for (size_t i = 0; i < bufferInfo.uniformBuffers.size(); i++)
      {
        if (bufferInfo.uniformBuffers[i] != VK_NULL_HANDLE)
        {
          vkDestroyBuffer(m_Context.Device, bufferInfo.uniformBuffers[i], nullptr);
        }
      }
      for (size_t i = 0; i < bufferInfo.uniformMemories.size(); i++)
      {
        if (bufferInfo.uniformMemories[i] != VK_NULL_HANDLE)
        {
          vkFreeMemory(m_Context.Device, bufferInfo.uniformMemories[i], nullptr);
        }
      }
    }
    m_buffers.clear();
  }

  bool VKBufferManager::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                     VkMemoryPropertyFlags properties, VkBuffer& buffer,
                                     VkDeviceMemory& bufferMemory)
  {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_Context.Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
      return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_Context.Device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_Context.physDevice, &memProperties);

    uint32_t memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
      if ((memRequirements.memoryTypeBits & (1 << i)) &&
          (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
      {
        memoryTypeIndex = i;
        break;
      }
    }

    if (memoryTypeIndex == UINT32_MAX)
    {
      vkDestroyBuffer(m_Context.Device, buffer, nullptr);
      return false;
    }

    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(m_Context.Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
      vkDestroyBuffer(m_Context.Device, buffer, nullptr);
      return false;
    }

    vkBindBufferMemory(m_Context.Device, buffer, bufferMemory, 0);
    return true;
  }

  void VKBufferManager::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
  {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_Context.CommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_Context.Device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_Context.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_Context.graphicsQueue);

    vkFreeCommandBuffers(m_Context.Device, m_Context.CommandPool, 1, &commandBuffer);
  }
}  // namespace CE