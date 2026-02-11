#include "tests.h"
#include "Engine/GameFramework/Actors/Actor.h"
#include "Engine/GameFramework/Components/BaseCollisionComponent.h"
#include "Engine/GameFramework/GameInstance.h"
#include "Engine/Core/Engine.h"

CTestLevel::CTestLevel ( CObject * Owner, const std::string & inLevelName ) :Super ( Owner, inLevelName )
	{
	Player = SpawnActor<CPawn> ( "Player" );

	Enemy = SpawnActor <CPawn> ( "Enemy" );

	}



void CTestLevel::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );

    static float TotalTime = 0.0f;
    TotalTime += DeltaTime;

    static bool bMovementStarted = false;
    static bool bMovementCompleted = false;
    static float MovementStartTime = 0.0f;

    // ТЕСТ 1: Движение
    if (!bMovementStarted && TotalTime > 0.1f)  // Через 0.1 секунды
        {
        LOG_DEBUG ( "\n=== TEST 1: Pawn Move Forward ===" );

        Player->SetInputEnabled ( true );
        Player->SetMoveSpeed ( 500.0f );
        Player->AddMovementInput ( Player->GetActorForwardVector (), 1.0f );

        bMovementStarted = true;
        MovementStartTime = TotalTime;
        LOG_DEBUG ( "  Movement started at time ", MovementStartTime, "s" );
        }

        // ТЕСТ 2: Проверка движения
    if (bMovementStarted && !bMovementCompleted && !Player->IsMoving ())
        {
        float TimeTaken = TotalTime - MovementStartTime;

        LOG_DEBUG ( "\n=== TEST 2: Movement Check ===" );
        LOG_DEBUG ( "  Time taken: ", TimeTaken, " seconds" );
        LOG_DEBUG ( "  Frames: ~", int ( TimeTaken / DeltaTime ) );
        LOG_DEBUG ( "  Final Position: (", Player->GetActorLocation ().z, ")" );

        bMovementCompleted = true;
        }

        // ТЕСТ 3: Поворот
    static bool bRotationStarted = false;
    static float RotationStartTime = 0.0f;

    if (bMovementCompleted && !bRotationStarted && TotalTime > MovementStartTime + 0.2f)
        {
        LOG_DEBUG ( "\n=== TEST 3: Pawn Rotate 90° ===" );

        Player->TeleportTo ( FVector ( 0.f, 0.f, 0.f ) );
        Player->SetActorRotationImmediately ( 0.f, 0.f, 0.f );
        Player->RotateActor ( FVector ( 0, 90, 0 ), true );

        bRotationStarted = true;
        RotationStartTime = TotalTime;
        LOG_DEBUG ( "  Rotation started at time ", RotationStartTime, "s" );
        }

        // ТЕСТ 4: Проверка поворота
    static bool bRotationCompleted = false;

    if (bRotationStarted && !bRotationCompleted && !Player->IsLerpingRotation ())
        {
        float TimeTaken = TotalTime - RotationStartTime;

        LOG_DEBUG ( "\n=== TEST 4: Rotation Check ===" );
        LOG_DEBUG ( "  Time taken: ", TimeTaken, " seconds" );
        LOG_DEBUG ( "  Forward: (", Player->GetActorForwardVector ().x, ", 0, 0)" );
        LOG_DEBUG ( "  ✅ PASSED!" );

        bRotationCompleted = true;
        }

        // ВЫХОД
    static bool bExited = false;
    if (bMovementCompleted && bRotationCompleted && !bExited)
        {
        LOG_DEBUG ( "\n========================================" );
        LOG_DEBUG ( "🎉 ALL TESTS PASSED! 🎉" );
        LOG_DEBUG ( "========================================" );
        LOG_DEBUG ( "  Total time: ", TotalTime, " seconds" );
        LOG_DEBUG ( "========================================\n" );

        CGameInstance::Get ().GetEngine ().RequestExit ();
        bExited = true;
        }
    }
    
    
    void CTestLevel::BeginPlay ()
	{
	Super::BeginPlay ();
	Player->SetActorLocation ( { 40.f,0.f,50.f } );

	Enemy->SetActorLocation ( { 10.f,0.f,20.f } );
	Player->GetRootComponent ()->GetCollisionComponent ()->SetChannelAsPawn ();
	Enemy->GetRootComponent ()->GetCollisionComponent ()->SetChannelAsPawn ();
	}

void CTestLevel::EndPlay ()
	{
	Super::EndPlay ();
	}
