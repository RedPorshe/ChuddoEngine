#pragma once
#include "Core/Object.h"



class CActor : public CObject
{
	public:
	CActor(const CObject* Owner, const std::string& inName = "Actor");
	explicit CActor(const std::string& inName = "Actor");
	virtual ~CActor();
	// Actor-specific methods can be added here
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

	void SetActive(bool bNewActive) { bIsActive = bNewActive; }
	bool IsActive() const { return bIsActive; }
	void SetVisible(bool bNewVisible) { bisVisible = bNewVisible; }
	bool IsVisible() const { return bisVisible; }
	void SetCanTick(bool bNewCanTick) { bIsCanTick = bNewCanTick; }
	bool IsCanTick() const { return bIsCanTick; }
	void SetPendingDestroy(bool bNewPendingDestroy) { bIsPendingDestroy = bNewPendingDestroy; }
	bool IsPendingDestroy() const { return bIsPendingDestroy; }
	FTransform GetActorTransform() const { return ActorTransform; }
	void SetActorTransform(const FTransform& NewTransform) { ActorTransform = NewTransform; }
	void SetActorLocation(const FVector& NewLocation) { ActorTransform.Location = NewLocation; }
	FVector GetActorLocation() const { return ActorTransform.Location; }
	void SetActorRotation(const FQuat& NewRotation) { ActorTransform.Rotation = NewRotation; }
	FQuat GetActorRotation() const { return ActorTransform.Rotation; }
	void SetActorScale(const FVector& NewScale) { ActorTransform.Scale = NewScale; }
	FVector GetActorScale() const { return ActorTransform.Scale; }
	void SetActorScale(float UniformScale) { ActorTransform.Scale = FVector(UniformScale, UniformScale, UniformScale); }
	void SetActorRotation(float Pitch, float Yaw, float Roll);
	


protected:	
	FTransform ActorTransform;
	bool bIsInitialized = false;
	bool bIsActive = true;
	bool bisVisible = true;
	bool bIsCanTick = true;
	bool bIsPendingDestroy = false;
};
