#pragma once
#include <unordered_map>

#include "Engine/Core/Object.h"
#include "Engine/GamePlay/World/World.h"

class CGameInstance : public CObject
{
 public:
  CGameInstance();
  virtual ~CGameInstance() = default;

  // Жизненный цикл
  virtual void Initialize();
  virtual void Shutdown();

  // Управление мирами
  CWorld* CreateWorld(const FString& WorldName);
  void LoadWorld(const FString& WorldName);
  void UnloadCurrentWorld();

  CWorld* GetCurrentWorld() const
  {
    return m_CurrentWorld;
  }
  CWorld* GetWorld(const FString& WorldName) const;

 
  virtual void BeginPlay();
  virtual void Update(float DeltaTime);
  virtual void Tick(float DeltaTime);

 protected:
  virtual void SetupWorlds();  
 protected:
  CWorld* m_CurrentWorld = nullptr;
  std::unordered_map<FString, std::unique_ptr<CWorld>> m_Worlds;
};
