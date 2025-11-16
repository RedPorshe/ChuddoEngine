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

  CE::SpringArmComponent* GetSpringArm() const
  {
    return m_SpringArm;
  }
  CE::CameraComponent* GetCamera() const
  {
    return m_Camera;
  }

 private:
  virtual void SetupPlayerInputComponent() override;

  void MoveForward(float Value);
  void MoveRight(float Value);
  void LookHorizontal(float Value);
  void LookVertical(float Value);
  void Jump();

  CE::SpringArmComponent* m_SpringArm = nullptr;
  CE::CameraComponent* m_Camera = nullptr;

  float m_MouseSensitivity = 1.0f;
  float m_MoveSpeed = 0.05f;
};