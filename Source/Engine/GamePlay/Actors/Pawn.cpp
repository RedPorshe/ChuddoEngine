#include "Engine/GamePlay/Actors/Pawn.h"

#include "Engine/GamePlay/Components/InputComponent.h"

namespace CE
{
  CEPawn::CEPawn(CEObject* Owner, FString NewName)
      : CEActor(Owner, NewName)
  {
    // Создаем компонент ввода
    m_InputComponent = AddSubObject<InputComponent>("Input", this, "InputComponent");

    CE_CORE_DEBUG("CEPawn created: ", NewName);
  }

  void CEPawn::BeginPlay()
  {
    CEActor::BeginPlay();

    SetupPlayerInputComponent();
  }

  void CEPawn::Tick(float DeltaTime)
  {
    CEActor::Tick(DeltaTime);
  }

  void CEPawn::SetupPlayerInputComponent()
  {
    if (m_InputComponent)
    {
    }
  }

}  // namespace CE