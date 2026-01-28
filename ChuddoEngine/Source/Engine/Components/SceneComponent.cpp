#include "Components/SceneComponent.h"
#include "Actors/Actor.h"

CSceneComponent::CSceneComponent ( const CObject * Owner, const std::string & inName ) : CBaseComponent ( Owner, inName )
	{
	if (GetOwner ())
		{
		const CActor * OwnerActor = dynamic_cast< const CActor * >( GetOwner () );
		if (OwnerActor)
			{
			m_OwnerActor = const_cast< CActor * >( OwnerActor );
			}
		else
			{
			const CSceneComponent * sceneComponentOwner = dynamic_cast< const CSceneComponent * >( GetOwner () );
			if (sceneComponentOwner)
				{
				m_OwnerComponent = const_cast< CSceneComponent * >( sceneComponentOwner );
				}
			}
		}
	std::cout << "Scene component '" << this->GetName () << "' created\n";
	}

CSceneComponent::~CSceneComponent ()
	{
	std::cout << "Destoying '" << this->GetName () << "'\n";
	}

void CSceneComponent::BeginPlay ()
	{
	
	CBaseComponent::BeginPlay ();
	if (this->HasOwnedObjects ())
		{
		for (int i {};i<OwnedObjects.size();i++)
			{
			 auto comp = dynamic_cast< const CSceneComponent * > ( OwnedObjects[i].get());
			if (comp)
				{
				const_cast< CSceneComponent * > ( comp )->BeginPlay ();
				}
			}
		}
	bIsInitialized = true;
	}

void CSceneComponent::Tick ( float DeltaTime )
	{
	CBaseComponent::Tick ( DeltaTime );
	//std::cout << typeid( this ).name () << "with name '" << this->GetName () << "'\n";
	if (this->HasOwnedObjects ())
		{
		for (int i {}; i < OwnedObjects.size (); i++)
			{
			auto comp = dynamic_cast< const CSceneComponent * > ( OwnedObjects[ i ].get () );
			if (comp)
				{
				const_cast< CSceneComponent * > ( comp )->Tick ( DeltaTime );
				}
			}
		}
	}
