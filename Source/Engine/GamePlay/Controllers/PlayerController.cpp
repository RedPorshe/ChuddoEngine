#include "Engine/GamePlay/Controllers/PlayerController.h"

#include "Engine/GamePlay/World/World.h"
#include "Engine/GamePlay/World/Levels/Level.h"

namespace CE
{
  PlayerController::PlayerController(CObject* Owner, FString NewName)
      : CEActor(Owner, NewName)
  {
    // Используем AddSubObject вместо AddDefaultSubObject чтобы избежать проблем с типами
    m_InputComponent = AddSubObject<InputComponent>("Input", this, "InputComponent");
    CE_CORE_DEBUG("PlayerController created: ", NewName);
  }

  void PlayerController::BeginPlay()
  {
    CEActor::BeginPlay();
    SetupInputComponent();
    CE_CORE_DEBUG("PlayerController BeginPlay: ", GetName());
  }

  void PlayerController::Update(float DeltaTime)
  {
    CEActor::Update(DeltaTime);

    // Здесь можно добавить логику обновления контроллера
    // Например, проверку состояний ввода и т.д.
  }

  void PlayerController::Possess(CEPawn* Pawn)
  {
    if (m_PossessedPawn == Pawn)
    {
      return;
    }

    if (m_PossessedPawn)
    {
      CE_CORE_DEBUG("PlayerController unpossessing current pawn: ", m_PossessedPawn->GetName());
      UnPossess();
    }

    m_PossessedPawn = Pawn;
    if (m_PossessedPawn)
    {
      CE_CORE_DEBUG("PlayerController possessed pawn: ", Pawn->GetName());

      // Перенаправляем ввод с контроллера на pawn
      if (m_InputComponent && m_PossessedPawn->GetInputComponent())
      {
        m_PossessedPawn->OnPossess();
      }
    }
  }

  void PlayerController::UnPossess()
  {
    if (m_PossessedPawn)
    {
      CE_CORE_DEBUG("PlayerController unpossessed pawn: ", m_PossessedPawn->GetName());
      m_PossessedPawn = nullptr;
    }
  }

  void PlayerController::SetupInputComponent()
  {
    if (m_InputComponent)
    {
      // Пример: биндинг для переключения режима ввода
      m_InputComponent->BindAction("ToggleInputMode", EInputEvent::Pressed,
                                   [this]()
                                   {
                                     CE_CORE_DEBUG("Toggle input mode pressed");
                                   });
    }
  }

  PlayerController* PlayerController::GetFirstPlayerController(CEWorld* World)
  {
    if (!World || !World->GetCurrentLevel())
      return nullptr;

    // Ищем первый PlayerController в текущем уровне
    for (const auto& actor : World->GetCurrentLevel()->GetActors())
    {
      if (auto* controller = dynamic_cast<PlayerController*>(actor.get()))
      {
        return controller;
      }
    }

    return nullptr;
  }
}  // namespace CE