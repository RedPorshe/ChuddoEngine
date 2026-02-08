#include "Actors/Actor.h"
#include "World/Level.h"
#include "World/World.h"
#include "GameInstance.h"
#include "GameFramework/Components/BaseComponent.h"
#include "Components/SceneComponent.h"


CActor::CActor ( CObject * owner, const std::string & inName ) : CObject ( owner, inName )
	{
	RootComponent = AddDefaultSubObject<CTransformComponent> ( inName + "_Transform" );	
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

	}

void CActor::Tick ( float deltaTime )
	{
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

FVector  CActor::GetActorLocation ()
	{
	FVector result {};
	if(GetRootComponent()!= nullptr) result = RootComponent->GetLocation ();
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

void CActor::SetActorLocation ( const FVector & InLocation )
	{	
	if (RootComponent)
		{
		RootComponent->SetLocation ( InLocation );
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
		}
	}

void CActor::SetActorRotation ( float inX, float inY, float inZ )
	{
	SetActorRotation ( FVector ( inX, inY, inZ ) );
	}





void CActor::SetActorName ( const std::string & newName )
	{
	this->Rename ( newName );
	}


