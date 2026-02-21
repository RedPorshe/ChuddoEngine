#include "Actors/Actor.h"
#include "World/Level.h"
#include "World/World.h"
#include "GameInstance.h"

#include "Components/Collisions/TerrainComponent.h"
#include "GameFramework/Components/BaseComponent.h"
#include "Components/SceneComponent.h"
#include "Components/Collisions/BaseCollisionComponent.h"
#include "Components/GravityComponent.h"


CActor::CActor ( CObject * owner, const std::string & inName ) : CObject ( owner, inName )
	{
	RootComponent = AddDefaultSubObject<CTransformComponent> ( inName + "_Transform" );
	m_Gravity = AddDefaultSubObject<CGravityComponent> ( GetName () + "_Gravity" );
	if (RootComponent) RootComponent->SetCollisionEnabled ( bIsCollisionEnabled );
	}

CActor::~CActor ()
	{
	ActorComponents.clear ();
	}

void CActor::BeginPlay ()
	{
	LOG_DEBUG ( "[ACTOR] BeginPlay: ", GetName () );
	for (auto comp : ActorComponents)
		{
		comp->OnBeginPlay ();
		}
	if (RootComponent) RootComponent->SetCollisionEnabled ( bIsCollisionEnabled );
	}

void CActor::Tick ( float deltaTime )
	{
	
	if (this == nullptr) return;
	if (IsPendingToDestroy ()) return;
		// Обработка интерполяции позиции
	if (bIsLerpingLocation && RootComponent)
		{
		LocationLerpAlpha += deltaTime * LerpSpeed;

		if (LocationLerpAlpha >= 1.0f)
			{
				// Интерполяция завершена
			LocationLerpAlpha = 1.0f;
			bIsLerpingLocation = false;
			bIsMovin = false;
			RootComponent->SetLocation ( TargetLocation );
			
			}
		else
			{
				// Линейная интерполяция
			FVector currentLocation = LerpStartLocation + ( TargetLocation - LerpStartLocation ) * LocationLerpAlpha;
			RootComponent->SetLocation ( currentLocation );
			}
		}

		// Обработка интерполяции вращения (Slerp - сферическая интерполяция)
	if (bIsLerpingRotation && RootComponent)
		{
		RotationLerpAlpha += deltaTime * LerpSpeed;

		if (RotationLerpAlpha >= 1.0f)
			{
				// Интерполяция завершена
			RotationLerpAlpha = 1.0f;
			bIsLerpingRotation = false;
			RootComponent->SetRotation ( TargetRotation );			
			}
		else
			{
				// Сферическая линейная интерполяция (Slerp)
			FQuat currentRotation = FQuat::Slerp ( LerpStartRotation, TargetRotation, RotationLerpAlpha );
			currentRotation.Normalize ();
			RootComponent->SetRotation ( currentRotation );
			}
		}

		// Вызов Tick для всех компонентов
	for (auto comp : ActorComponents)
		{
		if (comp && comp->GetOwner () == this)
			{
			comp->Tick ( deltaTime );
			}
		}
	
	}

void CActor::EndPlay ()
	{
	LOG_DEBUG ( "[ACTOR] EndPlay: ", GetName () );
	}

CLevel * CActor::GetLevel () const
	{
	return dynamic_cast< CLevel * >( GetOwner () );
	}

CWorld * CActor::GetWorld () const
	{
	return CGameInstance::Get ().GetWorld ();
	}


void CActor::SetRootComponent ( CTransformComponent * NewRoot )
	{
	if (!NewRoot)
		{
		LOG_WARN ( "Cannot set nullptr as RootComponent" );
		return;
		}

	if (NewRoot->GetOwner () != this)
		{
		LOG_WARN ( "RootComponent must belong to this actor" );
		return;
		}

	if (RootComponent == NewRoot)
		{
		return;
		}

	CTransformComponent * OldRoot = RootComponent;
	RootComponent = NewRoot;

	if (OldRoot)
		{
			// Прикрепляем старый корень к новому
		NewRoot->AttachComponentToComponent ( OldRoot );
		LOG_DEBUG ( "Changed RootComponent from '", OldRoot->GetName ()
					, "' to '", NewRoot->GetName (), "'" );
		}
	else
		{
		LOG_DEBUG ( "Set '", NewRoot->GetName ()
					, "' as RootComponent for actor '", GetName (), "'" );
		}
	}

