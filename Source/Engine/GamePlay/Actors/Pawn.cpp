#include "Engine/GamePlay/Actors/Pawn.h"

#include "Engine/GamePlay/Components/Input/InputComponent.h"

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

    SetupPlayerInputComponent();
  }

  void CEPawn::Update(float DeltaTime)
  {
    CEActor::Update(DeltaTime);
  }

  void CEPawn::SetupPlayerInputComponent()
  {
    if (m_InputComponent)
    {
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

    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_CurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    forward = glm::vec3(rotationMatrix * glm::vec4(forward, 0.0f));

    glm::vec3 movement = forward * Value * m_MoveSpeed;
    glm::vec3 newLocation = GetActorLocation() + movement;

    SetActorLocation(newLocation);
  }

  void CEPawn::MoveRight(float Value)
  {
    if (Value == 0.0f || !m_CameraComponent)
      return;

    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_CurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    right = glm::vec3(rotationMatrix * glm::vec4(right, 0.0f));

    glm::vec3 movement = (right * Value * m_MoveSpeed);
    glm::vec3 newLocation = GetActorLocation() + movement;

    SetActorLocation(newLocation);
  }

  void CEPawn::LookHorizontal(float Value)
  {
    if (Value == 0.0f)
      return;

    m_CurrentRotation.y -= Value * m_LookSensitivity;  // именно отнимать, если сделать += то будет инвертированное управление лево\право

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

    if (!bIsInvertMouseLook)
    {
      m_CurrentRotation.x -= Value * m_LookSensitivity;
    }
    else
    {
      m_CurrentRotation.x += Value * m_LookSensitivity;
    }

    m_CurrentRotation.x = glm::clamp(m_CurrentRotation.x, -89.0f, 89.0f);

    if (m_CameraComponent)
    {
      m_CameraComponent->SetRotation(glm::vec3(m_CurrentRotation.x, m_CurrentRotation.y, 0.0f));
    }
  }
}  // namespace CE