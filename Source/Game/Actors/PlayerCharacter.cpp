#include "Game/Actors/PlayerCharacter.h"

PlayerCharacter::PlayerCharacter(CE::CEObject* Owner, CE::FString NewName) : CE::CECharacter(Owner, NewName)
{
  // Настраиваем управление вращением

  // Создаем SpringArm компонент
  m_SpringArm = AddDefaultSubObject<CE::SpringArmComponent>("SpringArm", this, "SpringArmComponent");
  m_SpringArm->SetTargetOffset(glm::vec3(0.0f, 0.7f, 0.0f));
  m_SpringArm->SetArmLength(5.0f);
  m_SpringArm->SetCameraLag(0.05f);
  SetUseControllerRotation(true);  // SpringArm будет управлять только камерой
  m_SpringArm->SetUsePawnControlRotation(true);
  m_SpringArm->SetPitchLimits(-70.0f, 70.0f);

  // Аттачим SpringArm к корневому компоненту
  m_SpringArm->AttachToComponent(m_CapsuleComponent);

  // Создаем камеру и аттачим к SpringArm
  m_Camera = AddDefaultSubObject<CE::CameraComponent>("Camera", this, "CameraComponent");
  m_Camera->AttachToComponent(m_SpringArm);

  // Настраиваем камеру
  m_Camera->SetRelativePosition(0.0f, 0.0f, 0.0f);
  m_Camera->SetRelativeRotation(0.0f, 0.0f, 0.0f);
  m_Camera->SetFieldOfView(60.0f);
  m_Camera->SetAspectRatio(16.0f / 9.0f);
  m_Camera->SetNearPlane(0.1f);
  m_Camera->SetFarPlane(100.0f);

  // Создаем простую меш-модель
  m_MeshComponent->CreateCubeMesh();
}

void PlayerCharacter::BeginPlay()
{
  CE::CECharacter::BeginPlay();
}

void PlayerCharacter::Tick(float DeltaTime)
{
  // Сначала обновляем SpringArm с текущим управляющим вращением
  if (m_SpringArm && m_SpringArm->GetUsePawnControlRotation())
  {
    m_SpringArm->SetRelativeRotation(GetControlRotation());
  }

  // Затем вызываем родительский Tick (который применит движение)
  CE::CECharacter::Tick(DeltaTime);
}

void PlayerCharacter::MoveForward(float Value)
{
  if (abs(Value) > 0.1f)
  {
    glm::vec3 Forward = GetViewForwardVector();
    Forward.y = 0;
    Forward = glm::normalize(Forward);

    AddMovementInput(Forward, Value * m_MoveSpeed);
  }
}

void PlayerCharacter::MoveRight(float Value)
{
  if (abs(Value) > 0.1f)
  {
    glm::vec3 Right = GetViewRightVector();

    Right.y = 0;
    Right = glm::normalize(Right);

    AddMovementInput(Right, Value * m_MoveSpeed);
  }
}

void PlayerCharacter::LookHorizontal(float Value)
{
  if (abs(Value) > 0.001f)
  {
    AddControllerYawInput(Value * m_MouseSensitivity);
  }
}

void PlayerCharacter::LookVertical(float Value)
{
  if (abs(Value) > 0.001f)
  {
    AddControllerPitchInput(Value * m_MouseSensitivity);
  }
}

void PlayerCharacter::Jump()
{
  if (m_IsOnGround)
  {
    m_VerticalVelocity = 6.0f;
  }
}

void PlayerCharacter::SetupPlayerInputComponent()
{
  CE::CECharacter::SetupPlayerInputComponent();

  if (GetInputComponent())
  {
    CE_CORE_DEBUG("Setting up player input component");

    GetInputComponent()->BindAxis("MoveForward", [this](float Value)
                                  { MoveForward(Value); });

    GetInputComponent()->BindAxis("MoveRight", [this](float Value)
                                  { MoveRight(Value); });

    GetInputComponent()->BindAxis("LookHorizontal", [this](float Value)
                                  { LookHorizontal(Value); });

    GetInputComponent()->BindAxis("LookVertical", [this](float Value)
                                  { LookVertical(Value); });

    GetInputComponent()->BindAction("Jump", CE::EInputEvent::Pressed, [this]()
                                    { Jump(); });

    CE_CORE_DEBUG("Player input bindings setup complete");
  }
}