void CActor::Destroy ()
	{
	if (bIsPendingToDestroy)
		{
		LOG_WARN ( "Actor: ", GetName (), " already marked to destroy" );
		return;
		}
	auto level = GetLevel ();
	if (level)
		{
		level->DestroyActor ( GetName () );
		}
	}

void CActor::SetPendingToDestroy ()
	{
	if (bIsPendingToDestroy)
		{
		LOG_WARN ( "Actor: ", GetName (), " already marked to destroy" );
		return;
		}
	LOG_DEBUG ( "Actor:", GetName (), " is marked to destroy" );
	bIsPendingToDestroy = true;
	}

CBaseComponent * CActor::AddDefaultSubObject ( const std::string & className, const std::string & desiredDisplayName )
	{
	auto CompToAdd = CObject::AddSubObjectByClass ( className, desiredDisplayName );
	if (CBaseComponent * compToReturn = dynamic_cast< CBaseComponent * >( CompToAdd ))
		{
		return compToReturn;
		}
	return nullptr;
	}

FVector CActor::GetActorLocation ()
	{
	FVector result {};
	
	if (GetRootComponent () != nullptr)
		{
		return  RootComponent->GetLocation ();
		}
	
	return result;
	}

FVector CActor::GetActorRotation ()
	{
	FVector RotVec = GetActorRotationQuat ().GetEulerAngles ();
	return FVector (
		CEMath::RadiansToDegrees ( RotVec.x ),
		CEMath::RadiansToDegrees ( RotVec.y ),
		CEMath::RadiansToDegrees ( RotVec.z )
	);
	}

FVector CActor::GetActorScale ()
	{
	FVector result {};
	if (GetRootComponent () != nullptr) result = RootComponent->GetScale ();
	return result;
	}

FQuat CActor::GetActorRotationQuat ()
	{
	FQuat result {};
	if (GetRootComponent () != nullptr) result = RootComponent->GetRotationQuat ();
	return result;
	}

	// ============================================================================
	// Direction vectors getters
	// ============================================================================

FVector CActor::GetActorForwardVector ()
	{
	if (!RootComponent) return FVector::Forward ();
	FQuat rotation = RootComponent->GetRotationQuat ();
	rotation.Normalize ();
	// Вперед по оси Z (0, 0, 1) в локальном пространстве
	return rotation * FVector::Forward ();
	}

FVector CActor::GetActorRightVector ()
	{
	if (!RootComponent) return FVector::Right ();
	FQuat rotation = RootComponent->GetRotationQuat ();
	rotation.Normalize ();
	// Вправо по оси X (1, 0, 0) в локальном пространстве
	return rotation * FVector::Right ();
	}

FVector CActor::GetActorUpVector ()
	{
	if (!RootComponent) return FVector::Up ();
	FQuat rotation = RootComponent->GetRotationQuat ();
	rotation.Normalize ();
	// Вверх по оси Y (0, 1, 0) в локальном пространстве
	return rotation * FVector::Up ();
	}

	// ============================================================================
	// Transform setters
	// ============================================================================

void CActor::SetActorLocation ( const FVector & InLocation )
	{
	if (RootComponent)
		{
		RootComponent->SetLocation ( InLocation );
		// Сбрасываем интерполяцию позиции
		bIsLerpingLocation = false;
		LocationLerpAlpha = 0.0f;
		RootComponent->MarkTransformDirty ();
		}
	}

void CActor::SetActorLocation ( float inX, float inY, float inZ )
	{
	SetActorLocation ( FVector ( inX, inY, inZ ) );
	}

void CActor::SetActorScale ( const FVector & InScale )
	{
	if (RootComponent)
		{
		RootComponent->SetScale ( InScale );
		}
	}

void CActor::SetActorScale ( float inX, float inY, float inZ )
	{
	SetActorScale ( FVector ( inX, inY, inZ ) );
	}

void CActor::SetActorScale ( float InScale )
	{
	SetActorScale ( InScale, InScale, InScale );
	}

void CActor::SetActorRotation ( const FVector & inRotation )
	{
	FQuat rotationQuat = FQuat::FromEulerAngles (
		CEMath::DegreesToRadians ( inRotation.x ),
		CEMath::DegreesToRadians ( inRotation.y ),
		CEMath::DegreesToRadians ( inRotation.z )
	);
	SetActorRotation ( rotationQuat );
	}

void CActor::SetActorRotation ( const FQuat & inRotation )
	{
	if (RootComponent)
		{
		RootComponent->SetRotation ( inRotation );
		// Сбрасываем интерполяцию вращения
		bIsLerpingRotation = false;
		RotationLerpAlpha = 0.0f;
		}
	}

