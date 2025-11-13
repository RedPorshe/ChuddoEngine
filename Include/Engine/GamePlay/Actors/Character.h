#pragma once
#include "Engine/GamePlay/Actors/Pawn.h"
#include "Engine/GamePlay/Components/CapsuleComponent.h"
#include "Engine/GamePlay/Components/MeshComponent.h"

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

   protected:
    virtual void SetupPlayerInputComponent() override;

    CECapsuleComponent* m_CapsuleComponent = nullptr;
    MeshComponent* m_MeshComponent = nullptr;
    float m_VerticalVelocity = 0.0f;
    bool m_IsOnGround = false;

   private:
  };
}  // namespace CE