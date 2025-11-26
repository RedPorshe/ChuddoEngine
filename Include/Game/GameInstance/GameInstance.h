#pragma once
#include "Engine/GamePlay/GameInstance/GameInstance.h"

class MainGameInstance : public CE::CGameInstance
{
 public:
  MainGameInstance();
  virtual ~MainGameInstance() = default;
  virtual void Update(float DeltaTime) override;

 protected:
  virtual void SetupWorlds() override;
};