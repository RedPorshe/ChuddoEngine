#pragma once
#include "Pawns/Pawn.h"

class CCharacter : public CPawn
{
public:
	CCharacter(const CObject* Owner, const std::string& inName = "Character");
	explicit CCharacter(const std::string& inName = "Character");
	virtual ~CCharacter();

	// Lifecycle
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Movement
	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveUp(float Value);

	// Rotation/Looking
	void LookUp(float Value);
	void Turn(float Value);

	// Jump
	virtual void Jump();
	virtual void StopJumping();
	bool IsJumping() const { return bIsJumping; }

	// Sprint
	void StartSprinting();
	void StopSprinting();
	bool IsSprinting() const { return bIsSprinting; }

	// Movement properties
	void SetMaxWalkSpeed(float NewSpeed) { MaxWalkSpeed = NewSpeed; }
	float GetMaxWalkSpeed() const { return MaxWalkSpeed; }
	
	void SetMaxSprintSpeed(float NewSpeed) { MaxSprintSpeed = NewSpeed; }
	float GetMaxSprintSpeed() const { return MaxSprintSpeed; }

	// Current velocity
	FVector GetCharacterVelocity() const { return CharacterVelocity; }

protected:
	// Movement parameters
	float MaxWalkSpeed = 600.0f;
	float MaxSprintSpeed = 1000.0f;
	float AccelerationRate = 2000.0f;
	float DecelerationRate = 1500.0f;
	
	// Rotation parameters
	float LookUpRate = 45.0f;
	float TurnRate = 45.0f;

	// State
	FVector CharacterVelocity = FVector::Zero();
	bool bIsJumping = false;
	bool bIsSprinting = false;

	FVector DesiredMovementDirection = FVector::Zero();
	float DesiredRotationPitch = 0.0f;
	float DesiredRotationYaw = 0.0f;

};
