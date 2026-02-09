#include "Core/Engine.h"
#include "GameFramework/GameInstance.h"
#include "World/World.h"
#include "World/Level.h"
#include "Actors/Actor.h"


CEngine * CEngine::Instance = nullptr;

CEngine::~CEngine ()
	{

	if (bIsInitialized)
		{
		Shutdown ();
		}
	LOG_INFO ( "Engine destroyed" );
	}

CEngine & CEngine::Get ()
	{
	return *Instance;
	}

bool CEngine::InitializeEngine ()
	{
	if (Instance)
		{
		LOG_ERROR ( "Engine already initialized!" );
		return false;
		}

	Instance = new CEngine ();
	return Instance->Initialize ();
	}

void CEngine::ShutdownEngine ()
	{
	if (Instance)
		{
		delete Instance;
		Instance = nullptr;
		}
	}

bool CEngine::Initialize ()
	{
	if (bIsInitialized)
		{
		LOG_WARN ( "Engine already initialized" );
		return true;
		}

	if (!CGameInstance::Create ())
		{
		LOG_FATAL ( "Failed to create GameInstance" );
		return false;
		}

	bIsInitialized = true;
	LOG_INFO ( "Engine initialized" );
	return true;
	}

void CEngine::Shutdown ()
	{
	if (!bIsInitialized)
		return;

	LOG_INFO ( "Engine shutting down..." );

	if (CGameInstance::Get ().IsMustSaveState ())
		{
		LOG_INFO ( "saving gameInstance state" );
		CGameInstance::Get ().SaveGameInstanceState ();
		}

	CGameInstance::Destroy ();

	bIsInitialized = false;
	bIsRunning = false;

	LOG_DEBUG ( "Engine shutdown complete" );
	}

void CEngine::Start ()
	{
	CreateTestWorld ();

	MainLoop ();
	}


void CEngine::MainLoop ()
    {
    LOG_INFO ( "=== STARTING MAIN LOOP TEST ===" );

    CGameInstance::Get ().Init ();
    auto level = CGameInstance::Get ().GetWorld ()->GetCurrentLevel ();

    // Создаем главный актор
    auto actor = level->SpawnActor<CActor> ( "TestActor" );
    actor->SetActorLocation ( 0, 0, 0 );

    LOG_INFO ( "Actor created. Initial setup complete." );

    static int frame = 0;

    while (frame < 12)  // 12 кадров чтобы увидеть все изменения
        {
            // ВАЖНО: сначала логируем текущее состояние (результат ПРЕДЫДУЩЕГО кадра)
        LOG_INFO ( "=== Frame ", frame, " ===" );
        LOG_INFO ( "Position: ", actor->GetActorLocation () );
        LOG_INFO ( "Rotation: ", actor->GetActorRotation () );

        // Затем выполняем действие на ЭТОМ кадре
        switch (frame)
            {
                case 0:
                    {
                    LOG_INFO ( "Action: Instant move to (10, 0, 0)" );
                    actor->MoveActor ( FVector ( 10, 0, 0 ), false );
                    break;
                    }
                case 1:
                    {
               // Пустой кадр - просто наблюдаем результат предыдущего действия
                    LOG_INFO ( "(Observing result of instant move)" );
                    break;
                    }
                case 2:
                    {
                    LOG_INFO ( "Action: Rotate 45 degrees around Y axis" );
                    actor->RotateActor ( FVector ( 0, 45, 0 ), false );
                    break;
                    }
                case 3:
                    {
               // Пустой кадр - наблюдаем результат поворота
                    LOG_INFO ( "(Observing result of rotation)" );
                    break;
                    }
                case 4:
                    {
                    LOG_INFO ( "Action: Move forward 5 units (local space)" );
                    actor->AddActorLocalOffset ( FVector ( 0, 0, 5 ) );
                    break;
                    }
                case 5:
                    {
               // Пустой кадр - наблюдаем результат локального смещения
                    LOG_INFO ( "(Observing result of local offset)" );
                    break;
                    }
                case 6:
                    {
                    LOG_INFO ( "Action: Move in direction (1, 1, 0) distance 7" );
                    actor->MoveActorInDirection ( FVector ( 1, 1, 0 ), 7.0f, false ); // мгновенно
                    break;
                    }
                case 7:
                    {
               // Пустой кадр - наблюдаем результат движения по направлению
                    LOG_INFO ( "(Observing result of direction move)" );
                    break;
                    }
                case 8:
                    {
                    LOG_INFO ( "Action: Test interpolated move (5, 0, 0)" );
                    actor->MoveActor ( FVector ( 5, 0, 0 ), true );  // с интерполяцией!
                    break;
                    }
                case 9:
                case 10:
                case 11:
                    {
              // Несколько кадров для наблюдения интерполяции
                    LOG_INFO ( "(Interpolation in progress...)" );
                    break;
                    }
            }

            // Обновляем движок - изменения применятся в СЛЕДУЮЩЕМ кадре
        Tick ( 0.016f );
        frame++;
        }

    LOG_INFO ( "=== MAIN LOOP TEST COMPLETE ===" );

   
    }

void CEngine::Tick ( float deltaTime )
	{
	CGameInstance::Get ().Tick ( deltaTime );
	}
//#include "tests.h"

void CEngine::CreateTestWorld ()
	{
	auto word = CGameInstance::Get ().CreateWorld ( "testworld" );
	word->CreateLevel<CLevel> ( "New Level" );
	}


