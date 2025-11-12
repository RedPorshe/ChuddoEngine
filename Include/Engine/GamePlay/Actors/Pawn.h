#pragma once

#include "Engine/GamePlay/Actors/Actor.h"
#include "Engine/GamePlay/Components/Camera/CameraComponent.h"
#include "Engine/GamePlay/Components/Input/InputComponent.h"

namespace CE
{
  class CEPawn : public CEActor
  {
   public:
    CEPawn(CEObject* Owner = nullptr, FString NewName = "Pawn");
    virtual ~CEPawn() = default;
    void SetMouseInverted(bool val)
    {
      bIsInvertMouseLook = val;
    }
    virtual void BeginPlay() override;
    virtual void Update(float DeltaTime) override;

    // Система ввода - аналог UE
    virtual void SetupPlayerInputComponent();

    // Компоненты
    InputComponent* GetInputComponent() const
    {
      return m_InputComponent;
    }
    CameraComponent* GetCameraComponent() const
    {
      return m_CameraComponent;
    }

    // Базовые методы управления
    virtual void MoveForward(float Value);
    virtual void MoveRight(float Value);
    virtual void LookHorizontal(float Value);
    virtual void LookVertical(float Value);

   protected:
    InputComponent* m_InputComponent = nullptr;
    CameraComponent* m_CameraComponent = nullptr;

    // Настройки управления
    float m_MoveSpeed = .30f;
    float m_LookSensitivity = 0.5f;
    glm::vec2 m_CurrentRotation{0.0f};
    bool bIsInvertMouseLook{false};
  };
}  // namespace CE