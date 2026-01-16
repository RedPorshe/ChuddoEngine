#pragma once
#include "Actors/Actor.h"

// Forward declaration - avoid circular dependency
class CController;

class CPawn : public CActor
{
public:
	CPawn(const CObject* Owner, const std::string& inName = "Pawn");
	explicit CPawn(const std::string& inName = "Pawn");
	virtual ~CPawn();

	// Lifecycle
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Controller management
	void SetController(CController* NewController);
	CController* GetController() const { return Controller; }
	bool HasController() const { return Controller != nullptr; }
	void PossessedBy(CController* NewController);
	void UnPossessed();
	virtual void OnPossessed();
	virtual void OnUnPossessed();

	// Input handling
	virtual void SetupPlayerInputComponent();

	// Movement input from controller
	virtual void AddMovementInput(const FVector& InputDirection, float Scale = 1.0f);
	virtual void AddRotationInput(float Pitch, float Yaw);

	// State
	void SetCanBeControlled(bool bNewCanBeControlled) { bCanBeControlled = bNewCanBeControlled; }
	bool CanBeControlled() const { return bCanBeControlled; }

protected:
	CController* Controller = nullptr;
	bool bCanBeControlled = true;

	// Accumulated input for this frame
	FVector AccumulatedMovementInput = FVector::Zero();
	float AccumulatedPitchInput = 0.0f;
	float AccumulatedYawInput = 0.0f;

	
	void ProcessMovementInput(float DeltaTime);
	void ProcessRotationInput(float DeltaTime);
};
