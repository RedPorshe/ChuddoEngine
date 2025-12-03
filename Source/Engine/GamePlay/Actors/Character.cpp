#include "Engine/GamePlay/Actors/Character.h"
#include "Engine/GamePlay/Components/StaticMeshComponent.h"
#include "Engine/GamePlay/World/Levels/Level.h"
#include <iostream>



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
    m_Mesh->SetRelativePosition(FVector(0.f, 0.f, 1.f));
    m_Mesh->SetRelativeScale(2.f);

    m_SpringArm = AddDefaultSubObject<CSpringArmComponent>("SpringArm", this, "SpringArm Component");
    m_SpringArm->AttachToComponent(m_Mesh);
    m_SpringArm->SetRelativePosition(0.f,0.9f,0.f);
    m_SpringArm->SetArmLength(5.7f);
    m_SpringArm->SetCameraLag(0.5f);
    m_SpringArm->SetUsePawnControlRotation(true);
    m_Camera = AddDefaultSubObject<CCameraComponent>("Camera", this, "Camera Component");
    m_Camera->AttachToComponent(m_SpringArm);
    m_Camera->SetFieldOfView(120.f);
    m_Camera->SetFarPlane(50000.0f); // Increase drawing distance significantly
    m_Camera->SetRelativePosition(0.f,0.f,0.f);
    m_Camera->SetRelativeRotation(FVector(0.0f, 0.0f, 0.0f));



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
   
   // AddControllerYawInput(DeltaTime * 50.0f);  // Continuously rotate the control yaw to spin the character and camera
  }

  void CCharacter::SetupPlayerInputComponent()
  {
    std::cout << "SetupPlayerInputComponent called" << std::endl;
    CPawn::SetupPlayerInputComponent();

    if (GetInputComponent())
    {
      std::cout << "Binding axes" << std::endl;
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
    else
    {
      std::cout << "No InputComponent" << std::endl;
    }
  }

  void CCharacter::MoveForward(float Value)
  {
    if (Value != 0.0f)
    {
      CORE_DEBUG("MoveForward called with Value: ", Value);

      FVector forward = GetViewForwardVector();
      forward.y = 0.0f; // Keep movement on horizontal plane
      forward = forward.Normalized();
      FVector delta = forward * Value * 50.0f * 0.016f; // Increased speed for testing
      FVector currentLoc = GetActorLocation();
      FVector newLocation = currentLoc + delta;

      CORE_DEBUG("Moving from (", currentLoc.x, ", ", currentLoc.y, ", ", currentLoc.z, ") to (", newLocation.x, ", ", newLocation.y, ", ", newLocation.z, ")");
      SetActorLocation(newLocation);

    }
  }

  void CCharacter::MoveRight(float Value)
  {
    if (Value != 0.0f)
    {

      FVector right = -GetViewRightVector();
      right.y = 0.0f; // Keep movement on horizontal plane
      right = right.Normalized();
      FVector delta = right * Value * 5.0f * 0.016f; // Increased speed for testing
      FVector currentLoc = GetActorLocation();
      FVector newLocation = currentLoc + delta;

      SetActorLocation(newLocation);
    }
  }

  void CCharacter::Jump()
  {
    if (m_IsOnGround && !bIsJumping)
    {
      bIsJumping = true;
      m_VerticalVelocity = 10.0f; // Jump impulse
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