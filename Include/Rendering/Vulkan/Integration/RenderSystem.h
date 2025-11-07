#pragma once
#include "Core/AppInfo.h"
#include "Rendering/Data/RenderData.h"

namespace CE
{
  // Высокоуровневый интерфейс для приложения
  class RenderSystem
  {
   public:
    RenderSystem(AppInfo* info);
    ~RenderSystem();

    // Public API для Application
    void Initialize();
    void Shutdown();
    void DrawFrame(const FrameRenderData& renderData);
    bool ShouldClose() const;

    // Геттеры состояния
    bool IsInitialized() const
    {
      return m_initialized;
    }
    int GetWidth() const
    {
      return m_info->Width;
    }
    int GetHeight() const
    {
      return m_info->Height;
    }

   private:
    AppInfo* m_info = nullptr;
    bool m_initialized = false;
    class VulkanContext* m_vulkanContext = nullptr;
  };
}  // namespace CE