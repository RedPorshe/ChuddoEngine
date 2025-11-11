#pragma once
#include "Core/AppInfo.h"
#include "Core/Rendering/Data/RenderData.h"
#include "Core/Rendering/Vulkan/Core/VulkanContext.h"

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

    GLFWwindow* GetWindow() const
    {
      return m_vulkanContext->GetWindow();
    }
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
    void PollEvents();

   private:
    AppInfo* m_info = nullptr;
    bool m_initialized = false;
    VulkanContext* m_vulkanContext = nullptr;
  };
}  // namespace CE