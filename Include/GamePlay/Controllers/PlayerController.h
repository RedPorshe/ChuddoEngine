#pragma once

#include "GamePlay/Actors/Actor.h"
#include "GamePlay/Pawns/Pawn.h"

// Forward declaration
namespace CE
{
  class CEWorld;
}

namespace CE
{
  class PlayerController : public CEActor
  {
   public:
    PlayerController(CEObject* Owner = nullptr, FString NewName = "PlayerController");
    virtual ~PlayerController() = default;

    virtual void BeginPlay() override;
    virtual void Update(float DeltaTime) override;

    // Управление Pawn'ом
    void Possess(CEPawn* Pawn);
    void UnPossess();
    CEPawn* GetPossessedPawn() const
    {
      return m_PossessedPawn;
    }

    // Система ввода
    virtual void SetupInputComponent();
    InputComponent* GetInputComponent() const
    {
      return m_InputComponent;
    }

    // Получение контроллера из мира
    static PlayerController* GetFirstPlayerController(CEWorld* World);

   protected:
    CEPawn* m_PossessedPawn = nullptr;
    InputComponent* m_InputComponent = nullptr;
  };
}  // namespace CE