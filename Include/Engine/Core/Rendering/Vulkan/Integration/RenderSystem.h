#pragma once
#include "Engine/Core/AppInfo.h"
#include "Engine/Core/Rendering/Data/RenderData.h"
#include "Engine/Core/Rendering/Vulkan/Core/VulkanContext.h"

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

    SDL_Window* GetWindow() const
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

    VulkanContext* GetVulkanContext() const
    {
      return m_vulkanContext;
    }

   private:
    AppInfo* m_info = nullptr;
    bool m_initialized = false;
    VulkanContext* m_vulkanContext = nullptr;
  };