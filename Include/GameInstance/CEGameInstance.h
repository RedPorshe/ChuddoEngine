#pragma once
#include <unordered_map>

#include "Core/CEObject.h"
#include "World/CEWorld.h"

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
    void CreateWorld(const FString& WorldName);
    void LoadWorld(const FString& WorldName);
    void UnloadCurrentWorld();

    CEWorld* GetCurrentWorld() const
    {
      return m_CurrentWorld;
    }  // ← ИСПРАВЛЕНО: добавить .get()
    CEWorld* GetWorld(const FString& WorldName) const;

    // Глобальное состояние
    virtual void BeginPlay();
    virtual void Update(float DeltaTime);
    virtual void Tick(float DeltaTime);

   protected:
    CEWorld* m_CurrentWorld = nullptr;
    std::unordered_map<FString, std::unique_ptr<CEWorld>> m_Worlds;
  };
}  // namespace CE