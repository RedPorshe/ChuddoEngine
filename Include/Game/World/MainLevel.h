#pragma once

#include "Engine/GamePlay/Actors/Character.h"
#include "Engine/GamePlay/Actors/Pawn.h"
#include "Engine/GamePlay/Controllers/PlayerController.h"
#include "Engine/GamePlay/World/Levels/CELevel.h"
namespace CE
{
  class TerrainActor;
}

class MainLevel : public CE::CELevel
{
 public:
  MainLevel(CE::CEObject* Owner = nullptr,
            CE::FString NewName = "Main level");
  virtual ~MainLevel() = default;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;
  virtual void Update(float DeltaTime) override;

 protected:
  CE::PlayerController* playerController;
  CE::CEPawn* playerCharacter;
  CE::TerrainActor* terrain = nullptr;
  CE::CEActor* enemy = nullptr;
};
