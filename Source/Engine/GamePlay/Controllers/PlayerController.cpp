#include "Engine/GamePlay/Controllers/PlayerController.h"

#include "Engine/GamePlay/World/World.h"
#include "Engine/GamePlay/World/Levels/Level.h"
#include "Engine/GamePlay/Components/InputComponent.h"


  CPlayerController::CPlayerController(CObject* Owner, FString NewName)
      : CActor(Owner, NewName)
  {
    CORE_DEBUG("PlayerController created: ", NewName);
  }

  void CPlayerController::BeginPlay()
  {
    CActor::BeginPlay();
    CORE_DEBUG("PlayerController BeginPlay: ", GetName());
  }

  void CPlayerController::Update(float DeltaTime)
  {
    CActor::Update(DeltaTime);

    
  }

  void CPlayerController::Possess(CPawn* Pawn)
  {
    if (m_PossessedPawn == Pawn)
    {
      return;
    }

    if (m_PossessedPawn)
    {
      CORE_DEBUG("PlayerController unpossessing current pawn: ", m_PossessedPawn->GetName());
      UnPossess();
    }

    m_PossessedPawn = Pawn;
    if (m_PossessedPawn)
    {
      CORE_DEBUG("PlayerController possessed pawn: ", Pawn->GetName());
      m_PossessedPawn->OnPossess();

      // Setup input
      m_InputComponent = m_PossessedPawn->GetInputComponent();
      if (m_InputComponent)
      {
        // Movement axes
        m_InputComponent->BindAxis("MoveForward", [this](float value) {
          if (m_PossessedPawn) m_PossessedPawn->MoveForward(value);
        }, 1.0f);

        m_InputComponent->BindAxis("MoveRight", [this](float value) {
          if (m_PossessedPawn) m_PossessedPawn->MoveRight(value);
        }, 1.0f);

        m_InputComponent->BindAxis("MoveUp", [this](float value) {
          // Implement move up if needed
        }, 1.0f);

        // Look axes
        m_InputComponent->BindAxis("MouseX", [this](float value) {
          if (m_PossessedPawn) m_PossessedPawn->turn(value);
        }, 1.0f);

        m_InputComponent->BindAxis("MouseY", [this](float value) {
          if (m_PossessedPawn) m_PossessedPawn->lookUp(value);
        }, 1.0f);

        // Actions
        m_InputComponent->BindAction("Jump", EInputEvent::Pressed, [this]() {
          if (m_PossessedPawn) m_PossessedPawn->jump();
        });

        m_InputComponent->BindAction("Fire", EInputEvent::Pressed, [this]() {
          // Implement fire action if needed
        });

        CORE_DEBUG("PlayerController input bindings set up");
      }
    }
  }

  void CPlayerController::UnPossess()
  {
    if (m_PossessedPawn)
    {
      CORE_DEBUG("PlayerController unpossessed pawn: ", m_PossessedPawn->GetName());
      m_PossessedPawn = nullptr;
    }
  }



  CPlayerController* CPlayerController::GetFirstPlayerController(CWorld* World)
  {
    if (!World || !World->GetCurrentLevel())
      return nullptr;

   
    for (const auto& actor : World->GetCurrentLevel()->GetActors())
    {
      if (auto* controller = dynamic_cast<CPlayerController*>(actor.get()))
      {
        return controller;
      }
    }

    return nullptr;
  }