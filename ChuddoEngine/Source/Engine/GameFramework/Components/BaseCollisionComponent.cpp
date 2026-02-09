#include "Components/BaseCollisionComponent.h"
#include "Actors/Actor.h"
#include "Core/CollisionSystem.h"

CBaseCollisionComponent::CBaseCollisionComponent ( CObject * inOwner,
												   const std::string & inDisplayName )
	: Super ( inOwner, inDisplayName )
	, m_CollisionChannel ( FCollisionChannel::Static () )  
	{
	LOG_DEBUG ( "Created collision component '", inDisplayName,"' with '", m_CollisionChannel.GetName (),"' channel" );	
	CCollisionSystem::Get ().RegisterCollisionComponent ( this );
	}

CBaseCollisionComponent::~CBaseCollisionComponent ()
	{
	CCollisionSystem::Get ().UnregisterCollisionComponent ( this );
	OverlappingComponents.clear ();
	}


FVector CBaseCollisionComponent::GetWorldLocation () 
	{
		// Метод 1: Через владеющий актор
	CActor * ownerActor = GetOwnerActor ();
	if (ownerActor)
		{
		FVector pos = ownerActor->GetActorLocation ();
		LOG_DEBUG ( "[COLLISION LOC] ", GetName (), " -> Actor '",
					ownerActor->GetName (), "' -> Location: (",
					pos.x, ", ", pos.y, ", ", pos.z, ")" );
		return pos;
		}

		// Метод 2: Через иерархию
	CObject * owner = GetOwner ();
	int depth = 0;
	while (owner && depth < 10) // Ограничиваем глубину чтобы не зациклиться
		{
		if (CActor * actor = dynamic_cast< CActor * > ( owner ))
			{
			FVector pos = actor->GetActorLocation ();
			LOG_DEBUG ( "[COLLISION LOC] ", GetName (), " -> Owner[", depth, "] '",
						owner->GetName (), "' (Actor) -> Location: (",
						pos.x, ", ", pos.y, ", ", pos.z, ")" );
			return pos;
			}

			// Проверяем TransformComponent
		if (CTransformComponent * transform = dynamic_cast< CTransformComponent * >( owner ))
			{
			FVector pos = transform->GetLocation ();
			LOG_DEBUG ( "[COLLISION LOC] ", GetName (), " -> Owner[", depth, "] '",
						owner->GetName (), "' (Transform) -> Location: (",
						pos.x, ", ", pos.y, ", ", pos.z, ")" );
			return pos;
			}

		owner = owner->GetOwner ();
		depth++;
		}

	LOG_WARN ( "[COLLISION] ", GetName (), " GetWorldLocation() failed - no valid owner found" );
	return FVector::Zero ();
	}

void CBaseCollisionComponent::InitComponent ()
	{
	Super::InitComponent ();
	}

void CBaseCollisionComponent::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );
	}

void CBaseCollisionComponent::OnBeginPlay ()
	{
	Super::OnBeginPlay ();

	if (bIsCollisionEnabled)
		{
		LOG_DEBUG ( "Collision component '", GetName (), "' enabled with channel: ", m_CollisionChannel.GetName () );
		}
	}

bool CBaseCollisionComponent::CheckCollision ( CBaseCollisionComponent * other ) const
	{
	LOG_DEBUG ( "Collision this CBaseCollisionComponent always ignore and return false" );
	return false;
	}

void CBaseCollisionComponent::SetShapeType ( const ECollisionShape & inShape )
	{
	m_CollisionShape = inShape;
	}

void CBaseCollisionComponent::SetCollisionChannel ( const FCollisionChannel & channel )
	{
	m_CollisionChannel = channel;
	LOG_DEBUG ( "Set collision channel to: ", channel.GetName () );
	}

void CBaseCollisionComponent::SetCollisionChannel ( const std::string & channelName )
	{
	m_CollisionChannel = FCollisionChannel::Create ( channelName );
	LOG_DEBUG ( "Set collision channel to: ", channelName );
	}

	// ========== Быстрые настройки каналов ==========

void CBaseCollisionComponent::SetChannelAsStatic ()
	{
	m_CollisionChannel.SetupAsStatic ();
	LOG_DEBUG ( "Channel set as Static" );
	}

void CBaseCollisionComponent::SetChannelAsDynamic ()
	{
	m_CollisionChannel.SetupAsDynamic ();
	LOG_DEBUG ( "Channel set as Dynamic" );
	}

