#pragma once
#include "Core/Object.h"

class CSceneComponent;
class CBaseComponent;

class CActor : public CObject
{
	public:
	CActor(const CObject* Owner, const std::string& inName = "Actor");
	explicit CActor(const std::string& inName = "Actor");
	virtual ~CActor();
	// Actor-specific virtual methods
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	//setters
	//status setters
	void SetActive(bool bNewActive) { bIsActive = bNewActive; }
	void SetVisible(bool bNewVisible) { bIsVisible = bNewVisible; }
	void SetCanTick(bool bNewCanTick) { bIsCanTick = bNewCanTick; }
	void SetPendingDestroy(bool bNewPendingDestroy) { bIsPendingDestroy = bNewPendingDestroy; }
	//transform setters
	// Transform
	void SetActorTransform(const FTransform& NewTransform);
	void SetActorTransform(const FVector& NewLocation, const FQuat& NewRotation, const FVector& NewScale);
	void SetActorTransform(const FVector& NewLocation, const FVector& NewRotationEulerDegrees, const FVector& NewScale);

	//location
	void SetActorLocation(const FVector& NewLocation);
	void SetActorLocation(float X, float Y, float Z);
	// Rotation (Quaternion-based)
	void SetActorRotation(const FQuat& RotationQuaternion);
	void SetActorRotation(const FMat4& RotationMatrix);
	void SetActorRotation(float Pitch, float Yaw, float Roll);  // Euler angles in degrees
	// Scale
	void SetActorScale(const FVector& NewScale);
	void SetActorScale(float ScaleX, float ScaleY, float ScaleZ);
	void SetActorScale(float UniformScale);
	
	//getters
	// Transform
	FTransform GetActorTransform() const;
	//location
	FVector GetActorLocation() const;
	// Rotation (returns Quaternion; use GetActorRotationEuler for degrees)
	FQuat GetActorRotationQuat() const;
	FVector GetActorRotationEuler() const;  // Returns Euler angles in degrees
	// Scale
	FVector GetActorScale() const;
	//status checkers
	bool IsActive() const;
	bool IsVisible() const;
	bool IsCanTick() const;
	bool IsPendingDestroy() const;

	// Move funcs
	// move
	void MoveActor(const FVector& DeltaLocation);
	void MoveActor(float DeltaX, float DeltaY, float DeltaZ);
	// rotate (all accept Euler angles in degrees for user convenience)
	void RotateActor(float DeltaPitch, float DeltaYaw, float DeltaRoll);
	void RotateActor(const FVector& DeltaRotationEuler);  // Euler angles in degrees
	void RotateActor(const FMat4& DeltaRotationMatrix);
	void RotateActor(const FQuat& DeltaRotationQuaternion);
	
	// scale
	void ScaleActor(const FVector& DeltaScale);
	void ScaleActor(float DeltaScaleX, float DeltaScaleY, float DeltaScaleZ);
	void ScaleActor(float UniformDeltaScale);

	// Vector getters
	FVector GetActorForwardVector() const;
	FVector GetActorRightVector() const;
	FVector GetActorUpVector() const;

	


protected:	
	FTransform ActorTransform;
	bool bIsInitialized = false;
	bool bIsActive = true;
	bool bIsVisible = true;
	bool bIsCanTick = true;
	bool bIsPendingDestroy = false;
	CSceneComponent * rootComponent;
	std::vector<CBaseComponent *> m_components;
};
