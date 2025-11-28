#include "Engine/GamePlay/Actors/Character.h"
#include "Engine/GamePlay/Components/StaticMeshComponent.h"
#include "Engine/GamePlay/World/Levels/Level.h"
#include "Engine/GamePlay/CollisionSystem/CapsuleComponent.h"

namespace CE
{
  CCharacter::CCharacter(CObject* Owner, FString NewName)
      : CPawn(Owner, NewName), m_VerticalVelocity(0.0f), m_IsOnGround(false)
  {
     if(!m_InputComponent)
  {
m_InputComponent = AddSubObject<CInputComponent>("Input", this, "InputComponent");
  }
    m_Capsule = AddDefaultSubObject<CCapsuleComponent>("capsule", this, "CapsuleComponent");
    m_Capsule->SetRadius(0.5f);
    m_Capsule->SetHalfHeight(1.f);
    SetRootComponent(m_Capsule);
    m_Mesh = AddDefaultSubObject<CMeshComponent>("Mesh", this, "Mesh Component");
    m_Mesh->AttachToComponent(m_Capsule);
    m_Mesh->CreateCubeMesh();
    m_Mesh->SetRelativePosition(Math::Vector3f(0.f, 0.f, 1.f));
    m_Mesh->SetRelativeScale(2.f);

    m_SpringArm = AddDefaultSubObject<CSpringArmComponent>("SpringArm", this, "SpringArm Component");
    m_SpringArm->AttachToComponent(m_Capsule);
    m_SpringArm->SetRelativePosition(0.f,0.9f,0.f);
    m_SpringArm->SetArmLength(5.7f);
    m_SpringArm->SetCameraLag(0.5f);
    m_SpringArm->SetUsePawnControlRotation(true);
    m_Camera = AddDefaultSubObject<CCameraComponent>("Camera", this, "Camera Component");
    m_Camera->AttachToComponent(m_SpringArm);
    m_Camera->SetFieldOfView(60.f);
    m_Camera->SetRelativePosition(0.f,0.f,0.f);
    m_Camera->SetRelativeRotation(Math::Vector3f(0.0f, 0.0f, 0.0f));

      

    m_IsOnGround = true; // Temporary to test jumping
    m_bUseControllerRotation = true;
  }

  void CCharacter::BeginPlay()
  {
    CPawn::BeginPlay();
  }

  void CCharacter::Tick(float DeltaTime)
  {
    CPawn::Tick(DeltaTime);

    // Apply gravity if jumping
    if (bIsJumping)
    {
      m_VerticalVelocity += GetLevel()->GetGravity().y * DeltaTime;
    }

    // Ground check using raycast
    Math::Vector3f currentLocation = GetActorLocation();
    Math::Ray groundRay(currentLocation, Math::Vector3f(0.0f, -1.0f, 0.0f));
    FRaycastHit hit;
    float groundCheckDistance = 100.0f; // Increased distance to reach terrain

    if (GetLevel()->Raycast(groundRay, hit, groundCheckDistance, this))
    {
      // On ground
      m_IsOnGround = true;
      if (m_VerticalVelocity >= 0.0f) // Only reset jumping if falling or at peak
      {
        bIsJumping = false;
        m_VerticalVelocity = 0.0f;
      }
      // Snap to ground
      float groundY = hit.Location.y;
      float newY = groundY + m_Capsule->GetHalfHeight();
      SetActorLocation(Math::Vector3f(currentLocation.x, newY, currentLocation.z));

    }
    else
    {
      // In air
      m_IsOnGround = false;
      // Apply vertical movement
      Math::Vector3f newLocation = currentLocation;
      newLocation.y += m_VerticalVelocity * DeltaTime;
      SetActorLocation(newLocation);

    }
  }

  void CCharacter::SetupPlayerInputComponent()
  {
    CPawn::SetupPlayerInputComponent();

    if (GetInputComponent())
    {
      GetInputComponent()->BindAxis("MoveForward", [this](float Value)
                                   { MoveForward(Value); });
      GetInputComponent()->BindAxis("MoveRight", [this](float value)
                                   { MoveRight(value); });
      GetInputComponent()->BindAxis("LookHorizontal", [this](float value)
                                   { Turn(value); });
      GetInputComponent()->BindAxis("LookVertical", [this](float value)
                                   { LookUp(value); });
      GetInputComponent()->BindAction("Jump", EInputEvent::Pressed, [this]()
                                     { Jump(); });
    }
  }

  void CCharacter::MoveForward(float Value)
  {
    if (Value != 0.0f)
    {

      Math::Vector3f forward = GetViewForwardVector();
      forward.y = 0.0f; // Keep movement on horizontal plane
      forward = forward.Normalized();
      Math::Vector3f delta = forward * Value * 5.0f * 0.016f; // Increased speed for testing
      Math::Vector3f currentLoc = GetActorLocation();
      Math::Vector3f newLocation = currentLoc + delta;

      SetActorLocation(newLocation);

    }
  }

  void CCharacter::MoveRight(float Value)
  {
    if (Value != 0.0f)
    {

      Math::Vector3f right = -GetViewRightVector();
      right.y = 0.0f; // Keep movement on horizontal plane
      right = right.Normalized();
      Math::Vector3f delta = right * Value * 5.0f * 0.016f; // Increased speed for testing
      Math::Vector3f currentLoc = GetActorLocation();
      Math::Vector3f newLocation = currentLoc + delta;

      SetActorLocation(newLocation);
    }
  }

  void CCharacter::Jump()
  {
    if (m_IsOnGround && !bIsJumping)
    {
      bIsJumping = true;
      m_VerticalVelocity = -10.0f; // Jump impulse
      m_IsOnGround = false;

    }
  }

  void CCharacter::LookUp(float Value)
  {
    AddControllerPitchInput(Value);
  }

  void CCharacter::Turn(float Value)
  {
    AddControllerYawInput(Value);
  }
}  // namespace CE
