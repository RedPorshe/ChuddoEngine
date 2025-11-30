#include "Engine/Core/Rendering/Vulkan/Managers/BufferManager.h"

#include <stdexcept>

#include "Engine/Core/Rendering/Data/RenderData.h"


  BufferManager::BufferManager(std::shared_ptr<DeviceManager> deviceManager)
      : m_deviceManager(deviceManager)
  {
  }

  BufferManager::~BufferManager()
  {
    Shutdown();
  }

  bool BufferManager::Initialize()
  {
    RENDER_DEBUG("Initializing BufferManager...");

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = m_deviceManager->getIndices().graphicsFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

    VkResult result = vkCreateCommandPool(m_deviceManager->GetDevice(), &poolInfo, nullptr, &m_commandPool);
    VK_CHECK(result, "Failed to create command pool for buffer manager");

    RENDER_DEBUG("BufferManager initialized successfully");
    return true;
  }

  void BufferManager::Shutdown()
  {
    DestroyAllBuffers();

    if (m_commandPool != VK_NULL_HANDLE)
    {
      vkDestroyCommandPool(m_deviceManager->GetDevice(), m_commandPool, nullptr);
      m_commandPool = VK_NULL_HANDLE;
      RENDER_DEBUG("BufferManager command pool destroyed");
    }

    RENDER_DEBUG("BufferManager shutdown complete");
  }

  VkDescriptorBufferInfo BufferManager::GetBufferInfo(const std::string& name) const
  {
    auto it = m_buffers.find(name);
    if (it != m_buffers.end())
    {
      VkDescriptorBufferInfo bufferInfo{};
      bufferInfo.buffer = it->second.buffer;
      bufferInfo.offset = 0;
      bufferInfo.range = it->second.size;
      return bufferInfo;
    }

    RENDER_ERROR("Buffer '", name, "' not found for descriptor info");
    return VkDescriptorBufferInfo{};
  }

  bool BufferManager::CreateVertexBuffer(const std::string& name, const std::vector<Vertex>& vertices)
  {
    if (!vertices.empty())
    {
      if (m_buffers.find(name) != m_buffers.end())
      {
        RENDER_WARN("Vertex buffer '", name, "' already exists");
        return true;
      }

      VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

      VkBuffer stagingBuffer;
      VkDeviceMemory stagingBufferMemory;
      if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        stagingBuffer, stagingBufferMemory))
      {
        RENDER_ERROR("Failed to create staging buffer for vertex buffer '", name, "'");
        return false;
      }

      void* data;
      vkMapMemory(m_deviceManager->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
      memcpy(data, vertices.data(), (size_t)bufferSize);
      vkUnmapMemory(m_deviceManager->GetDevice(), stagingBufferMemory);

      VkBuffer vertexBuffer;
      VkDeviceMemory vertexBufferMemory;
      if (!CreateBuffer(bufferSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        vertexBuffer, vertexBufferMemory))
      {
        RENDER_ERROR("Failed to create vertex buffer '", name, "'");
        vkDestroyBuffer(m_deviceManager->GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(m_deviceManager->GetDevice(), stagingBufferMemory, nullptr);
        return false;
      }

      CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

      BufferInfo bufferInfo;
      bufferInfo.buffer = vertexBuffer;
      bufferInfo.memory = vertexBufferMemory;
      bufferInfo.size = bufferSize;
      bufferInfo.type = BufferType::VERTEX;
      m_buffers[name] = bufferInfo;

      vkDestroyBuffer(m_deviceManager->GetDevice(), stagingBuffer, nullptr);
      vkFreeMemory(m_deviceManager->GetDevice(), stagingBufferMemory, nullptr);

      return true;
    }
    return false;
  }

  bool BufferManager::CreateIndexBuffer(const std::string& name, const std::vector<uint32_t>& indices)
  {
    if (!indices.empty())
    {
      if (m_buffers.find(name) != m_buffers.end())
      {
        RENDER_WARN("Index buffer '", name, "' already exists");
        return true;
      }

      VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

      VkBuffer stagingBuffer;
      VkDeviceMemory stagingBufferMemory;
      if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        stagingBuffer, stagingBufferMemory))
      {
        RENDER_ERROR("Failed to create staging buffer for index buffer '", name, "'");
        return false;
      }

      void* data;
      vkMapMemory(m_deviceManager->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
      memcpy(data, indices.data(), (size_t)bufferSize);
      vkUnmapMemory(m_deviceManager->GetDevice(), stagingBufferMemory);

      VkBuffer indexBuffer;
      VkDeviceMemory indexBufferMemory;
      if (!CreateBuffer(bufferSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        indexBuffer, indexBufferMemory))
      {
        RENDER_ERROR("Failed to create index buffer '", name, "'");
        vkDestroyBuffer(m_deviceManager->GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(m_deviceManager->GetDevice(), stagingBufferMemory, nullptr);
        return false;
      }

      CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

      BufferInfo bufferInfo;
      bufferInfo.buffer = indexBuffer;
      bufferInfo.memory = indexBufferMemory;
      bufferInfo.size = bufferSize;
      bufferInfo.type = BufferType::INDEX;
      m_buffers[name] = bufferInfo;

      vkDestroyBuffer(m_deviceManager->GetDevice(), stagingBuffer, nullptr);
      vkFreeMemory(m_deviceManager->GetDevice(), stagingBufferMemory, nullptr);

      return true;
    }
    return false;
  }

  bool BufferManager::CreateUniformBuffer(const std::string& name, VkDeviceSize size)
  {
    if (m_buffers.find(name) != m_buffers.end())
    {
      RENDER_WARN("Uniform buffer '", name, "' already exists");
      return true;
    }

    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;
    if (!CreateBuffer(size,
                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniformBuffer, uniformBufferMemory))
    {
      RENDER_ERROR("Failed to create uniform buffer '", name, "'");
      return false;
    }

    BufferInfo bufferInfo;
    bufferInfo.buffer = uniformBuffer;
    bufferInfo.memory = uniformBufferMemory;
    bufferInfo.size = size;
    bufferInfo.type = BufferType::UNIFORM;
    m_buffers[name] = bufferInfo;

    return true;
  }

  bool BufferManager::CreateStagingBuffer(const std::string& name, VkDeviceSize size)
  {
    if (m_buffers.find(name) != m_buffers.end())
    {
      RENDER_WARN("Staging buffer '", name, "' already exists");
      return true;
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    if (!CreateBuffer(size,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      stagingBuffer, stagingBufferMemory))
    {
      RENDER_ERROR("Failed to create staging buffer '", name, "'");
      return false;
    }

    BufferInfo bufferInfo;
    bufferInfo.buffer = stagingBuffer;
    bufferInfo.memory = stagingBufferMemory;
    bufferInfo.size = size;
    bufferInfo.type = BufferType::STAGING;
    m_buffers[name] = bufferInfo;

    RENDER_DEBUG("Created staging buffer '", name, "' with size ", size);
    return true;
  }

  bool BufferManager::UpdateVertexBuffer(const std::string& name, const std::vector<Vertex>& vertices)
  {
    auto it = m_buffers.find(name);
    if (it == m_buffers.end() || it->second.type != BufferType::VERTEX)
    {
      RENDER_ERROR("Vertex buffer '", name, "' not found or wrong type");
      return false;
    }

    VkDeviceSize newSize = sizeof(vertices[0]) * vertices.size();
    if (newSize > it->second.size)
    {
      RENDER_ERROR("New vertex data too large for buffer '", name, "'");
      return false;
    }

    // Создаем временный staging buffer
    std::string stagingName = name + "_staging_temp";
    if (!CreateStagingBuffer(stagingName, newSize))
    {
      return false;
    }

    // Копируем данные в staging buffer
    void* data = MapBuffer(stagingName);
    memcpy(data, vertices.data(), (size_t)newSize);
    UnmapBuffer(stagingName);

    // Копируем в vertex buffer
    CopyBuffer(stagingName, name, newSize);

    // Удаляем временный staging buffer
    DestroyBuffer(stagingName);

    RENDER_DEBUG("Updated vertex buffer '", name, "' with ", vertices.size(), " vertices");
    return true;
  }

  bool BufferManager::UpdateIndexBuffer(const std::string& name, const std::vector<uint32_t>& indices)
  {
    auto it = m_buffers.find(name);
    if (it == m_buffers.end() || it->second.type != BufferType::INDEX)
    {
      RENDER_ERROR("Index buffer '", name, "' not found or wrong type");
      return false;
    }

    VkDeviceSize newSize = sizeof(indices[0]) * indices.size();
    if (newSize > it->second.size)
    {
      RENDER_ERROR("New index data too large for buffer '", name, "'");
      return false;
    }

    // Создаем временный staging buffer
    std::string stagingName = name + "_staging_temp";
    if (!CreateStagingBuffer(stagingName, newSize))
    {
      return false;
    }

    // Копируем данные в staging buffer
    void* data = MapBuffer(stagingName);
    memcpy(data, indices.data(), (size_t)newSize);
    UnmapBuffer(stagingName);

    // Копируем в index buffer
    CopyBuffer(stagingName, name, newSize);

    // Удаляем временный staging buffer
    DestroyBuffer(stagingName);

    RENDER_DEBUG("Updated index buffer '", name, "' with ", indices.size(), " indices");
    return true;
  }

  bool BufferManager::UpdateUniformBuffer(const std::string& name, const void* data, VkDeviceSize size)
  {
    auto it = m_buffers.find(name);
    if (it == m_buffers.end() || it->second.type != BufferType::UNIFORM)
    {
      RENDER_ERROR("Uniform buffer '", name, "' not found or wrong type");
      return false;
    }

    if (size > it->second.size)
    {
      RENDER_ERROR("Data too large for uniform buffer '", name, "'");
      return false;
    }

    void* mappedData;
    vkMapMemory(m_deviceManager->GetDevice(), it->second.memory, 0, size, 0, &mappedData);
    memcpy(mappedData, data, (size_t)size);
    vkUnmapMemory(m_deviceManager->GetDevice(), it->second.memory);
    return true;
  }

  void BufferManager::CopyBuffer(const std::string& srcName, const std::string& dstName, VkDeviceSize size)
  {
    auto srcIt = m_buffers.find(srcName);
    auto dstIt = m_buffers.find(dstName);

    if (srcIt == m_buffers.end() || dstIt == m_buffers.end())
    {
      RENDER_ERROR("Source or destination buffer not found for copy operation");
      return;
    }

    CopyBuffer(srcIt->second.buffer, dstIt->second.buffer, size);
  }

  void BufferManager::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
  {
    VulkanUtils::CopyBuffer(m_deviceManager->GetDevice(), m_commandPool,
                            m_deviceManager->GetGraphicsQueue(),
                            srcBuffer, dstBuffer, size);
  }

  VkBuffer BufferManager::GetBuffer(const std::string& name) const
  {
    auto it = m_buffers.find(name);
    return it != m_buffers.end() ? it->second.buffer : VK_NULL_HANDLE;
  }

  VkDeviceSize BufferManager::GetBufferSize(const std::string& name) const
  {
    auto it = m_buffers.find(name);
    return it != m_buffers.end() ? it->second.size : 0;
  }

  BufferType BufferManager::GetBufferType(const std::string& name) const
  {
    auto it = m_buffers.find(name);
    return it != m_buffers.end() ? it->second.type : BufferType::VERTEX;
  }

  void* BufferManager::MapBuffer(const std::string& name)
  {
    auto it = m_buffers.find(name);
    if (it == m_buffers.end())
    {
      RENDER_ERROR("Buffer '", name, "' not found for mapping");
      return nullptr;
    }

    if (it->second.mappedData)
    {
      RENDER_WARN("Buffer '", name, "' is already mapped");
      return it->second.mappedData;
    }

    vkMapMemory(m_deviceManager->GetDevice(), it->second.memory, 0, it->second.size, 0, &it->second.mappedData);
    return it->second.mappedData;
  }

  void BufferManager::UnmapBuffer(const std::string& name)
  {
    auto it = m_buffers.find(name);
    if (it == m_buffers.end() || !it->second.mappedData)
    {
      return;
    }

    vkUnmapMemory(m_deviceManager->GetDevice(), it->second.memory);
    it->second.mappedData = nullptr;
  }

  void BufferManager::DestroyBuffer(const std::string& name)
  {
    auto it = m_buffers.find(name);
    if (it == m_buffers.end())
    {
      return;
    }

    if (it->second.mappedData)
    {
      UnmapBuffer(name);
    }

    DestroyBuffer(it->second);
    m_buffers.erase(it);
    RENDER_DEBUG("Destroyed buffer '", name, "'");
  }

  void BufferManager::DestroyAllBuffers()
  {
    for (auto& [name, bufferInfo] : m_buffers)
    {
      if (bufferInfo.mappedData)
      {
        vkUnmapMemory(m_deviceManager->GetDevice(), bufferInfo.memory);
      }
      DestroyBuffer(bufferInfo);
      RENDER_DEBUG("Destroyed buffer '", name, "'");
    }
    m_buffers.clear();
  }

  bool BufferManager::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                   VkMemoryPropertyFlags properties, VkBuffer& buffer,
                                   VkDeviceMemory& bufferMemory)
  {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(m_deviceManager->GetDevice(), &bufferInfo, nullptr, &buffer);
    if (result != VK_SUCCESS)
    {
      RENDER_ERROR("Failed to create buffer");
      return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_deviceManager->GetDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(m_deviceManager->GetDevice(), &allocInfo, nullptr, &bufferMemory);
    if (result != VK_SUCCESS)
    {
      RENDER_ERROR("Failed to allocate buffer memory");
      vkDestroyBuffer(m_deviceManager->GetDevice(), buffer, nullptr);
      return false;
    }

    vkBindBufferMemory(m_deviceManager->GetDevice(), buffer, bufferMemory, 0);
    return true;
  }

  void BufferManager::DestroyBuffer(BufferInfo& bufferInfo)
  {
    if (bufferInfo.buffer != VK_NULL_HANDLE)
    {
      vkDestroyBuffer(m_deviceManager->GetDevice(), bufferInfo.buffer, nullptr);
      bufferInfo.buffer = VK_NULL_HANDLE;
    }

    if (bufferInfo.memory != VK_NULL_HANDLE)
    {
      vkFreeMemory(m_deviceManager->GetDevice(), bufferInfo.memory, nullptr);
      bufferInfo.memory = VK_NULL_HANDLE;
    }
  }

  uint32_t BufferManager::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
  {
    return VulkanUtils::FindMemoryType(m_deviceManager->GetPhysicalDevice(), typeFilter, properties);
  }