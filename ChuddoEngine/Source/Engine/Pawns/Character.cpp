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
