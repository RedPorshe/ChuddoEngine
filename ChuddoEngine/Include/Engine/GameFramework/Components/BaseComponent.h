#pragma once

#include "Core/Object.h"

class CActor;

class CBaseComponent : public CObject
	{
	CHUDDO_DECLARE_ABSTRACT_CLASS ( CBaseComponent, CObject )
	public:
		CBaseComponent ( CObject * owner = nullptr, const std::string & inName = "BaseComponent" );
		virtual ~CBaseComponent ();
		virtual void InitComponent ();
		virtual void Tick ( float DeltaTime );
		virtual void OnBeginPlay ();
		void SetPrimaryTick ( bool value = true );
		void SetAutoInitialize ( bool value );
		CActor * GetOwnerActor ();
		bool IsHaveOwnerActor  ( );
		void AttachComponentToComponent ( CBaseComponent * CompToAttach );
		void AttachActorToComponent ( CActor * ActorToAttach );
		void DetachFromParent ();
		bool WouldCreateCircularReference ( CBaseComponent * CompToAttach ) ;
		bool CanTick () const;
	protected:
		std::vector<CBaseComponent *> OwnedComponents;
		std::vector<CActor *> AttachedActors;
		bool bIsComponentTick { false };
		bool bIsInitialized { false };
		bool bIsAutoInit { false };
		CActor * ActorOwner = nullptr;

	};

REGISTER_CLASS_FACTORY ( CBaseComponent );