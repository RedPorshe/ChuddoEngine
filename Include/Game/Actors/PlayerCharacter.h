#pragma once
#include "Engine/GamePlay/Actors/Character.h"
#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/GamePlay/Components/SpringArmComponent.h"

class PlayerCharacter : public CE::CECharacter
{
 public:
  PlayerCharacter(CE::CEObject* Owner = nullptr, CE::FString NewName = "PlayerCharacter");
  virtual ~PlayerCharacter() = default;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

 private:
  virtual void SetupPlayerInputComponent() override;

  CE::SpringArmComponent* m_SpringArm = nullptr;
  CE::CameraComponent* m_Camera = nullptr;

  void MoveForward(float Value);
  void MoveRight(float Value);
  void LookHorizontal(float Value);
  void LookVertical(float Value);
  float m_MouseSensitivity = 0.5f;
};