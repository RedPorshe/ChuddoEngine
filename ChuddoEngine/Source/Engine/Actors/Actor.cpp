#include "Actors/Actor.h"
#include "Components/BaseComponent.h"
#include "Components/SceneComponent.h"

#include "Utils/Math/CE_MathHelpers.h"

CActor::CActor ( const CObject * Owner, const std::string & inName ) : CObject ( Owner, inName )
	{
	std::cout << "Actor '" << GetName () << "' created." << std::endl;
	SetCanTick ( true );
	rootComponent = new CSceneComponent ( this, "RootComp" );
	auto meshcomp = new CSceneComponent ( this, "mesh comp" );
	auto meshcomp2 = new CSceneComponent ( this, "mesh comp2" );
	auto meshcomp3 = new CSceneComponent ( this, "mesh comp3" );
	m_components.push_back ( rootComponent );
	m_components.push_back ( meshcomp );
	m_components.push_back ( meshcomp2 );
	m_components.push_back ( meshcomp3 );
	}
CActor::CActor ( const std::string & inName ) : CObject ( inName )
	{
	std::cout << "Actor '" << GetName () << "' created. Without Owner" << std::endl;
	SetCanTick ( true );
	}
CActor::~CActor ()
	{
	std::cout << "Actor '" << GetName () << "' destroyed." << std::endl;
	}
void CActor::BeginPlay ()
	{
	std::cout << typeid( *this ).name () << " with name '" << this->GetName () << "' is  Begin play.\n";
	if (HasOwnedObjects ()) // for consistency
		{
		for (size_t i = 0; i < GetOwnedObjectsCount (); ++i)
			{
			CObject * Obj = OwnedObjects[ i ].get ();
			if (Obj)
				{
				CBaseComponent * ActorObj = dynamic_cast< CBaseComponent * > ( Obj );
				if (ActorObj)
					{
					std::cout << "Starting BeginPlay on owned object: " << ActorObj->GetName () << " Owner is : " << GetName () << std::endl;
					ActorObj->BeginPlay ();
					}
				}
			}

		}
	for (auto comp : m_components)
		{
		if (comp != nullptr)
			{
			comp->BeginPlay ();
			}
		}
	bIsInitialized = true;
	}
void CActor::Tick ( float DeltaTime )
	{
	std::cout << typeid( *this ).name () << " with name '" << this->GetName () << "' is tick with delta time : " << DeltaTime << "\n";
	if (bIsCanTick && bIsInitialized)
		{		
		if (HasOwnedObjects ())
			{
			for (size_t i = 0; i < GetOwnedObjectsCount (); ++i)
				{
				CObject * Obj = OwnedObjects[ i ].get ();
				if (Obj)
					{
					CBaseComponent * ActorObj = dynamic_cast< CBaseComponent * > ( Obj );
					if (ActorObj)
						{
						std::cout << "Ticking owned Component: " << ActorObj->GetName () << " Owner is : " << GetName () << std::endl;
						if(ActorObj->IsCanTick())
							{
							ActorObj->Tick ( DeltaTime );
							}
						}
					}
				}

			}		
		for (auto comp : m_components)
			{
			if (comp != nullptr && comp->IsCanTick())
				{
				comp->Tick ( DeltaTime );
				}
			}
		}
	}

void CActor::SetActorTransform ( const FTransform & NewTransform )
	{
	ActorTransform = NewTransform;
	}

void CActor::SetActorTransform ( const FVector & NewLocation, const FQuat & NewRotation, const FVector & NewScale )
	{
	ActorTransform.Location = NewLocation;
	ActorTransform.Rotation = NewRotation;
	ActorTransform.Scale = NewScale;
	}

void CActor::SetActorTransform ( const FVector & NewLocation, const FVector & NewRotationEulerDegrees, const FVector & NewScale )
	{
	ActorTransform.Location = NewLocation;
	FVector eulerRad (
		CEMath::DegreesToRadians ( NewRotationEulerDegrees.x ),
		CEMath::DegreesToRadians ( NewRotationEulerDegrees.y ),
		CEMath::DegreesToRadians ( NewRotationEulerDegrees.z )
	);
	ActorTransform.Rotation = FQuat::FromEulerAngles ( eulerRad.x, eulerRad.y, eulerRad.z );
	ActorTransform.Scale = NewScale;
	}

void CActor::SetActorLocation ( const FVector & NewLocation )
	{
	ActorTransform.Location = NewLocation;
	}

void CActor::SetActorLocation ( float X, float Y, float Z )
	{
	ActorTransform.Location = FVector ( X, Y, Z );
	}

