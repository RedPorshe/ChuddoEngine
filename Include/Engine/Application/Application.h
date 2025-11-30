#pragma once
#include "CoreMinimal.h"
#include "Engine/Core/AppInfo.h"
#include "Engine/Core/Rendering/Data/RenderData.h"
#include "Engine/Core/Rendering/Vulkan/Integration/RenderSystem.h"
#include "Engine/GamePlay/GameInstance/GameInstance.h"

namespace CE
{
  class Application
  {
   public:
    Application(AppInfo* info);
    virtual ~Application();

    virtual void Initialize();
    virtual void Run();
    virtual void Shutdown();

    // Геттеры для доступа извне
    CGameInstance* GetGameInstance() const
    {
      return m_GameInstance.get();
    }
    float GetDeltaTime() const
    {
      return m_DeltaTime;
    }

    protected:
   
    void CalculateDeltaTime();
    void ProcessInput();
    void Update();
    void Render();

    std::unique_ptr<CGameInstance> m_GameInstance;
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