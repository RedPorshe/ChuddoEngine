
#include "Game/Actors/PlayerCharacter.h"

PlayerCharacter::PlayerCharacter(CE::CEObject* Owner, CE::FString NewName) : CE::CECharacter(Owner, NewName)
{
  // Создаем SpringArm компонент
  m_SpringArm = AddDefaultSubObject<CE::SpringArmComponent>("SpringArm", this, "SpringArmComponent");
  m_SpringArm->SetTargetOffset(glm::vec3(0.0f, 0.9f, 0.0f));  // Смещение к "голове" (90 см ВВЕРХ)
  m_SpringArm->SetArmLength(5.0f);                            // 5 метров от персонажа
  m_SpringArm->SetCameraLag(0.05f);                           // Небольшая задержка для плавности
  m_SpringArm->AttachToComponent(m_CapsuleComponent);

  // Создаем камеру и аттачим к SpringArm
  m_Camera = AddDefaultSubObject<CE::CameraComponent>("Camera", this, "CameraComponent");
  m_Camera->AttachToComponent(m_SpringArm);

  // Настраиваем камеру
  m_Camera->SetPosition(0.0f, 0.0f, 0.0f);  // Позиция относительно SpringArm
  m_Camera->SetFieldOfView(60.0f);
  m_Camera->SetAspectRatio(16.0f / 9.0f);
  m_Camera->SetNearPlane(0.1f);   // 10 см
  m_Camera->SetFarPlane(100.0f);  // 100 метров
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
  if (m_CapsuleComponent && abs(Value) > 0.1f)
  {
    glm::vec3 movement = glm::vec3(0.0f, 0.0f, Value * .020f);  // Движение вперед/назад
    m_CapsuleComponent->Move(movement);
  }
}
void PlayerCharacter::MoveRight(float Value)
{
  if (m_CapsuleComponent && abs(Value) > 0.1f)
  {
    glm::vec3 movement = glm::vec3(Value * .020f, 0.0f, 0.0f);  // Движение вправо/влево
    m_CapsuleComponent->Move(movement);
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