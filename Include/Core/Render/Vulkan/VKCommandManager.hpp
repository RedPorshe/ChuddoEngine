#pragma once
#include "Core/Render/Vulkan/VKContext.hpp"

namespace CE
{
  class VKCommandManager
  {
   public:
    VKCommandManager(VKContext& pContext);

    bool Create();
    void Destroy();
    bool AllocateCommandBuffers();

   private:
    VKContext& m_Context;
  };
}  // namespace CE