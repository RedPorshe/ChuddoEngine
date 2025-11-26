#pragma once

#include "Engine/GamePlay/Actors/Character.h"
#include "Engine/GamePlay/Actors/Pawn.h"
#include "Engine/GamePlay/Controllers/PlayerController.h"
#include "Engine/GamePlay/World/Levels/Level.h"
namespace CE
{
  class TerrainActor;
}

class MainLevel : public CE::CLevel
{
 public:
  MainLevel(CE::CObject* Owner = nullptr,
            CE::FString NewName = "Main level");
  virtual ~MainLevel() = default;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;
  virtual void Update(float DeltaTime) override;

 protected:
  CE::CPlayerController* playerController;
  CE::CPawn* playerCharacter;
  CE::TerrainActor* terrain = nullptr;
  CE::CActor* enemy = nullptr;
};
