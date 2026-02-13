#include "tests.h"
#include "Engine/GameFramework/Actors/Actor.h"
#include "Engine/GameFramework/Actors/TerrainActor.h"
#include "Engine/GameFramework/Components/Collisions/BaseCollisionComponent.h"
#include "Engine/GameFramework/Components/Collisions/BoxComponent.h"
#include "Engine/GameFramework/Components/Collisions/SphereComponent.h"
#include "Engine/GameFramework/Components/Collisions/CapsuleComponent.h"
#include "Engine/GameFramework/GameInstance.h"
#include "Engine/Core/Engine.h"
#include "Engine/GameFramework/Actors/PlayerController.h"
#include "Engine/GameFramework/Actors/HUD.h"
#include "World/World.h"
#include "Actors/PlayerStart.h"
#include "TestCubeActor.h"

CTestLevel::CTestLevel ( CObject * Owner, const std::string & inLevelName )
	: Super ( Owner, inLevelName )
	{
		// PlayerStart
	CPlayerStart * PlayerStart = SpawnActor<CPlayerStart> ( "MainPlayerStart" );
	if (PlayerStart)
		{
		PlayerStart->SetActorLocation ( 0.f, 0.f, 0.f );
		if (PlayerStart->GetRootComponent ())
			{
			PlayerStart->GetRootComponent ()->SetAutoGenerateCollisionComponent ( false );
			}
		}

		// Test Cube - СТАТИЧЕСКИЙ
	ParentActor = SpawnActor<CTestCube> ( "TestCubeActor" );
	if (ParentActor)
		{
		

		CCapsuleComponent * BoxCollision = nullptr;
		auto comps = ParentActor->GetActorComponents ();
		for (auto comp : comps)
			{
			if (CCapsuleComponent * box = dynamic_cast< CCapsuleComponent * >( comp ))
				{
				BoxCollision = box;
				break;
				}
			}

			// НАСТРАИВАЕМ ЗДЕСЬ!
		if (BoxCollision)
			{
			BoxCollision->SetChannelAsDynamic ();
			BoxCollision->SetResponseToChannel ( "Dynamic", ECollisionResponse::OVERLAP );
			BoxCollision->SetResponseToChannel ( "Static", ECollisionResponse::BLOCK );
			BoxCollision->SetResponseToChannel ( "Pawn", ECollisionResponse::OVERLAP );
			
			}
		}

		// Test Sphere - ДИНАМИЧЕСКИЙ
	ChildActor = SpawnActor<CTestSphere> ( "TestSphereActor" );
	if (ChildActor)
		{
		
		CSphereComponent * SphereCollision = nullptr;
		auto comps = ChildActor->GetActorComponents ();
		for (auto comp : comps)
			{
			if (CSphereComponent * sphere = dynamic_cast< CSphereComponent * >( comp ))
				{
				SphereCollision = sphere;
				break;
				}
			}

			// НАСТРАИВАЕМ ЗДЕСЬ!
		if (SphereCollision)
			{
			SphereCollision->SetChannelAsDynamic ();
			SphereCollision->SetResponseToChannel ( "Static", ECollisionResponse::BLOCK );
			SphereCollision->SetResponseToChannel ( "Dynamic", ECollisionResponse::OVERLAP );
			LOG_DEBUG ( "[TESTLEVEL] Sphere configured: Dynamic, Block with Static, Overlap with Dynamic" );
			}
		}
	CTerrainActor * flatTerrain = SpawnTerrainActor ( "FlatTerrain", 50, 50, 200.0f, -50.0f );

// Создаём террейн с холмами из карты высот
	std::vector<float> heights ( 100 * 100 );
	for (int z = 0; z < 100; z++)
		{
		for (int x = 0; x < 100; x++)
			{
				// Простая синусоида для примера
			float h = sin ( x * 0.1f ) * cos ( z * 0.1f ) * 20.0f;
			heights[ z * 100 + x ] = h;
			}
		}
	CTerrainActor * hillyTerrain = SpawnTerrainActorFromHeightmap ( "HillyTerrain", heights, 100, 100, 50.0f );

	// Используем упрощённый метод
	SpawnTerrain ();
	}


void CTestLevel::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );
	static std::string ActorName {};
	static std::string ActorName2 {};
	
	if (ParentActor != nullptr)
		{
		static int dir = 0;
		if (dir < 1)
			{
			ActorName2 = ParentActor->GetName ();
			dir++;
			}
		auto tocheck = FindObjectByName ( ActorName2 );
		if (tocheck == nullptr) ParentActor = nullptr;
		if (CTestCube * spher = dynamic_cast< CTestCube * > ( tocheck ))
			{
			static float time = 0.f;
			if (time > 1.5f)
				{
				LOG_DEBUG ( ActorName2," location y = ", spher->GetActorLocation ().y );
				time = 0.f;
				}
			time+=DeltaTime;
			}
		}
		// Выход после 10 секунд
	static float exitTimer = 0.0f;
	exitTimer += DeltaTime;
	if (exitTimer > 10.0f)
		{
		CGameInstance::Get ().GetEngine ().RequestExit ();
		}
	}



void CTestLevel::BeginPlay ()
	{
	Super::BeginPlay ();
	ChildActor->SetActorLocation ( 200.f, 100.f, 0.f );
	ParentActor->SetActorLocation ( 0.f, 100.f, 0.f );

	LOG_DEBUG ( "[TESTLEVEL] BeginPlay completed" );
	}

void CTestLevel::EndPlay ()
	{
	Super::EndPlay ();
	}




CTestGameMode::CTestGameMode ( CObject * inOwner, const std::string & inName )
	: Super ( inOwner, inName )
	{
		// Устанавливаем кастомные классы для теста
	SetDefaultPawnClass ( "CPawn" );
	SetDefaultPlayerControllerClass ( "CPlayerController" );
	SetDefaultHUDClass ( "CHUD" );

	LOG_DEBUG ( "[TESTGAMEMODE] Created: ", GetName () );
	}

CTestGameMode::~CTestGameMode ()
	{}

void CTestGameMode::InitGame ()
	{
	Super::InitGame ();
	LOG_DEBUG ( "[TESTGAMEMODE] InitGame: ", GetName () );
	}

void CTestGameMode::StartPlay ()
	{
	Super::StartPlay ();
	LOG_DEBUG ( "[TESTGAMEMODE] StartPlay - Game Started!" );
	LOG_DEBUG ( "[TESTGAMEMODE] Auto-spawning player: ",
				ShouldSpawnPlayerAutomatically () ? "Yes" : "No" );
	}

void CTestGameMode::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );

	static float LogTimer = 0.0f;
	LogTimer += DeltaTime;

	// Логируем каждые 5 секунд для проверки
	if (LogTimer >= 5.0f)
		{
		LOG_DEBUG ( "[TESTGAMEMODE] Game Time: ", GetGameTime (), " seconds" );
		LOG_DEBUG ( "[TESTGAMEMODE] Players: ", GetNumPlayers () );
		LogTimer = 0.0f;
		}
	}