#pragma once

#include "Actors/Character.h"

class MyCharact : public CCharacter
	{
	CHUDDO_DECLARE_CLASS ( MyCharact, CCharacter );
	public:
		MyCharact ( CObject * inOwner, const std::string & DisplayName );
		virtual ~MyCharact () = default;
		void BeginPlay () override;
		void Tick ( float DeltaTime ) override;
		void EndPlay () override;
		void SetupPlayerInputComponent ( CInputComponent * InputComponent ) override;
	};
REGISTER_CLASS_FACTORY ( MyCharact );