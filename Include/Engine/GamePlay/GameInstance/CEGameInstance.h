#pragma once
#include <unordered_map>

#include "Engine/Core/CEObject.h"
#include "Engine/GamePlay/World/CEWorld.h"

namespace CE
{
  class CEGameInstance : public CEObject
  {
   public:
    CEGameInstance();
    virtual ~CEGameInstance() = default;

    // Жизненный цикл
    virtual void Initialize();
    virtual void Shutdown();

    // Управление мирами
    CEWorld* CreateWorld(const FString& WorldName);
    void LoadWorld(const FString& WorldName);
    void UnloadCurrentWorld();

    CEWorld* GetCurrentWorld() const
    {
      return m_CurrentWorld;
    }
    CEWorld* GetWorld(const FString& WorldName) const;

    // Глобальное состояние
    virtual void BeginPlay();
    virtual void Update(float DeltaTime);
    virtual void Tick(float DeltaTime);

   protected:
    virtual void SetupWorlds();  // Виртуальный метод для настройки миров

   protected:
    CEWorld* m_CurrentWorld = nullptr;
    std::unordered_map<FString, std::unique_ptr<CEWorld>> m_Worlds;
  };
}  // namespace CE