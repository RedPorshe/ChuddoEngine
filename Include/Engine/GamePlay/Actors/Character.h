#pragma once
#include "Engine/GamePlay/Actors/Pawn.h"
#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/GamePlay/Components/SpringArmComponent.h"

namespace CE
{
  class CCharacter : public CPawn
  {
   public:
    CCharacter(CObject* Owner = nullptr, FString NewName = "Character");
    virtual ~CCharacter() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;


    CMeshComponent* GetMeshComponent() const
    {
      return m_Mesh;
    }

   protected:
    virtual void SetupPlayerInputComponent() override;

    // Movement functions moved from CPawn
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Jump();
    void LookUp(float Value);
    void Turn(float Value);

    CMeshComponent* m_Mesh = nullptr;
    CCameraComponent* m_Camera = nullptr;
    CSpringArmComponent* m_SpringArm = nullptr;
    float m_VerticalVelocity = 0.0f;
    bool m_IsOnGround = false;
    bool bIsJumping{false};

   private:
  };
}  // namespace CE
