#include "Engine/GamePlay/Actors/Character.h"
#include "Engine/GamePlay/Components/StaticMeshComponent.h"
#include "Engine/GamePlay/World/Levels/Level.h"



  CCharacter::CCharacter(CObject* Owner, FString NewName)
      : CPawn(Owner, NewName), m_VerticalVelocity(0.0f), m_IsOnGround(false)
  {
     if(!m_InputComponent)
  {
m_InputComponent = AddSubObject<CInputComponent>("Input", this, "InputComponent");
  }

    m_Mesh = AddDefaultSubObject<CMeshComponent>("Mesh", this, "Mesh Component");
    SetRootComponent(m_Mesh);
    m_Mesh->CreateCubeMesh();
    m_Mesh->SetRelativePosition(CEMath::Vector3f(0.f, 0.f, 1.f));
    m_Mesh->SetRelativeScale(2.f);

    m_SpringArm = AddDefaultSubObject<CSpringArmComponent>("SpringArm", this, "SpringArm Component");
    m_SpringArm->AttachToComponent(m_Mesh);
    m_SpringArm->SetRelativePosition(0.f,0.9f,0.f);
    m_SpringArm->SetArmLength(5.7f);
    m_SpringArm->SetCameraLag(0.5f);
    m_SpringArm->SetUsePawnControlRotation(true);
    m_Camera = AddDefaultSubObject<CCameraComponent>("Camera", this, "Camera Component");
    m_Camera->AttachToComponent(m_SpringArm);
    m_Camera->SetFieldOfView(60.f);
    m_Camera->SetRelativePosition(0.f,0.f,0.f);
    m_Camera->SetRelativeRotation(CEMath::Vector3f(0.0f, 0.0f, 0.0f));



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

      CEMath::Vector3f forward = GetViewForwardVector();
      forward.y = 0.0f; // Keep movement on horizontal plane
      forward = forward.Normalized();
      CEMath::Vector3f delta = forward * Value * 5.0f * 0.016f; // Increased speed for testing
      CEMath::Vector3f currentLoc = GetActorLocation();
      CEMath::Vector3f newLocation = currentLoc + delta;

      SetActorLocation(newLocation);

    }
  }

  void CCharacter::MoveRight(float Value)
  {
    if (Value != 0.0f)
    {

      CEMath::Vector3f right = -GetViewRightVector();
      right.y = 0.0f; // Keep movement on horizontal plane
      right = right.Normalized();
      CEMath::Vector3f delta = right * Value * 5.0f * 0.016f; // Increased speed for testing
      CEMath::Vector3f currentLoc = GetActorLocation();
      CEMath::Vector3f newLocation = currentLoc + delta;

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