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
		CActor * ParentActor = nullptr;
		CActor * ChildActor = nullptr;

	};

REGISTER_CLASS_FACTORY ( CTestLevel );

#include "GameFramework/GameMode.h"

class CTestGameMode : public CGameMode
	{
	CHUDDO_DECLARE_CLASS ( CTestGameMode, CGameMode )

	public:
		CTestGameMode ( CObject * inOwner = nullptr, const std::string & inName = "TestGameMode" );
		virtual ~CTestGameMode ();

		virtual void InitGame () override;
		virtual void StartPlay () override;
		virtual void Tick ( float DeltaTime ) override;

		// Переопределяем чтобы убедиться что GameMode работает
		virtual bool ShouldSpawnPlayerAutomatically () const override { return true; }
	};

REGISTER_CLASS_FACTORY ( CTestGameMode );