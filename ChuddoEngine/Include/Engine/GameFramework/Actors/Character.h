#pragma once

#include "Pawn.h"

class CCharacter : public CPawn
	{
	CHUDDO_DECLARE_CLASS( CCharacter, CPawn );
	public:
		CCharacter ( CObject * inOwner = nullptr, const std::string & inDisplayName = "Character" );
		virtual ~CCharacter ();
		// Character-specific properties and methods can be added here
		virtual void Tick ( float DeltaTime ) override;
		virtual void BeginPlay () override;
	};
REGISTER_CLASS_FACTORY( CCharacter );