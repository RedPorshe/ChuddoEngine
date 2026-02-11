#pragma once
#include "Engine/GameFramework/World/Level.h"
#include "Actors/Pawn.h"

class CTestLevel : public CLevel
	{
	CHUDDO_DECLARE_CLASS ( CTestLevel, CLevel );
	public:
		CTestLevel ( CObject * Owner = nullptr, const std::string & inLevelName = "TestLevel" );
		virtual void Tick ( float DeltaTime ) override;
		virtual void BeginPlay () override;
		virtual void EndPlay () override;

		CPawn * Player = nullptr;
		CPawn * Enemy = nullptr;

		

	};

REGISTER_CLASS_FACTORY ( CTestLevel );
