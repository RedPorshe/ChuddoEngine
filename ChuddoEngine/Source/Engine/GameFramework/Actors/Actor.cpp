#include "Actors/Actor.h"
#include "World/Level.h"
#include "GameInstance.h"
#include "GameFramework/Components/BaseComponent.h"
#include "Components/SceneComponent.h"


CActor::CActor ( CObject * owner, const std::string & inName ) : CObject ( owner, inName )
	{
	LOG_DEBUG("[ACTOR] Actor created : " , inName );
	}

CActor::~CActor ()
	{
	LOG_DEBUG( "[ACTOR] Actor destroyed: " , GetName ());


	for (auto comp : ActorComponents)
		{
		if (comp && comp->GetOwner () == this)
			{
			
			}
		}
	ActorComponents.clear ();
	}

void CActor::BeginPlay ()
	{
	LOG_DEBUG( "[ACTOR] BeginPlay: " , GetName ());
	for (auto comp : ActorComponents)
		{
		comp->OnBeginPlay ();
		}
	}

void CActor::Tick ( float deltaTime )
	{
	std::string prefix = IsCanTickOnAttached () ? "[Attached ACTOR] " : "[ACTOR] ";
	LOG_DEBUG( prefix , "Tick: " , GetName () , " (delta: " , deltaTime , ")");

	// Тикаем только компоненты, которые ВСЁ ЕЩЁ принадлежат этому актору
	for (auto comp : ActorComponents)
		{
		if (comp && comp->GetOwner () == this)  // Ключевая проверка!
			{
			comp->Tick ( deltaTime );
			}
		}
	}

void CActor::EndPlay ()
	{
	LOG_DEBUG( "[ACTOR] EndPlay: " , GetName () );
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
		LOG_WARN( "Cannot set nullptr as RootComponent");
		return;
		}

	if (NewRoot->GetOwner () != this)
		{
		LOG_WARN( "RootComponent must belong to this actor");
		return;
		}

	if (RootComponent == NewRoot)
		{
		LOG_WARN( "Component is already RootComponent");
		return;
		}

	CSceneComponent * OldRoot = RootComponent;
	RootComponent = NewRoot;

	if (OldRoot)
		{
			// Прикрепляем старый корень к новому
		NewRoot->AttachComponentToComponent ( OldRoot );
		LOG_DEBUG( "Changed RootComponent from '" , OldRoot->GetName ()
			, "' to '" , NewRoot->GetName () , "'");
		}
	else
		{
		LOG_DEBUG( "Set '" , NewRoot->GetName ()
			, "' as RootComponent for actor '" , GetName () , "'");
		}
	}

void CActor::SetActorName ( const std::string & newName )
	{
	this->Rename ( newName );
	}
