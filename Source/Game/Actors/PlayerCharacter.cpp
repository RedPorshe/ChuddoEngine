
#include "Game/Actors/PlayerCharacter.h"

PlayerCharacter::PlayerCharacter(CE::CEObject* Owner, CE::FString NewName) : CE::CECharacter(Owner, NewName)
{
  // Создаем SpringArm компонент
  m_SpringArm = AddDefaultSubObject<CE::SpringArmComponent>("SpringArm", this, "SpringArmComponent");
  m_SpringArm->SetTargetOffset(glm::vec3(0.0f, 0.9f, 0.0f));
  m_SpringArm->SetArmLength(5.0f);
  m_SpringArm->SetCameraLag(0.05f);
  m_SpringArm->AttachToComponent(m_CapsuleComponent);  // SpringArm приаттачен к капсуле

  // Создаем камеру и аттачим к SpringArm
  m_Camera = AddDefaultSubObject<CE::CameraComponent>("Camera", this, "CameraComponent");
  m_Camera->AttachToComponent(m_SpringArm);  // Камера приаттачена к SpringArm (ВАЖНО!)

  // Настраиваем камеру - используем относительные координаты
  m_Camera->SetRelativePosition(0.0f, 0.0f, 0.0f);  // Относительно SpringArm
  m_Camera->SetFieldOfView(60.0f);
  m_Camera->SetAspectRatio(16.0f / 9.0f);
  m_Camera->SetNearPlane(0.1f);
  m_Camera->SetFarPlane(100.0f);
  m_MeshComponent->CreateCubeMesh();
}

void PlayerCharacter::BeginPlay()
{
  CE::CECharacter::BeginPlay();
}

void PlayerCharacter::Tick(float DeltaTime)
{
  CE::CECharacter::Tick(DeltaTime);
}

void PlayerCharacter::MoveForward(float Value)
{
  if (m_CapsuleComponent && abs(Value) > 0.1f)  // Используем CapsuleComponent
  {
    // Получаем направление от SpringArm (камеры)
    glm::vec3 forward = m_SpringArm->GetForwardVector();
    forward.y = 0.0f;
    forward = glm::normalize(forward);

    glm::vec3 movement = forward * (Value * 0.020f);
    m_CapsuleComponent->Move(movement);
  }
}

void PlayerCharacter::MoveRight(float Value)
{
  if (m_CapsuleComponent && abs(Value) > 0.1f)  // Используем CapsuleComponent
  {
    // Получаем направление от SpringArm (камеры)
    glm::vec3 right = m_SpringArm->GetRightVector();
    right.y = 0.0f;
    right = glm::normalize(right);

    glm::vec3 movement = right * (Value * 0.020f);
    m_CapsuleComponent->Move(movement);
  }
}

void PlayerCharacter::LookHorizontal(float Value)
{
  if (m_SpringArm && abs(Value) > 0.001f)
  {
    // Вращаем SpringArm по горизонтали (относительно родителя)
    glm::vec3 rotation = m_SpringArm->GetRelativeRotation();
    rotation.y += Value * m_MouseSensitivity;
    m_SpringArm->SetRelativeRotation(rotation);

    CE_CORE_DEBUG("LookHorizontal: SpringArm Y=", rotation.y);
  }
}

void PlayerCharacter::LookVertical(float Value)
{
  if (m_SpringArm && abs(Value) > 0.001f)
  {
    // Вращаем SpringArm по вертикали (относительно родителя)
    glm::vec3 rotation = m_SpringArm->GetRelativeRotation();
    rotation.x += Value * m_MouseSensitivity;
    rotation.x = glm::clamp(rotation.x, -180.0f, 180.0f);
    m_SpringArm->SetRelativeRotation(rotation);

    CE_CORE_DEBUG("LookVertical: SpringArm X=", rotation.x);
  }
}

void PlayerCharacter::SetupPlayerInputComponent()
{
  CE::CECharacter::SetupPlayerInputComponent();
  if (GetInputComponent())
  {
    // Биндинги управления для персонажа
    GetInputComponent()->BindAxis("MoveForward", [this](float Value)
                                  { MoveForward(Value); });

    GetInputComponent()->BindAxis("MoveRight", [this](float Value)
                                  { MoveRight(Value); });

    GetInputComponent()->BindAxis("LookHorizontal", [this](float Value)
                                  { LookHorizontal(Value); });

    GetInputComponent()->BindAxis("LookVertical", [this](float Value)
                                  { LookVertical(Value); });

    GetInputComponent()->BindAction("Jump", CE::EInputEvent::Pressed, [this]()
                                    {
        if (m_IsOnGround)
        {
          // Прыжок вверх (положительное значение Y)
          m_VerticalVelocity = 4.0f; // Импульс вверх
          CE_CORE_DEBUG("JUMP! Vertical velocity: ", m_VerticalVelocity);
        } });

    CE_CORE_DEBUG("Character input bindings setup complete");
  }
}