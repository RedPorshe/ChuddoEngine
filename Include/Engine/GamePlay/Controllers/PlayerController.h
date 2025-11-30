#pragma once

#include "Engine/GamePlay/Actors/Actor.h"
#include "Engine/GamePlay/Actors/Pawn.h"


  class CWorld;

  class CPlayerController : public CActor
  {
   public:
    CPlayerController(CObject* Owner = nullptr, FString NewName = "PlayerController");
    virtual ~CPlayerController() = default;

    virtual void BeginPlay() override;
    virtual void Update(float DeltaTime) override;

    // Управление Pawn'ом
    void Possess(CPawn* Pawn);
    void UnPossess();
    CPawn* GetPossessedPawn() const
    {
      return m_PossessedPawn;
    }

    // Система ввода
    virtual void SetupInputComponent();
    CInputComponent* GetInputComponent() const
    {
      return m_InputComponent;
    }

    // Получение контроллера из мира
    static CPlayerController* GetFirstPlayerController(CWorld* World);

   protected:
    CPawn* m_PossessedPawn = nullptr;
    CInputComponent* m_InputComponent = nullptr;
  };