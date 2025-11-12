#pragma once

#include "Engine/GamePlay/Actors/Actor.h"
#include "Engine/GamePlay/Components/InputComponent.h"

namespace CE
{
  class CEPawn : public CEActor
  {
   public:
    CEPawn(CEObject* Owner = nullptr, FString NewName = "Pawn");
    virtual ~CEPawn() = default;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Система ввода - аналог UE
    virtual void SetupPlayerInputComponent();

    // Компоненты
    InputComponent* GetInputComponent() const
    {
      return m_InputComponent;
    }

   protected:
    InputComponent* m_InputComponent = nullptr;
  };
}  // namespace CE