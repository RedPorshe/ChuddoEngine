#include "Pawns/Pawn.h"

#include "Components/Input/InputComponent.h"

namespace CE
{
  CEPawn::CEPawn(CEObject* Owner, FString NewName)
      : CEActor(Owner, NewName)
  {
    // Создаем компонент ввода
    m_InputComponent = AddSubObject<InputComponent>("Input", this, "InputComponent");

    // Создаем камеру как root component
    m_CameraComponent = AddSubObject<CameraComponent>("Camera", this, "CameraComponent");
    SetRootComponent(m_CameraComponent);

    CE_CORE_DEBUG("CEPawn created: ", NewName);
  }

  void CEPawn::BeginPlay()
  {
    CEActor::BeginPlay();

    // Вызываем настройку ввода - аналог UE's SetupPlayerInputComponent
    SetupPlayerInputComponent();

    CE_CORE_DEBUG("CEPawn BeginPlay: ", GetName());
  }

  void CEPawn::Update(float DeltaTime)
  {
    CEActor::Update(DeltaTime);

    // Здесь можно добавить логику обновления, если нужно
  }

  void CEPawn::SetupPlayerInputComponent()
  {
    // Базовые биндинги - могут быть переопределены в дочерних классах
    if (m_InputComponent)
    {
      // Axis биндинги для непрерывного ввода
      m_InputComponent->BindAxis("MoveForward",
                                 [this](float Value)
                                 { MoveForward(Value); });
      m_InputComponent->BindAxis("MoveRight",
                                 [this](float Value)
                                 { MoveRight(Value); });
      m_InputComponent->BindAxis("LookHorizontal",
                                 [this](float Value)
                                 { LookHorizontal(Value); });
      m_InputComponent->BindAxis("LookVertical",
                                 [this](float Value)
                                 { LookVertical(Value); });
    }
  }

  void CEPawn::MoveForward(float Value)
  {
    if (Value == 0.0f || !m_CameraComponent)
      return;

    // Вычисляем направление вперед на основе текущего поворота
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);  // Forward по -Z
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_CurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    forward = glm::vec3(rotationMatrix * glm::vec4(forward, 0.0f));

    glm::vec3 movement = forward * Value * m_MoveSpeed;
    SetActorLocation(GetActorLocation() + movement);
  }

  void CEPawn::MoveRight(float Value)
  {
    if (Value == 0.0f || !m_CameraComponent)
      return;

    // Вычисляем направление вправо на основе текущего поворота
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);  // Right по X
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_CurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    right = glm::vec3(rotationMatrix * glm::vec4(right, 0.0f));

    glm::vec3 movement = right * Value * m_MoveSpeed;
    SetActorLocation(GetActorLocation() + movement);
  }

  void CEPawn::LookHorizontal(float Value)
  {
    if (Value == 0.0f)
      return;

    m_CurrentRotation.y += Value * m_LookSensitivity;

    // Нормализуем угол
    if (m_CurrentRotation.y > 180.0f)
      m_CurrentRotation.y -= 360.0f;
    if (m_CurrentRotation.y < -180.0f)
      m_CurrentRotation.y += 360.0f;

    if (m_CameraComponent)
    {
      m_CameraComponent->SetRotation(glm::vec3(m_CurrentRotation.x, m_CurrentRotation.y, 0.0f));
    }
  }

  void CEPawn::LookVertical(float Value)
  {
    if (Value == 0.0f)
      return;

    m_CurrentRotation.x += Value * m_LookSensitivity;

    // Ограничиваем угол обзора по вертикали
    m_CurrentRotation.x = glm::clamp(m_CurrentRotation.x, -89.0f, 89.0f);

    if (m_CameraComponent)
    {
      m_CameraComponent->SetRotation(glm::vec3(m_CurrentRotation.x, m_CurrentRotation.y, 0.0f));
    }
  }
}  // namespace CE