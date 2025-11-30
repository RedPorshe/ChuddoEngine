#include "Engine/GamePlay/Controllers/PlayerController.h"

#include "Engine/GamePlay/World/World.h"
#include "Engine/GamePlay/World/Levels/Level.h"


  CPlayerController::CPlayerController(CObject* Owner, FString NewName)
      : CActor(Owner, NewName)
  {
   
    m_InputComponent = AddSubObject<CInputComponent>("Input", this, "InputComponent");
    CORE_DEBUG("PlayerController created: ", NewName);
  }

  void CPlayerController::BeginPlay()
  {
    CActor::BeginPlay();
    SetupInputComponent();
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

      
      if (m_InputComponent && m_PossessedPawn->GetInputComponent())
      {
        m_PossessedPawn->OnPossess();
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

  void CPlayerController::SetupInputComponent()
  {
    if (m_InputComponent)
    {
     
      m_InputComponent->BindAction("ToggleInputMode", EInputEvent::Pressed,
                                   [this]()
                                   {
                                     CORE_DEBUG("Toggle input mode pressed");
                                   });
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