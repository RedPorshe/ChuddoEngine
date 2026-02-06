#include "GameFramework/Components/BaseComponent.h"
#include "GameFramework/Actors/Actor.h"

REGISTER_CLASS_FACTORY ( CBaseComponent );


CBaseComponent::CBaseComponent ( CObject * owner, const std::string & inName ) : CObject ( owner, inName )
	{
	LOG_DEBUG( "Constructor of CBaseComponent with name '" , GetName () , "'");
	auto AnewOwner = dynamic_cast< CActor * >( owner );
	if (AnewOwner)
		{
		ActorOwner = AnewOwner;
		LOG_DEBUG( GetName () , " has owner actor " , typeid( *ActorOwner ).name () , " with name '" , ActorOwner->GetName () , "'");
		}
	}

CBaseComponent::~CBaseComponent ()
	{
	LOG_DEBUG("Destructor of CBaseComponent with name '" , GetName () , "'");
	}

void CBaseComponent::InitComponent ()
	{
	if (bIsInitialized)
		{
		LOG_WARN( "Component '" , GetName () , "' already initialized");
		return;
		}
	bIsInitialized = true;
	}

void CBaseComponent::Tick ( float DeltaTime )
	{
	if (!bIsComponentTick) return;

	if (!bIsInitialized)
		{
		static int warnCount = 0;
		if (warnCount++ < 3) // Лимитируем предупреждения
			{
			LOG_WARN ( "Component '" , GetName ()
				, "' not initialized, skipping tick");
			}
		return;
		}

		// Тикаем собственные компоненты
	for (auto comp : OwnedComponents)
		{
		if (comp && comp->CanTick ())
			{
			comp->Tick ( DeltaTime );
			}
		}

		// Тикаем прикреплённых акторов
	for (auto actor : AttachedActors)
		{
		if (actor && actor->IsCanTickOnAttached () && actor->IsAttached ())
			{
			actor->Tick ( DeltaTime );
			}
		}	
	}

bool CBaseComponent::CanTick () const
	{
	return bIsComponentTick && bIsInitialized;
	}

void CBaseComponent::OnBeginPlay ()
	{
	if (bIsAutoInit)
		{
		InitComponent ();
		}
	//over funcs on begin play can place here...
	}

void CBaseComponent::SetPrimaryTick ( bool value )
	{
	bIsComponentTick = value;
	}

void CBaseComponent::SetAutoInitialize ( bool value )
	{
	bIsAutoInit = value;
	}

CActor * CBaseComponent::GetOwnerActor ()
	{
	if (ActorOwner)
		{ return ActorOwner; }
	return nullptr;
	}

bool CBaseComponent::IsHaveOwnerActor ()
	{
	return GetOwnerActor () != nullptr;
	}

void CBaseComponent::AttachComponentToComponent ( CBaseComponent * CompToAttach )
	{
	if (!CompToAttach)
		{
		LOG_ERROR ( "Component to attach is nullptr");
		return;
		}

	if (CompToAttach == this)
		{
		LOG_ERROR ( "Cannot attach component to itself");
		return;
		}

		// Проверяем, не прикреплён ли уже
	auto it = std::find ( OwnedComponents.begin (), OwnedComponents.end (), CompToAttach );
	if (it != OwnedComponents.end ())
		{
		LOG_WARN ( "Component '" , CompToAttach->GetName (), "' already attached to this component");
		return;
		}

		// Проверяем, не создаст ли это циклическую ссылку
	if (WouldCreateCircularReference ( CompToAttach ))
		{
		LOG_ERROR( "Would create circular reference");
		return;
		}

		// Передаём владение
	if (CompToAttach->GetOwner ()->TransferOwnership ( CompToAttach, this ))
		{
		OwnedComponents.push_back ( CompToAttach );
		LOG_DEBUG(  "Success attach component '" , CompToAttach->GetName ()
			, "' to component '", GetName () , "'");
		}
	else
		{
		LOG_ERROR ("Failed to transfer ownership");
		}
	}

void CBaseComponent::AttachActorToComponent ( CActor * ActorToAttach )
	{
	if (!ActorToAttach)
		{
		LOG_ERROR( "Actor to attach is nullptr");
		return;
		}

		// Проверяем, не прикреплён ли уже
	auto it = std::find ( AttachedActors.begin (), AttachedActors.end (), ActorToAttach );
	if (it != AttachedActors.end ())
		{
		LOG_WARN( "Actor '" , ActorToAttach->GetName ()
			, "' already attached to this component");
		return;
		}

		// ЛОГИЧЕСКОЕ прикрепление (не владение!)
	AttachedActors.push_back ( ActorToAttach );
	

	// Если это SceneComponent, можно обновить трансформы
	if (CSceneComponent * SceneComp = dynamic_cast< CSceneComponent * >( this ))
		{
		if (CSceneComponent * ActorRoot = ActorToAttach->GetRootComponent ())
			{
				// Прикрепляем корневой компонент актора к этому компоненту
			ActorRoot->AttachComponentToComponent ( SceneComp );
			}
		}
	ActorToAttach->SetIsAttached ( true );
	LOG_DEBUG( "Success attach actor '" , ActorToAttach->GetName ()
		, "' to component '", this->GetName () , "'");
	}

bool CBaseComponent::WouldCreateCircularReference ( CBaseComponent * CompToAttach ) 
	{
		// Проверяем, не является ли CompToAttach нашим предком
	CObject * current = this;
	while (current)
		{
		if (current == CompToAttach)
			return true;

		CBaseComponent * comp = dynamic_cast< CBaseComponent * >( current );
		current = comp ? comp->GetOwner () : nullptr;
		}
	return false;
	}

void CBaseComponent::DetachFromParent ()
	{
	}


