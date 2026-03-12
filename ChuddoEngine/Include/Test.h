#pragma once

#include "Actors/Pawns/Character.h"

class CFirstPersonCharacter : public CCharacter
	{
	CHUDDO_DECLARE_CLASS ( CFirstPersonCharacter, CCharacter )
	public:
		CFirstPersonCharacter ( CObject * inOWner, const std::string & inName );
		virtual ~CFirstPersonCharacter () = default;

		void BeginPlay () override;
		void Tick ( float DeltaTime ) override;
		void EndPlay () override;

		void OnComponentBeginOverlap ( CBaseCollisionComponent * other ) override;
		void OnComponentEndOverlap ( CBaseCollisionComponent * other ) override;
		void OnComponentHit ( CBaseCollisionComponent * other )override;
	protected:
		void SetupPlayerInputComponent ( CInputComponent * InputComponent ) override;
		void DebugInfo ( float dt ) override;
		void MoveForward ( float val );
		void MoveRight ( float val );
		void Jump ();

		CCameraComponent * Camera = nullptr;
	};
REGISTER_CLASS_FACTORY ( CFirstPersonCharacter );


