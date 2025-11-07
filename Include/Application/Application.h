#pragma once
#include "CoreMinimal.h"
#include "GameInstance/CEGameInstance.h"
#include "Rendering/Data/RenderData.h"

namespace CE
{
  class Application
  {
   public:
    Application();
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

    CE::FrameRenderData m_RenderData;

    // Время и FPS
    float m_DeltaTime = 0.0f;
    float m_LastFrameTime = 0.0f;
    bool m_IsRunning = false;

    // Статистика
    uint32_t m_FrameCount = 0;
    float m_FPSTimer = 0.0f;
  };
}  // namespace CE