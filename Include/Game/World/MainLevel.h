#pragma once

#include "Engine/GamePlay/Actors/Character.h"
#include "Engine/GamePlay/Actors/Pawn.h"
#include "Engine/GamePlay/Controllers/PlayerController.h"
#include "Engine/GamePlay/World/Levels/Level.h"

class TerrainActor;

class MainLevel : public CLevel
{
 public:
  MainLevel(CObject* Owner = nullptr,
            FString NewName = "Main level");
  virtual ~MainLevel() = default;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;
  virtual void Update(float DeltaTime) override;

 protected:
  CPlayerController* playerController;
  CCharacter* playerCharacter;
  TerrainActor* terrain = nullptr;
  CActor* enemy = nullptr;
};