void CActor::SetActorRotation ( float inX, float inY, float inZ )
	{
	SetActorRotation ( FVector ( inX, inY, inZ ) );
	}

	// ============================================================================
	// Teleport functions (immediate movement)
	// ============================================================================

void CActor::TeleportTo ( const FVector & NewLocation )
	{
	LOG_DEBUG ( "[ACTOR] TeleportTo: ", GetName (), " to (",
				NewLocation.x, ", ", NewLocation.y, ", ", NewLocation.z, ")" );
	SetActorLocation ( NewLocation );
	}

void CActor::TeleportTo ( float NewX, float NewY, float NewZ )
	{
	TeleportTo ( FVector ( NewX, NewY, NewZ ) );
	}

void CActor::SetActorRotationImmediately ( const FQuat & NewRotation )
	{
	LOG_DEBUG ( "[ACTOR] SetActorRotationImmediately: ", GetName () );
	SetActorRotation ( NewRotation );
	}

void CActor::SetActorRotationImmediately ( const FVector & NewRotation )
	{
	LOG_DEBUG ( "[ACTOR] SetActorRotationImmediately: ", GetName (),
				" to (", NewRotation.x, ", ", NewRotation.y, ", ", NewRotation.z, ")" );
	SetActorRotation ( NewRotation );
	}

void CActor::SetActorRotationImmediately ( float inX, float inY, float inZ )
	{
	SetActorRotationImmediately ( FVector ( inX, inY, inZ ) );
	}



void CActor::MoveActor ( const FVector & Delta, bool Interpolate )
	{
	if (!RootComponent)
		return;

	if (!Interpolate)
		{	

		FVector currentLocation = RootComponent->GetLocation ();
		
		FVector NewLocation = currentLocation + Delta;
		
		RootComponent->SetLocation ( NewLocation );
		
		return;
		}

		
	FVector currentLocation = RootComponent->GetLocation ();
	TargetLocation = currentLocation + Delta;
	LerpStartLocation = currentLocation;
	LocationLerpAlpha = 0.0f;
	bIsLerpingLocation = true;
	bIsMovin = true;

	}

void CActor::RotateActor ( const FVector & DeltaRotation, bool Interpolate )
	{
	if (!RootComponent)
		return;

	FQuat deltaQuat = FQuat::FromEulerAngles (
		CEMath::DegreesToRadians ( DeltaRotation.x ),
		CEMath::DegreesToRadians ( DeltaRotation.y ),
		CEMath::DegreesToRadians ( DeltaRotation.z )
	);

	RotateActor ( deltaQuat, Interpolate );
	}

void CActor::RotateActor ( const FQuat & DeltaRotation, bool Interpolate )
	{
	if (!RootComponent)
		return;

	

	  
	FQuat currentQuat = RootComponent->GetRotationQuat ();

	
	FQuat targetQuat = currentQuat * DeltaRotation;
	targetQuat.Normalize ();

	if (!Interpolate)
		{
			
		RootComponent->SetRotation ( targetQuat );
		
		return;
		}

	TargetRotation = targetQuat;
	LerpStartRotation = currentQuat;
	RotationLerpAlpha = 0.0f;
	bIsLerpingRotation = true;

	
	}



void CActor::AddActorWorldOffset ( const FVector & DeltaLocation, bool Interpolate )
	{
		
	MoveActor ( DeltaLocation, Interpolate );
	}

void CActor::AddActorLocalOffset ( const FVector & DeltaLocation, bool Interpolate )
	{
	if (!RootComponent)
		return;

	FQuat rotation = RootComponent->GetRotationQuat ();
	rotation.Normalize ();

		FVector worldDelta = rotation * DeltaLocation;

	
	AddActorWorldOffset ( worldDelta, Interpolate );
	}

void CActor::AddActorWorldRotation ( const FQuat & DeltaRotation, bool Interpolate )
	{
	if (!RootComponent)
		return;

	
	FQuat currentQuat = RootComponent->GetRotationQuat ();

	
	FQuat newRotation = currentQuat * DeltaRotation;
	newRotation.Normalize ();

	if (!Interpolate)
		{
		RootComponent->SetRotation ( newRotation );
		LOG_DEBUG ( "[ACTOR] Instant world rotation" );
		return;
		}

		// Интерполированное вращение
	TargetRotation = newRotation;
	LerpStartRotation = currentQuat;
	RotationLerpAlpha = 0.0f;
	bIsLerpingRotation = true;

	}

