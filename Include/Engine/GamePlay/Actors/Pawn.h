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
    void AddMovementInput(const glm::vec3& WorldDirection, float ScaleValue = 1.0f, bool bForce = false);
    void AddControllerYawInput(float Value);
    void AddControllerPitchInput(float Value);

    // Управление вращением
    void SetControlRotation(const glm::vec3& NewRotation);
    const glm::vec3& GetControlRotation() const
    {
      return m_ControlRotation;
    }

    // Получение векторов движения (аналогично камере)
    glm::vec3 GetPawnViewLocation() const;
    glm::vec3 GetViewForwardVector() const;
    glm::vec3 GetViewRightVector() const;
    glm::vec3 GetViewUpVector() const;

    // Получение векторов движения
    glm::vec3 GetMovementDirection() const
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
    glm::vec3 m_MovementInput{0.0f, 0.0f, 0.0f};
    glm::vec3 m_ControlRotation{0.0f, 0.0f, 0.0f};  // Pitch, Yaw, Roll
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