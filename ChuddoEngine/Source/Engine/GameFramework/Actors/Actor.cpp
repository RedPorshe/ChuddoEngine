#include "Actors/Actor.h"
#include "World/Level.h"
#include "World/World.h"
#include "GameInstance.h"
#include "GameFramework/Components/BaseComponent.h"
#include "Components/SceneComponent.h"


CActor::CActor ( CObject * owner, const std::string & inName ) : CObject ( owner, inName )
	{

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

void CActor::SetRootComponent ( CSceneComponent * NewRoot )
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
		LOG_WARN ( "Component is already RootComponent" );
		return;
		}

	CSceneComponent * OldRoot = RootComponent;
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
		LOG_WARN ("Actor: ", GetName (), " already marked to destroy");
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
		LOG_WARN ( "Actor: ", GetName(), " already marked to destroy");
		return;
		}
	LOG_DEBUG ( "Actor:", GetName (), " is marked to destroy" );
	bIsPendingToDestroy = true;
	}

void CActor::SetActorName ( const std::string & newName )
	{
	this->Rename ( newName );
	}