void CActor::SetActorRotation ( const FQuat & RotationQuaternion )
	{
	ActorTransform.Rotation = RotationQuaternion;
	}

void CActor::SetActorRotation ( const FMat4 & RotationMatrix )
	{
	ActorTransform.Rotation = FQuat ( RotationMatrix );
	}

void CActor::SetActorRotation ( float Pitch, float Yaw, float Roll )
	{
		// Convert Euler angles from degrees to radians
	FVector eulerRad (
		CEMath::DegreesToRadians ( Pitch ),
		CEMath::DegreesToRadians ( Yaw ),
		CEMath::DegreesToRadians ( Roll )
	);
	ActorTransform.Rotation = FQuat::FromEulerAngles ( eulerRad.x, eulerRad.y, eulerRad.z );
	}

void CActor::SetActorScale ( const FVector & NewScale )
	{
	ActorTransform.Scale = NewScale;
	}

void CActor::SetActorScale ( float ScaleX, float ScaleY, float ScaleZ )
	{
	ActorTransform.Scale = FVector ( ScaleX, ScaleY, ScaleZ );
	}

void CActor::SetActorScale ( float UniformScale )
	{
	ActorTransform.Scale = FVector ( UniformScale, UniformScale, UniformScale );
	}

FTransform CActor::GetActorTransform () const
	{
	return ActorTransform;
	}

FVector CActor::GetActorLocation () const
	{
	return ActorTransform.Location;
	}

FVector CActor::GetActorRotationEuler () const
	{
	return ActorTransform.GetRotationEulerDegrees ();
	}

FQuat CActor::GetActorRotationQuat () const
	{
	return ActorTransform.Rotation;
	}

FVector CActor::GetActorScale () const
	{
	return ActorTransform.Scale;
	}

bool CActor::IsActive () const
	{
	return bIsActive;
	}

bool CActor::IsVisible () const
	{
	return bIsVisible;
	}

bool CActor::IsCanTick () const
	{
	return bIsCanTick;
	}

bool CActor::IsPendingDestroy () const
	{
	return bIsPendingDestroy;
	}

void CActor::MoveActor ( const FVector & DeltaLocation )
	{
	ActorTransform.Location += DeltaLocation;
	}

void CActor::MoveActor ( float DeltaX, float DeltaY, float DeltaZ )
	{
	ActorTransform.Location += FVector ( DeltaX, DeltaY, DeltaZ );
	}

void CActor::RotateActor ( float DeltaPitch, float DeltaYaw, float DeltaRoll )
	{
	RotateActor ( FVector ( DeltaPitch, DeltaYaw, DeltaRoll ) );
	}

void CActor::RotateActor ( const FVector & DeltaRotationEuler )
	{
		// Convert Euler delta angles from degrees to radians
	FVector deltaRad (
		CEMath::DegreesToRadians ( DeltaRotationEuler.x ),
		CEMath::DegreesToRadians ( DeltaRotationEuler.y ),
		CEMath::DegreesToRadians ( DeltaRotationEuler.z )
	);

	// Create quaternion from delta angles
	FQuat deltaQuat = FQuat::FromEulerAngles ( deltaRad.x, deltaRad.y, deltaRad.z );

	// Apply rotation: new rotation = delta * current
	ActorTransform.Rotation = deltaQuat * ActorTransform.Rotation;
	}

void CActor::RotateActor ( const FMat4 & DeltaRotationMatrix )
	{
	FQuat deltaRotationQuat ( DeltaRotationMatrix );
	ActorTransform.Rotation = deltaRotationQuat * ActorTransform.Rotation;
	}

void CActor::RotateActor ( const FQuat & DeltaRotationQuaternion )
	{
	ActorTransform.Rotation = DeltaRotationQuaternion * ActorTransform.Rotation;
	}

void CActor::ScaleActor ( const FVector & DeltaScale )
	{
	ActorTransform.Scale += DeltaScale;
	}

void CActor::ScaleActor ( float DeltaScaleX, float DeltaScaleY, float DeltaScaleZ )
	{
	ActorTransform.Scale += FVector ( DeltaScaleX, DeltaScaleY, DeltaScaleZ );
	}

void CActor::ScaleActor ( float UniformDeltaScale )
	{
	ActorTransform.Scale += FVector ( UniformDeltaScale, UniformDeltaScale, UniformDeltaScale );
	}

FVector CActor::GetActorForwardVector () const
	{
	return GetActorRotationQuat () * FVector::Forward ();
	}

FVector CActor::GetActorRightVector () const
	{
	return GetActorRotationQuat () * FVector::Right ();
	}

FVector CActor::GetActorUpVector () const
	{
	return GetActorRotationQuat () * FVector::Up ();
	}

