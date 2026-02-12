#include "tests.h"
#include "Engine/GameFramework/Actors/Actor.h"
#include "Engine/GameFramework/Components/BaseCollisionComponent.h"
#include "Engine/GameFramework/GameInstance.h"
#include "Engine/Core/Engine.h"
#include "Engine/GameFramework/Actors/PlayerController.h"
#include "Engine/GameFramework/Actors/HUD.h"
#include "World/World.h"
#include "Actors/PlayerStart.h"

CTestLevel::CTestLevel ( CObject * Owner, const std::string & inLevelName ) :Super ( Owner, inLevelName )
	{
   // ✅ 1. ТОЛЬКО PlayerStart - GameMode сам создаст игрока!
    CPlayerStart * PlayerStart = SpawnActor<CPlayerStart> ( "MainPlayerStart" );
    if (PlayerStart)
        {
        PlayerStart->SetActorLocation ( 0.f, 0.f, 0.f );
        }

        // ✅ 2. Тестовые акторы для иерархии (нужны для TEST 5)
    ParentActor = SpawnActor<CActor> ( "ParentActor" );
    if (ParentActor)
        {
        ParentActor->SetActorLocation ( 100.f, 0.f, 0.f );
        }

    ChildActor = SpawnActor<CActor> ( "ChildActor" );
    if (ChildActor)
        {
        ChildActor->SetActorLocation ( 50.f, 0.f, 0.f );
        }

	}



void CTestLevel::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    static int exitc = 0;
    if (exitc > 20)
        {
        CGameInstance::Get ().GetEngine ().RequestExit ();
        }
    exitc++;
    }
    

    
    void CTestLevel::BeginPlay ()
	{
	Super::BeginPlay ();
     

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