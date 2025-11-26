#pragma once
#include "Engine/GamePlay/Actors/Pawn.h"
#include "Engine/GamePlay/Components/MeshComponent.h"

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
      return m_MeshComponent;
    }

   protected:
    virtual void SetupPlayerInputComponent() override;

   
    CMeshComponent* m_MeshComponent = nullptr;
    float m_VerticalVelocity = 0.0f;
    bool m_IsOnGround = false;

   private:
  };
}  // namespace CE