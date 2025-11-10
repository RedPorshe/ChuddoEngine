#include "Rendering/Vulkan/Integration/RenderSystem.h"

#include "Rendering/Vulkan/Core/VulkanContext.h"

namespace CE
{
  RenderSystem::RenderSystem(AppInfo* info) : m_info{info}
  {
    m_vulkanContext = new VulkanContext(m_info);
  }

  RenderSystem::~RenderSystem()
  {
    Shutdown();
  }

  void RenderSystem::Initialize()
  {
    if (m_initialized)
      return;

    CE_CORE_DEBUG("Initializing RenderSystem...");
    m_vulkanContext->Initialize();
    m_initialized = true;
    CE_CORE_DEBUG("RenderSystem initialized");
  }

  void RenderSystem::Shutdown()
  {
    if (!m_initialized)
      return;

    CE_CORE_DEBUG("Shutting down RenderSystem...");
    m_vulkanContext->Shutdown();
    m_initialized = false;
  }

  void RenderSystem::DrawFrame(const FrameRenderData& renderData)
  {
    if (!m_initialized)
      return;
    m_vulkanContext->DrawFrame(renderData);
  }

  bool RenderSystem::ShouldClose() const
  {
    return m_vulkanContext ? m_vulkanContext->ShouldClose() : true;
  }
}  // namespace CE
void CE::RenderSystem::PollEvents()
{
  m_vulkanContext->Pollevents();
}
// namespace CE