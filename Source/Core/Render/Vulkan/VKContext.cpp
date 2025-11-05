#include "Core/Render/Vulkan/VKContext.hpp"

void CE::VKContext::Cleanup()
{
  vkDeviceWaitIdle(Device);
}