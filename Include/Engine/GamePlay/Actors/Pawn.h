#pragma once

#include "Engine/GamePlay/Actors/Actor.h"
#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/GamePlay/Components/InputComponent.h"

namespace CE
{
  class CPawn : public CActor
  {
   public:
    CPawn(CObject* Owner = nullptr, FString NewName = "Pawn");
    virtual ~CPawn() = default;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    void OnPossess();

    // Система ввода - аналог UE
    virtual void SetupPlayerInputComponent();

    // Компоненты
    CInputComponent* GetInputComponent() const
    {
      return this->m_InputComponent;
    }

    // Аналог AddMovementInput из UE
    void AddMovementInput(const Math::Vector3f& WorldDirection, float ScaleValue = 1.0f, bool bForce = false);
    void AddControllerYawInput(float Value);
    void AddControllerPitchInput(float Value);

    // Управление вращением
    void SetControlRotation(const Math::Vector3f& NewRotation);
    const Math::Vector3f& GetControlRotation() const
    {
      return m_ControlRotation;
    }

    // Получение векторов движения (аналогично камере)
    Math::Vector3f GetPawnViewLocation() const;
    Math::Vector3f GetViewForwardVector() const;
    Math::Vector3f GetViewRightVector() const;
    Math::Vector3f GetViewUpVector() const;

    // Получение векторов движения
    Math::Vector3f GetMovementDirection() const
    {
      return m_MovementInput;
    }
    void ConsumeMovementInput();

    // Флаги управления
    void SetUseControllerRotation(bool bUse)
    {
      m_bUseControllerRotation = bUse;
    }
    bool GetUseControllerRotation() const
    {
      return m_bUseControllerRotation;
    }

   protected:
    CInputComponent* m_InputComponent = nullptr;
    Math::Vector3f m_MovementInput{0.0f, 0.0f, 0.0f};
    Math::Vector3f m_ControlRotation{0.0f, 0.0f, 0.0f};  // Pitch, Yaw, Roll
    bool m_bMovementInputConsumed = false;
    bool m_bUseControllerRotation = false;

   private:
    CCameraComponent* FindCameraComponent() const;
    bool m_bIsJumping = false;
    void ApplyRotationToActor();
    void ApplyMovementInputToActor();

    void MoveForward(float Value);
    void MoveRight(float Value);
    void lookUp(float Value);
    void turn(float Value);
    void jump();
  };
}  // namespace CE