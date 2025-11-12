#pragma once
#include "Engine/GamePlay/Actors/Pawn.h"
#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/GamePlay/Components/CapsuleComponent.h"
#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/GamePlay/Components/SpringArmComponent.h"

namespace CE
{
  class CECharacter : public CEPawn
  {
   public:
    CECharacter(CEObject* Owner = nullptr, FString NewName = "Character");
    virtual ~CECharacter() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Компоненты персонажа
    CECapsuleComponent* GetCapsuleComponent() const
    {
      return m_CapsuleComponent;
    }
    MeshComponent* GetMeshComponent() const
    {
      return m_MeshComponent;
    }
    SpringArmComponent* GetSpringArmComponent() const
    {
      return m_SpringArm;
    }
    CameraComponent* GetCameraComponent() const
    {
      return m_Camera;
    }

   protected:
    virtual void SetupPlayerInputComponent() override;
    void SetupCharacterComponents();
    // Компоненты персонажа
    CECapsuleComponent* m_CapsuleComponent = nullptr;
    MeshComponent* m_MeshComponent = nullptr;
    SpringArmComponent* m_SpringArm = nullptr;
    CameraComponent* m_Camera = nullptr;

   private:
    float m_VerticalVelocity = 0.0f;
    bool m_IsOnGround = false;
  };
}  // namespace CE