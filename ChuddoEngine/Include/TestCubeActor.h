#pragma once

#include "Actors/Actor.h"
#include "Components/MeshComponent.h"
class CStaticMeshComponent;

class CTestCube : public CActor
	{
	CHUDDO_DECLARE_CLASS ( CTestCube, CActor );
	public:
		CTestCube ( CObject * inOwner = nullptr, const std::string InName = "TestCube" );
		virtual ~CTestCube ();
		virtual void BeginPlay () override;
		virtual void Tick ( float deltaTime ) override;
		virtual void EndPlay () override;
		virtual void OnComponentHit ( CBaseCollisionComponent * other ) override;
	private:
    bool bIsCanMove = true;
	CStaticMeshComponent * MeshComponent = nullptr;
	};

REGISTER_CLASS_FACTORY ( CTestCube );

class CTestSphere : public CActor
	{
	CHUDDO_DECLARE_CLASS ( CTestSphere, CActor );
	public:
		CTestSphere ( CObject * inOwner = nullptr, const std::string InName = "TestSphere" );
		virtual ~CTestSphere ();
		virtual void BeginPlay () override;
		virtual void Tick ( float deltaTime ) override;
		virtual void EndPlay () override;
		virtual void OnComponentHit ( CBaseCollisionComponent * other ) override;
      CMeshComponent * MeshComponent = nullptr;
		CBaseCollisionComponent* GetCollisionComponent ();
	private: 
		float AcumTime = 0.f;
		bool bIsCanMove = true;
		float stoptimer = 0.5f;
		float time = 0.f;
		float resp = 0.f;
	};

REGISTER_CLASS_FACTORY ( CTestSphere );