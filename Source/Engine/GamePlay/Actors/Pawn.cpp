#include "Engine/GamePlay/Actors/Pawn.h"

#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/GamePlay/Components/InputComponent.h"
#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/GamePlay/Components/SpringArmComponent.h"
#include "Engine/GamePlay/Input/InputSystem.h"


  CPawn::CPawn(CObject* Owner, FString NewName)
      : CActor(Owner, NewName)
  {
    
    CORE_DEBUG("CEPawn created: {}", NewName.c_str());
  }

  void CPawn::BeginPlay()
  {
    CActor::BeginPlay();
  }
  void CPawn::ApplyRotationToActor()
  {
    if (m_bUseControllerRotation)
    {
      auto currentRotation = this->GetActorRotation();
      float newYaw = m_ControlRotation.y;
      this->SetActorRotation(FVector(currentRotation.x, newYaw, currentRotation.z));
      // Pitch handled by SpringArm
    }
    else
    {
      // Если не использовать вращение контроллера, можно реализовать другую логику
    }
  }
  void CPawn::ApplyMovementInputToActor()
  {
    
  }

  void CPawn::Tick(float DeltaTime)
  {
    CActor::Tick(DeltaTime);

    

    ApplyRotationToActor();
    ApplyMovementInputToActor();
    ConsumeMovementInput();
  }

  CCameraComponent* CPawn::FindCameraComponent() const
  {    
    CCameraComponent* camera = nullptr;
    ForEachComponent<CCameraComponent>([&camera](CCameraComponent* comp)
                                      {
      if (!camera) 
      {
        camera = comp;
       
      } });

    if (!camera)
    {
      CORE_DEBUG("No camera component found");
    }

    return camera;
  }

  FVector CPawn::GetPawnViewLocation() const
  {
    if (CCameraComponent* camera = FindCameraComponent())
    {
      FVector pos = camera->GetWorldLocation();
      return pos;
    }

    FVector pos = GetActorLocation() + FVector(0.0f, -0.1f, 0.0f);
    return pos;
  }

  FVector CPawn::GetViewForwardVector() const
  {
    if (CCameraComponent* camera = FindCameraComponent())
    {
      FVector forward = camera->GetCameraForwardVector();
      return forward;
    }

    return FVector(0.0f, 0.0f, -1.0f);
  }

  FVector CPawn::GetViewRightVector() const
  {
    if (CCameraComponent* camera = FindCameraComponent())
    {
      FVector right = camera->GetCameraRightVector();
      return right;
    }
    return FVector(1.0f, 0.0f, 0.0f);
  }

  void CPawn::MoveForward(float Value)
  {
    if (Value != 0.0f)
    {
      CORE_DEBUG("MoveForward called with value: ", Value);
      FVector forward = GetViewForwardVector();
      forward.y = 0.0f; // Keep movement on horizontal plane
      forward = forward.Normalized();
      FVector delta = forward * Value * 5.0f * 0.016f; // Increased speed for testing
      FVector currentLoc = GetActorLocation();
      FVector newLocation = currentLoc + delta;
      CORE_DEBUG("Moving pawn forward from (", currentLoc.x, ", ", currentLoc.y, ", ", currentLoc.z, ") to (", newLocation.x, ", ", newLocation.y, ", ", newLocation.z, ")");
      SetActorLocation(newLocation);
      CORE_DEBUG("Pawn location after move: (", GetActorLocation().x, ", ", GetActorLocation().y, ", ", GetActorLocation().z, ")");
    }
  }
  void CPawn::lookUp(float Value)
  {
    AddControllerPitchInput(Value);    
  }
  void CPawn::turn(float Value)
  {
    AddControllerYawInput(Value);    
  }
  void CPawn::jump()
  {
        
  }
  void CPawn::MoveRight(float Value)
  {
    if (Value != 0.0f)
    {
      CORE_DEBUG("MoveRight called with value: ", Value);
      FVector right = -GetViewRightVector();
      right.y = 0.0f; // Keep movement on horizontal plane
      right = right.Normalized();
      FVector delta = right * Value * 5.0f * 0.016f; // Increased speed for testing
      FVector currentLoc = GetActorLocation();
      FVector newLocation = currentLoc + delta;
      CORE_DEBUG("Moving pawn right from (", currentLoc.x, ", ", currentLoc.y, ", ", currentLoc.z, ") to (", newLocation.x, ", ", newLocation.y, ", ", newLocation.z, ")");
      SetActorLocation(newLocation);
    }
  }

  FVector CPawn::GetViewUpVector() const
  {
    if (CCameraComponent* camera = FindCameraComponent())
    {
      return camera->GetCameraUpVector();
    }

    return FVector(0.0f, 1.0f, 0.0f);
  }

  void CPawn::OnPossess()
  {
    SetupPlayerInputComponent();
  }

  void CPawn::SetupPlayerInputComponent()
  {
    // Empty implementation, logic moved to CCharacter
  }

  void CPawn::AddMovementInput(const FVector& WorldDirection, float ScaleValue, bool bForce)
  {
    if (ScaleValue != 0.0f && (bForce || m_RootComponent != nullptr))
    {
      (void)WorldDirection;
      (void)ScaleValue;
      (void)bForce;
    }
  }

  void CPawn::AddControllerYawInput(float Value)
  {
    m_ControlRotation.x += Value;
    while (m_ControlRotation.x > 180.0f)
      m_ControlRotation.x -= 360.0f;
    while (m_ControlRotation.x < -180.0f)
      m_ControlRotation.x += 360.0f;
  }

  void CPawn::AddControllerPitchInput(float Value)
  {
    m_ControlRotation.y += Value;
    m_ControlRotation.y = std::clamp(m_ControlRotation.y, -89.0f, 89.0f);
  }

  void CPawn::SetControlRotation(const FVector& NewRotation)
  {
    m_ControlRotation = NewRotation;
    m_ControlRotation.y = std::clamp(m_ControlRotation.y, -89.0f, 89.0f);
  }

  void CPawn::ConsumeMovementInput()
  {
    m_MovementInput = FVector(0.0f);
    m_bMovementInputConsumed = true;
  }
