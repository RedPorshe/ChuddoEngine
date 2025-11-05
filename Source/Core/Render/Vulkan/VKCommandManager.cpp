

#include "Core/Render/Vulkan/VKCommandManager.hpp"

#include "Core/Render/Vulkan/VKContext.hpp"
#include "Core/Render/Vulkan/Vkfuncs.hpp"

namespace CE
{
  VKCommandManager::VKCommandManager(VKContext& pContext) : m_Context{pContext}
  {
  }

  bool VKCommandManager::Create()
  {
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_Context.physDevice, m_Context);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(m_Context.Device, &poolInfo, nullptr, &m_Context.CommandPool) != VK_SUCCESS)
    {
      return false;
    }

    return AllocateCommandBuffers();
  }

  void VKCommandManager::Destroy()
  {
    vkDestroyCommandPool(m_Context.Device, m_Context.CommandPool, nullptr);
  }

  bool VKCommandManager::AllocateCommandBuffers()
  {
    m_Context.CommandBuffers.resize(m_Context.MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_Context.CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_Context.CommandBuffers.size();

    if (vkAllocateCommandBuffers(m_Context.Device, &allocInfo, m_Context.CommandBuffers.data()) != VK_SUCCESS)
    {
      return false;
    }

    return true;
  }
}  // namespace CE