#include "Engine/GamePlay/Actors/Pawn.h"

#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/GamePlay/Components/InputComponent.h"
#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/GamePlay/Components/SpringArmComponent.h"
#include "Engine/GamePlay/Input/InputSystem.h"

namespace CE
{
  CEPawn::CEPawn(CObject* Owner, FString NewName)
      : CEActor(Owner, NewName)
  {
    auto* mesh = AddDefaultSubObject<MeshComponent>("Mesh", this, "MeshComponent");
    SetRootComponent(mesh);
    mesh->SetMesh("Assets/Meshes/test_cube.obj");
    m_InputComponent = AddSubObject<InputComponent>("Input", this, "InputComponent");
    auto* m_cameraComponent = AddSubObject<CameraComponent>("Camera", this, "CameraComponent");
    auto* m_springArmComponent = AddSubObject<SpringArmComponent>("SpringArm", this, "SpringArmComponent");
    m_springArmComponent->AttachToComponent(m_RootComponent);
    m_springArmComponent->SetRelativePosition(glm::vec3(0.0f, 1.0f, 0.0f));
    m_springArmComponent->SetRelativeRotation(glm::vec3(-10.0f, 0.0f, 0.0f));
    m_springArmComponent->SetArmLength(5.0f);

    m_cameraComponent->AttachToComponent(m_springArmComponent);

    m_cameraComponent->SetFieldOfView(60.0f);
    m_cameraComponent->SetRelativeRotation(glm::vec3(0.0f, 0.0f, 0.0f));

    SetupPlayerInputComponent();
    CE_CORE_DEBUG("CEPawn created: {}", NewName.c_str());
  }

  void CEPawn::BeginPlay()
  {
    CEActor::BeginPlay();
  }
  void CEPawn::ApplyRotationToActor()
  {
    if (m_bUseControllerRotation)
    {
      auto currentRotation = this->GetActorRotation();
      float newYaw = m_ControlRotation.y;
      this->SetActorRotation(glm::vec3(currentRotation.x, newYaw, currentRotation.z));
      float newPitch = m_ControlRotation.x;
      this->SetActorRotation(glm::vec3(newPitch, currentRotation.y, currentRotation.z));
    }
    else
    {
      // Если не использовать вращение контроллера, можно реализовать другую логику
    }
  }
  void CEPawn::ApplyMovementInputToActor()
  {
    if (glm::length(m_MovementInput) > 0.01f)
    {
      // Логика перемещения уже реализована в Tick, здесь можно оставить пустым
      glm::vec3 finalMovement = m_MovementInput;
      finalMovement.y *= -1.0f;
      if (m_bUseControllerRotation)
      {
        finalMovement = m_MovementInput;
        finalMovement.y *= -1.0f;
        glm::vec3 forward = this->GetActorForwardVector();
        glm::vec3 right = this->GetActorRightVector();
        forward.y = 0.0f;
        right.y = 0.0f;

        if (glm::length(forward) > 0.01f)
          forward = glm::normalize(forward);
        if (glm::length(right) > 0.01f)
          right = glm::normalize(right);
        finalMovement = forward * m_MovementInput.z + right * m_MovementInput.x;
      }
      else
      {
        glm::vec3 forward = GetViewForwardVector();
        glm::vec3 right = GetViewRightVector();

        forward.y = 0.0f;
        right.y = 0.0f;

        if (glm::length(forward) > 0.01f)
          forward = glm::normalize(forward);
        if (glm::length(right) > 0.01f)
          right = glm::normalize(right);

        finalMovement = forward * m_MovementInput.z + right * m_MovementInput.x;
      }

      m_RootComponent->Move(finalMovement);
    }
  }

  void CEPawn::Tick(float DeltaTime)
  {
    CEActor::Tick(DeltaTime);

    ApplyRotationToActor();
    ApplyMovementInputToActor();
    ConsumeMovementInput();
    if (m_bIsJumping)
    {
      if (m_RootComponent)
      {
        glm::vec3 currentLocation = GetRootComponent()->GetWorldLocation();
        currentLocation.y += 0.5f;  // Высота прыжка
        GetRootComponent()->SetPosition(currentLocation);
      }
      m_bIsJumping = false;
    }
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

    glm::vec3 pos = GetActorLocation() + glm::vec3(0.0f, -0.1f, 0.0f);
    return pos;
  }

  glm::vec3 CEPawn::GetViewForwardVector() const
  {
    if (CameraComponent* camera = FindCameraComponent())
    {
      glm::vec3 forward = camera->GetCameraForwardVector();
      return forward;
    }

    return glm::vec3(0.0f, 0.0f, -1.0f);
  }

  glm::vec3 CEPawn::GetViewRightVector() const
  {
    if (CameraComponent* camera = FindCameraComponent())
    {
      glm::vec3 right = camera->GetCameraRightVector();
      return right;
    }
    return glm::vec3(1.0f, 0.0f, 0.0f);
  }

  void CEPawn::MoveForward(float Value)
  {
    if (Value != 0.0f)
    {
      Value *= .20f;
      if (m_bUseControllerRotation)
      {
        glm::vec3 forward = this->GetActorForwardVector();
        forward.y = 0.0f;
        if (glm::length(forward) > 0.01f)
          forward = glm::normalize(forward);
        AddMovementInput(forward, Value);
      }
      else
      {
        glm::vec3 forward = GetViewForwardVector();
        forward.y = 0.0f;
        if (glm::length(forward) > 0.01f)
          forward = glm::normalize(forward);
        AddMovementInput(forward, Value);
      }
    }
  }
  void CEPawn::lookUp(float Value)
  {
    AddControllerPitchInput(Value);
  }
  void CEPawn::turn(float Value)
  {
    AddControllerYawInput(Value);
  }
  void CEPawn::jump()
  {
    if (!m_bIsJumping)
      m_bIsJumping = true;
  }
  void CEPawn::MoveRight(float Value)
  {
    if (Value != 0.0f)
    {
      Value *= .20f;
      if (m_bUseControllerRotation)
      {
        glm::vec3 right = this->GetActorRightVector();
        right.y = 0.0f;
        right.x *= -1.0f;
        if (glm::length(right) > 0.01f)
          right = glm::normalize(right);
        AddMovementInput(right, Value);
      }
      else
      {
        glm::vec3 right = GetViewRightVector();
        right.y = 0.0f;
        right.x *= -1.0f;
        if (glm::length(right) > 0.01f)
          right = glm::normalize(right);

        AddMovementInput(right, Value);
      }
    }
  }

  glm::vec3 CEPawn::GetViewUpVector() const
  {
    if (CameraComponent* camera = FindCameraComponent())
    {
      return camera->GetCameraUpVector();
    }

    return glm::vec3(0.0f, 1.0f, 0.0f);
  }

  void CEPawn::OnPossess()
  {
    SetupPlayerInputComponent();
  }

  void CEPawn::SetupPlayerInputComponent()
  {
    if (m_InputComponent)
    {
      m_InputComponent->BindAxis("MoveForward", [this](float Value)
                                 { MoveForward(Value); });
      m_InputComponent->BindAxis("MoveRight", [this](float value)
                                 { MoveRight(value); });
      m_InputComponent->BindAxis("LookHorizontal", [this](float value)
                                 { turn(value); });
      m_InputComponent->BindAxis("LookVertical", [this](float value)
                                 { lookUp(value); });
      m_InputComponent->BindAction("Jump", EInputEvent::Pressed, [this]()
                                   { jump(); });
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