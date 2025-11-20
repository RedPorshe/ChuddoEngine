#pragma once
#include "Engine/GamePlay/Actors/Pawn.h"
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

    
    MeshComponent* GetMeshComponent() const
    {
      return m_MeshComponent;
    }

   protected:
    virtual void SetupPlayerInputComponent() override;

   
    MeshComponent* m_MeshComponent = nullptr;
    float m_VerticalVelocity = 0.0f;
    bool m_IsOnGround = false;

   private:
  };
}  // namespace CE