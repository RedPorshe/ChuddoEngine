#pragma once
#include "Core/AppInfo.h"
#include "Core/Rendering/Data/RenderData.h"
#include "Core/Rendering/Vulkan/Integration/RenderSystem.h"
#include "CoreMinimal.h"
#include "GamePlay/GameInstance/CEGameInstance.h"

namespace CE
{
  class Application
  {
   public:
    Application(AppInfo* info);
    ~Application();

    void Initialize();
    void Run();
    void Shutdown();

    // Геттеры для доступа извне
    CEGameInstance* GetGameInstance() const
    {
      return m_GameInstance.get();
    }
    float GetDeltaTime() const
    {
      return m_DeltaTime;
    }

   private:
    void CalculateDeltaTime();
    void ProcessInput();
    void Update();
    void Render();

   private:
    std::unique_ptr<CEGameInstance> m_GameInstance;
    std::unique_ptr<RenderSystem> m_RenderSystem;
    CE::FrameRenderData m_RenderData;
    // Application Info
    AppInfo* m_info = nullptr;
    // Время и FPS
    float m_DeltaTime = 0.0f;
    float m_LastFrameTime = 0.0f;
    bool m_IsRunning = false;

    // Статистика
    uint32_t m_FrameCount = 0;
    float m_FPSTimer = 0.0f;
  };
}  // namespace CE