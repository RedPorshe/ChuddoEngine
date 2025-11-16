#include "Engine/GamePlay/Actors/Pawn.h"

#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/GamePlay/Components/InputComponent.h"
#include "Engine/GamePlay/Components/SpringArmComponent.h"

namespace CE
{
  CEPawn::CEPawn(CEObject* Owner, FString NewName)
      : CEActor(Owner, NewName)
  {
    m_InputComponent = AddSubObject<InputComponent>("Input", this, "InputComponent");
    SetupPlayerInputComponent();
    CE_CORE_DEBUG("CEPawn created: {}", NewName.c_str());
  }

  void CEPawn::BeginPlay()
  {
    CEActor::BeginPlay();
  }

  void CEPawn::Tick(float DeltaTime)
  {
    CEActor::Tick(DeltaTime);

    // Применяем управляющее вращение к корневому компоненту
    if (m_RootComponent && m_bUseControllerRotation)
    {
      // Применяем только Yaw для поворота персонажа
      glm::vec3 currentRotation = m_RootComponent->GetRelativeRotation();
      m_RootComponent->SetRelativeRotation(glm::vec3(currentRotation.x, m_ControlRotation.y, currentRotation.z));

      CE_CORE_DEBUG("Applied controller rotation to root: Yaw={}", m_ControlRotation.y);
    }

    // Обработка накопленного ввода движения
    if (m_RootComponent && glm::length(m_MovementInput) > 0.01f)
    {
      glm::vec3 finalMovement = m_MovementInput;

      if (m_bUseControllerRotation)
      {
        // First Person: движение в локальных координатах персонажа
        // Персонаж уже повернут, поэтому движение должно быть в его локальных координатах
        finalMovement = m_MovementInput;  // Оставляем как есть - это локальное движение

        CE_CORE_DEBUG("First Person movement - Local Input: ({}, {}, {})",
                      finalMovement.x, finalMovement.y, finalMovement.z);
      }
      else
      {
        // Third Person: движение относительно камеры
        glm::vec3 forward = GetViewForwardVector();
        glm::vec3 right = GetViewRightVector();

        // Игнорируем вертикальную составляющую для движения по плоскости
        forward.y = 0.0f;
        right.y = 0.0f;

        if (glm::length(forward) > 0.01f)
          forward = glm::normalize(forward);
        if (glm::length(right) > 0.01f)
          right = glm::normalize(right);

        // Преобразуем в мировые координаты относительно камеры
        finalMovement = forward * m_MovementInput.z + right * m_MovementInput.x;

        CE_CORE_DEBUG("Third Person movement - Camera Relative: ({}, {}, {})",
                      finalMovement.x, finalMovement.y, finalMovement.z);
      }

      // Применяем движение к корневому компоненту
      m_RootComponent->Move(finalMovement);

      CE_CORE_DEBUG("Final movement applied: ({}, {}, {})",
                    finalMovement.x, finalMovement.y, finalMovement.z);
    }

    // Сбрасываем ввод движения после обработки
    ConsumeMovementInput();
  }

  CameraComponent* CEPawn::FindCameraComponent() const
  {
    // Ищем компонент камеры среди дочерних компонентов
    CameraComponent* camera = nullptr;
    ForEachComponent<CameraComponent>([&camera](CameraComponent* comp)
                                      {
      if (!camera) 
      {
        camera = comp;
       
      } });

    if (!camera)
    {
      CE_CORE_DEBUG("No camera component found");
    }

    return camera;
  }

  glm::vec3 CEPawn::GetPawnViewLocation() const
  {
    if (CameraComponent* camera = FindCameraComponent())
    {
      glm::vec3 pos = camera->GetWorldLocation();
      return pos;
    }

    glm::vec3 pos = GetActorLocation() + glm::vec3(0.0f, 0.7f, 0.0f);
    return pos;
  }

  glm::vec3 CEPawn::GetViewForwardVector() const
  {
    if (CameraComponent* camera = FindCameraComponent())
    {
      glm::vec3 forward = camera->GetCameraForwardVector();
      return forward;
    }

    if (m_bUseControllerRotation)
    {
      glm::quat rotation = glm::quat(glm::radians(m_ControlRotation));
      glm::vec3 forward = rotation * m_RootComponent->GetForwardVector();
      return forward;
    }

    glm::vec3 forward = m_RootComponent ? m_RootComponent->GetForwardVector() : glm::vec3(0.0f, 0.0f, 1.0f);
    return forward;
  }

  glm::vec3 CEPawn::GetViewRightVector() const
  {
    if (CameraComponent* camera = FindCameraComponent())
    {
      glm::vec3 right = camera->GetCameraRightVector();
      return right;
    }

    if (m_bUseControllerRotation)
    {
      glm::quat rotation = glm::quat(glm::radians(m_ControlRotation));
      glm::vec3 right = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
      return right;
    }
    glm::vec3 right = m_RootComponent ? m_RootComponent->GetRightVector() : glm::vec3(1.0f, 0.0f, 0.0f);

    return right;
  }

  glm::vec3 CEPawn::GetViewUpVector() const
  {
    if (CameraComponent* camera = FindCameraComponent())
    {
      return camera->GetCameraUpVector();
    }

    if (m_bUseControllerRotation)
    {
      glm::quat rotation = glm::quat(glm::radians(m_ControlRotation));
      return rotation * glm::vec3(0.0f, 1.0f, 0.0f);
    }

    return m_RootComponent ? m_RootComponent->GetUpVector() : glm::vec3(0.0f, 1.0f, 0.0f);
  }

  void CEPawn::OnPossess()
  {
    SetupPlayerInputComponent();
  }

  void CEPawn::SetupPlayerInputComponent()
  {
    if (m_InputComponent)
    {
    }
  }

  void CEPawn::AddMovementInput(const glm::vec3& WorldDirection, float ScaleValue, bool bForce)
  {
    if (ScaleValue != 0.0f && (bForce || m_RootComponent != nullptr))
    {
      glm::vec3 direction = WorldDirection;
      if (glm::length(direction) > 0.01f)
      {
        direction = glm::normalize(direction);
      }
      direction.z *= -1.0f;  // Invert Z for Vulkan coordinate system
      m_MovementInput += direction * ScaleValue;
      m_bMovementInputConsumed = false;
    }
  }

  void CEPawn::AddControllerYawInput(float Value)
  {
    m_ControlRotation.y += Value;
    while (m_ControlRotation.y > 180.0f)
      m_ControlRotation.y -= 360.0f;
    while (m_ControlRotation.y < -180.0f)
      m_ControlRotation.y += 360.0f;
  }

  void CEPawn::AddControllerPitchInput(float Value)
  {
    m_ControlRotation.x += Value;
    m_ControlRotation.x = glm::clamp(m_ControlRotation.x, -89.0f, 89.0f);
  }

  void CEPawn::SetControlRotation(const glm::vec3& NewRotation)
  {
    m_ControlRotation = NewRotation;
    m_ControlRotation.x = glm::clamp(m_ControlRotation.x, -89.0f, 89.0f);
  }

  void CEPawn::ConsumeMovementInput()
  {
    m_MovementInput = glm::vec3(0.0f);
    m_bMovementInputConsumed = true;
  }
}  // namespace CE