void CBaseCollisionComponent::SetChannelAsCharacter ()
	{
	m_CollisionChannel.SetupAsCharacter ();
	LOG_DEBUG ( "Channel set as Character" );
	}

void CBaseCollisionComponent::SetChannelAsTrigger ()
	{
	m_CollisionChannel.SetupAsTrigger ();
	LOG_DEBUG ( "Channel set as Trigger" );
	}

void CBaseCollisionComponent::SetChannelAsPawn ()
	{
	m_CollisionChannel.SetupAsPawn ();
	LOG_DEBUG ( "Channel set as Pawn" );
	}

void CBaseCollisionComponent::SetChannelAsVehicle ()
	{
	m_CollisionChannel.SetupAsVehicle ();
	LOG_DEBUG ( "Channel set as Vehicle" );
	}

void CBaseCollisionComponent::SetChannelAsInteractable ()
	{
	m_CollisionChannel.SetupAsInteractable ();
	LOG_DEBUG ( "Channel set as Interactable" );
	}

void CBaseCollisionComponent::SetChannelAsCustom ( const std::string & channelName,
												   ECollisionResponse defaultResponse )
	{
	m_CollisionChannel = FCollisionChannel::Create ( channelName, defaultResponse );
	LOG_DEBUG ( "Channel set as custom: ", channelName );
	}

	// ========== Настройка ответов ==========

void CBaseCollisionComponent::SetResponseToChannel ( const std::string & otherChannelName,
													 ECollisionResponse response )
	{
	m_CollisionChannel.SetResponseTo ( otherChannelName, response );
	LOG_DEBUG ( "Set response to channel '", otherChannelName,"' : ", Collision::ResponseToString ( response ) );
	}

void CBaseCollisionComponent::SetResponseToChannel ( ECollisionChannel otherChannel,
													 ECollisionResponse response )
	{
	m_CollisionChannel.SetResponseTo ( otherChannel, response );
	LOG_DEBUG ( "Set response to channel '", Collision::ChannelToString ( otherChannel ), "' : ", Collision::ResponseToString ( response ) );
	}

	// ========== Проверка взаимодействия ==========

bool CBaseCollisionComponent::CanCollideWith ( const CBaseCollisionComponent * other ) const
	{
	if (!other || !bIsCollisionEnabled || !other->bIsCollisionEnabled)
		return false;

	return m_CollisionChannel.CanCollideWith ( other->m_CollisionChannel.GetChannel () );
	}

bool CBaseCollisionComponent::CanCollideWith ( const std::string & otherChannelName ) const
	{
	if (!bIsCollisionEnabled)
		return false;

	return m_CollisionChannel.CanCollideWith ( otherChannelName );
	}

bool CBaseCollisionComponent::ShouldBlockWith ( const CBaseCollisionComponent * other ) const
	{
	if (!other || !bIsCollisionEnabled || !other->bIsCollisionEnabled)
		return false;

	return m_CollisionChannel.ShouldBlock ( other->m_CollisionChannel.GetChannel () );
	}

bool CBaseCollisionComponent::ShouldOverlapWith ( const CBaseCollisionComponent * other ) const
	{
	if (!other || !bIsCollisionEnabled || !other->bIsCollisionEnabled)
		return false;

	return m_CollisionChannel.ShouldOverlap ( other->m_CollisionChannel.GetChannel () );
	}

void CBaseCollisionComponent::OnBeginOverlap ( CBaseCollisionComponent * other )
	{
	if (!other || !GetOwnerActor ())
		return;

	
	if (OverlappingComponents.find ( other ) != OverlappingComponents.end ())
		return; 

	OverlappingComponents.insert ( other );

	if (other->OverlappingComponents.find ( this ) == other->OverlappingComponents.end ())
		{
		other->OverlappingComponents.insert ( this );
		other->OnBeginOverlap ( this ); 
		}

	GetOwnerActor ()->OnComponentBeginOverlap ( other );
	}

void CBaseCollisionComponent::OnEndOverlap ( CBaseCollisionComponent * other )
	{
	if (!other || !GetOwnerActor ())
		return;

	if (OverlappingComponents.find ( other ) == OverlappingComponents.end ())
		return; 

	
	OverlappingComponents.erase ( other );

	if (other->OverlappingComponents.find ( this ) != other->OverlappingComponents.end ())
		{
		other->OverlappingComponents.erase ( this );
		other->OnEndOverlap ( this );
		}

	GetOwnerActor ()->OnComponentEndOverlap ( other );
	}