void CActor::AddActorLocalRotation ( const FQuat & DeltaRotation, bool Interpolate )
	{
	if (!RootComponent)
		return;

	// Получаем текущее вращение
	FQuat currentQuat = RootComponent->GetRotationQuat ();

	// ЛОКАЛЬНОЕ вращение: умножаем СЛЕВА (q' = Δq * q)
	FQuat newRotation = DeltaRotation * currentQuat;
	newRotation.Normalize ();

	if (!Interpolate)
		{
		RootComponent->SetRotation ( newRotation );
		
		return;
		}

		
	TargetRotation = newRotation;
	LerpStartRotation = currentQuat;
	RotationLerpAlpha = 0.0f;
	bIsLerpingRotation = true;

	
	}

	// ============================================================================
	// Helper movement methods
	// ============================================================================

void CActor::MoveActorInDirection ( const FVector & Direction, float Distance, bool Interpolate )
	{
	if (!RootComponent || Direction.IsZero ())
		return;

	

	  // Нормализуем направление и умножаем на расстояние
	FVector normalizedDir = Direction.Normalized ();
	FVector delta = normalizedDir * Distance;

	MoveActor ( delta, Interpolate );
	}

void CActor::RotateAroundAxis ( const FVector & Axis, float AngleDegrees, bool Interpolate )
	{
	if (!RootComponent || Axis.IsZero ())
		return;

	FQuat rotationQuat ( Axis.Normalized (), CEMath::DegreesToRadians ( AngleDegrees ) );

	AddActorLocalRotation ( rotationQuat, Interpolate );
	}

	// ============================================================================
	// Collision methods
	// ============================================================================

void CActor::SetCollisionEnabled ( bool value )
	{
	bIsCollisionEnabled = value;
	if (RootComponent)
		{
		RootComponent->SetCollisionEnabled ( bIsCollisionEnabled );
		}
	}


FVector CActor::GetActorLocation () const
	{
	FVector result {};
	if (GetRootComponent () != nullptr)
		{
			// Здесь нужно вызвать const версию GetLocation() у CTransformComponent
		result = RootComponent->GetLocation ();
		}
	return result;
	}

FVector CActor::GetActorRotation () const
	{
	FVector RotVec = GetActorRotationQuat ().GetEulerAngles ();
	return FVector (
		CEMath::RadiansToDegrees ( RotVec.x ),
		CEMath::RadiansToDegrees ( RotVec.y ),
		CEMath::RadiansToDegrees ( RotVec.z )
	);
	}

FVector CActor::GetActorScale () const
	{
	FVector result {};
	if (GetRootComponent () != nullptr)
		{
			// Здесь нужно вызвать const версию GetScale() у CTransformComponent
		result = RootComponent->GetScale ();
		}
	return result;
	}

FQuat CActor::GetActorRotationQuat () const
	{
	FQuat result {};
	if (GetRootComponent () != nullptr)
		{
			// Здесь нужно вызвать const версию GetRotationQuat() у CTransformComponent
		result = RootComponent->GetRotationQuat ();
		}
	return result;
	}

FVector CActor::GetActorForwardVector () const
	{
	if (!RootComponent) return FVector::Forward ();
	FQuat rotation = GetActorRotationQuat ();
	rotation.Normalize ();
	return rotation * FVector::Forward ();
	}

FVector CActor::GetActorRightVector () const
	{
	if (!RootComponent) return FVector::Right ();
	FQuat rotation = GetActorRotationQuat ();
	rotation.Normalize ();
	return rotation * FVector::Right ();
	}

FVector CActor::GetActorUpVector () const
	{
	if (!RootComponent) return FVector::Up ();
	FQuat rotation = GetActorRotationQuat ();
	rotation.Normalize ();
	return rotation * FVector::Up ();
	}


void CActor::OnComponentBeginOverlap ( CBaseCollisionComponent * other )
	{
	if (other == nullptr) return;

	}

void CActor::OnComponentEndOverlap ( CBaseCollisionComponent * other )
	{
	if (other == nullptr) return;
	LOG_ERROR ( "OnComponentEndOverlap with : ", other->GetOwnerActor ()->GetName (), " for ", GetName () );
	
	}

void CActor::OnComponentHit ( CBaseCollisionComponent * other )
	{
	if (other->GetShapeType () == ECollisionShape::TERRAIN)
		{
		m_Gravity->SetVerticalVelocity ( 0.f );
		}
	}

void CActor::SetActorName ( const std::string & newName )
	{
	this->Rename ( newName );
	}