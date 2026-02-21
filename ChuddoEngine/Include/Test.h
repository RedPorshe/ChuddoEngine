#pragma once

#include "Engine/GameFramework/World/Level.h"


class CPawn;


class CTestLevel : public CLevel
	{
	CHUDDO_DECLARE_CLASS ( CTestLevel, CLevel );
	public:
		CTestLevel ( CObject * inOwner, const std::string & inDisplayName );
		virtual ~CTestLevel () override;
		virtual void BeginPlay () override;
		virtual void EndPlay () override;
		virtual void Tick ( float DeltaTime ) override;
	private:
		// Здесь можно добавить любые тестовые данные или методы для уровня
		CPawn * TestCharacter = nullptr;
	};
REGISTER_CLASS_FACTORY( CTestLevel );