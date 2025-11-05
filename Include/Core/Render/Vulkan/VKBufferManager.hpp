#pragma once
#include <unordered_map>

#include "Core/Render/Vulkan/VKContext.hpp"
#include "Core/Render/vertex.hpp"

namespace CE
{
  class VKBufferManager
  {
   public:
    VKBufferManager(VKContext& pContext);
    ~VKBufferManager();

    VkDeviceMemory GetUniformBufferMemory(const std::string& name, uint32_t frameIndex);
    // {
    //   auto it = m_buffers.find(name);
    //   if (it != m_buffers.end() && frameIndex < it->second.uniformMemories.size())
    //   {
    //     return it->second.uniformMemories[frameIndex];
    //   }

    //   return VK_NULL_HANDLE;
    // }

    bool UniformBufferExists(const std::string& name) const;
    // {
    // return m_buffers.find(name) != m_buffers.end();
    // }

    size_t GetUniformBufferSize(const std::string& name) const;
    // {
    //   auto it = m_buffers.find(name);
    //   if (it != m_buffers.end())
    //   {
    //     return it->second.size;
    //   }
    //   return 0;
    // }

    // Vertex buffers
    bool CreateVertexBuffer(const std::string& name, const std::vector<Vertex>& vertices);
    VkBuffer GetVertexBuffer(const std::string& name);

    // Index buffers
    bool CreateIndexBuffer(const std::string& name, const std::vector<uint32_t>& indices);
    VkBuffer GetIndexBuffer(const std::string& name);

    // Uniform buffers
    bool CreateUniformBuffers(const std::string& name, size_t dataSize);
    VkBuffer GetUniformBuffer(const std::string& name, uint32_t frameIndex);

    void DestroyBuffer(const std::string& name);
    void DestroyAll();
    friend class VKUniformManager;

   private:
    struct BufferInfo
    {
      VkBuffer buffer = VK_NULL_HANDLE;
      VkDeviceMemory memory = VK_NULL_HANDLE;
      size_t size = 0;
      std::vector<VkBuffer> uniformBuffers;
      std::vector<VkDeviceMemory> uniformMemories;
    };

    VKContext& m_Context;
    std::unordered_map<std::string, BufferInfo> m_buffers;

    bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties, VkBuffer& buffer,
                      VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  };
}  // namespace CE