#pragma once
#include "Engine/GamePlay/GameInstance/CEGameInstance.h"

class MainGameInstance : public CE::CEGameInstance
{
 public:
  MainGameInstance();
  virtual ~MainGameInstance() = default;
  virtual void Update(float DeltaTime) override;

 protected:
  virtual void SetupWorlds() override;
};