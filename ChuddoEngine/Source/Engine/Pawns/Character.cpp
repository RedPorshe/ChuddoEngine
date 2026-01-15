#include "Pawns/Character.h"
#include "Utils/Math/CE_MathHelpers.h"

CCharacter::CCharacter(const CObject* Owner, const std::string& inName) : CPawn(Owner, inName)
{
	std::cout << "Character '" << GetName() << "' created." << std::endl;
}

CCharacter::CCharacter(const std::string& inName) : CPawn(inName)
{
	std::cout << "Character '" << GetName() << "' created without owner." << std::endl;
}

CCharacter::~CCharacter()
{
	std::cout << "Character '" << GetName() << "' destroyed." << std::endl;
}

void CCharacter::BeginPlay()
{
	std::cout << "Begin Play on Character: " << GetName() << std::endl;
	CPawn::BeginPlay();
}

void CCharacter::Tick(float DeltaTime)
{
	std::cout << "Tick on Character: " << GetName() << " Velocity: " << CharacterVelocity << std::endl;
	CPawn::Tick(DeltaTime);
}

void CCharacter::MoveForward(float Value)
{
	if (!bCanBeControlled || !HasController())
		return;

	FVector ForwardDirection = GetActorForwardVector();
	AddMovementInput(ForwardDirection, Value);
}

void CCharacter::MoveRight(float Value)
{
	if (!bCanBeControlled || !HasController())
		return;

	FVector RightDirection = GetActorRightVector();
	AddMovementInput(RightDirection, Value);
}

void CCharacter::MoveUp(float Value)
{
	if (!bCanBeControlled || !HasController())
		return;

	FVector UpDirection = GetActorUpVector();
	AddMovementInput(UpDirection, Value);
}

void CCharacter::LookUp(float Value)
{
	if (!bCanBeControlled || !HasController())
		return;

	AddRotationInput(Value * LookUpRate, 0.0f);
}

void CCharacter::Turn(float Value)
{
	if (!bCanBeControlled || !HasController())
		return;

	AddRotationInput(0.0f, Value * TurnRate);
}

void CCharacter::Jump()
{
	if (!bCanBeControlled)
		return;

	if (!bIsJumping)
	{
		bIsJumping = true;
		std::cout << "Character '" << GetName() << "' jumped." << std::endl;
	}
}

void CCharacter::StopJumping()
{
	bIsJumping = false;
	std::cout << "Character '" << GetName() << "' stopped jumping." << std::endl;
}

void CCharacter::StartSprinting()
{
	if (!bCanBeControlled)
		return;

	bIsSprinting = true;
	std::cout << "Character '" << GetName() << "' started sprinting." << std::endl;
}

void CCharacter::StopSprinting()
{
	bIsSprinting = false;
	std::cout << "Character '" << GetName() << "' stopped sprinting." << std::endl;
}

void CCharacter::ProcessMovementInput(float DeltaTime)
{
	// Determine target speed based on sprint state
	float TargetSpeed = bIsSprinting ? MaxSprintSpeed : MaxWalkSpeed;

	// Calculate desired velocity from accumulated input
	// IMPORTANT: Use only horizontal components of movement input
	// This prevents camera pitch from affecting movement direction
	FVector HorizontalForward = GetActorForwardVector();
	HorizontalForward.y = 0.0f;  // Remove vertical component
	HorizontalForward = HorizontalForward.Normalized();

	FVector HorizontalRight = GetActorRightVector();
	HorizontalRight.y = 0.0f;  // Remove vertical component
	HorizontalRight = HorizontalRight.Normalized();

	// Compute desired horizontal movement direction
	FVector DesiredHorizontalDirection = FVector::Zero();
	if (!AccumulatedMovementInput.IsZero())
	{
		// Decompose input into forward/right components
		// AccumulatedMovementInput is in world space direction
		FVector NormalizedInput = AccumulatedMovementInput.Normalized();
		DesiredHorizontalDirection = NormalizedInput;
		DesiredHorizontalDirection.y = 0.0f;  // Keep only horizontal
		
		if (!DesiredHorizontalDirection.IsZero())
		{
			DesiredHorizontalDirection = DesiredHorizontalDirection.Normalized();
		}
	}

	// Apply acceleration/deceleration
	float CurrentSpeed = CharacterVelocity.Length();
	float DesiredSpeed = DesiredHorizontalDirection.IsZero() ? 0.0f : TargetSpeed;

	if (DesiredSpeed > CurrentSpeed)
	{
		// Accelerate
		CurrentSpeed = CEMath::Clamp(CurrentSpeed + AccelerationRate * DeltaTime, 0.0f, DesiredSpeed);
	}
	else if (DesiredSpeed < CurrentSpeed)
	{
		// Decelerate
		CurrentSpeed = CEMath::Clamp(CurrentSpeed - DecelerationRate * DeltaTime, 0.0f, DesiredSpeed);
	}

	// Update velocity - keep only horizontal movement
	if (!DesiredHorizontalDirection.IsZero())
	{
		CharacterVelocity = DesiredHorizontalDirection * CurrentSpeed;
	}
	else
	{
		CharacterVelocity = FVector::Zero();
	}

	// Apply movement
	if (!CharacterVelocity.IsZero())
	{
		MoveActor(CharacterVelocity * DeltaTime);
	}
}

void CCharacter::ProcessRotationInput(float DeltaTime)
{
	if (AccumulatedPitchInput != 0.0f || AccumulatedYawInput != 0.0f)
	{
		FVector CurrentRotation = GetActorRotationEuler();
		
		// Apply pitch (limit to -90 to 90 degrees)
		float NewPitch = CurrentRotation.x + AccumulatedPitchInput * DeltaTime;
		NewPitch = CEMath::Clamp(NewPitch, -90.0f, 90.0f);
		
		// Apply yaw (no limit)
		float NewYaw = CurrentRotation.y + AccumulatedYawInput * DeltaTime;
		
		SetActorRotation(NewPitch, NewYaw, CurrentRotation.z);
	}
}
