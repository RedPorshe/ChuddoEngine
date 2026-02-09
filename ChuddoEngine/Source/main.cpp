// main.cpp
#include "Core/Engine.h"
#include <iostream>
#include "tests.h"
#include "Components/BaseCollisionComponent.h"
#include "Core/CollisionSystem.h"

void TestCollisionChannels ()
    {
    if (!CGameInstance::Create ())
        {
        return;
        }
    COLLISION_SYSTEM;
    auto & GI = CGameInstance::Get ();
    auto world = GI.CreateWorld ( "TestWorld" );
    auto level = world->CreateLevel<CLevel> ( "TestLevel" );
    GI.Init ();

    // Создаём акторы с компонентами коллизий
    auto * wall = level->SpawnActorByClass ( "CActor", "Wall" );
    auto * crate = level->SpawnActorByClass ( "CActor", "Crate" );

    // Настраиваем позиции для теста коллизий
    wall->SetActorLocation ( 0, 0, 0 );
    crate->SetActorLocation ( 100, 0, 0 ); // Далеко - нет коллизии

    // Настраиваем разные каналы коллизий
    auto * wallCollision = wall->GetRootComponent ()->GetCollisionComponent ();
    auto * crateCollision = crate->GetRootComponent ()->GetCollisionComponent ();

    // Тест 1: Static vs Dynamic (должны коллайдить)
    wallCollision->SetChannelAsStatic ();
    crateCollision->SetChannelAsDynamic ();

    LOG_INFO ( "=== Test 1: Static vs Dynamic ===" );
    LOG_INFO ( "Can collide: ", wallCollision->CanCollideWith ( crateCollision ) );
    LOG_INFO ( "Should block: ", wallCollision->ShouldBlockWith ( crateCollision ) );

    // Тест 2: Trigger vs Dynamic (должны overlap)
    wallCollision->SetChannelAsTrigger ();

    LOG_INFO ( "=== Test 2: Trigger vs Dynamic ===" );
    LOG_INFO ( "Can collide: ", wallCollision->CanCollideWith ( crateCollision ) );
    LOG_INFO ( "Should overlap: ", wallCollision->ShouldOverlapWith ( crateCollision ) );

    // Тест 3: Same channel (Static vs Static)
    wallCollision->SetChannelAsStatic ();
    crateCollision->SetChannelAsStatic ();

    LOG_INFO ( "=== Test 3: Static vs Static ===" );
    LOG_INFO ( "Can collide: ", wallCollision->CanCollideWith ( crateCollision ) );

    // Тест 4: Custom responses
    wallCollision->SetChannelAsCustom ( "Player", ECollisionResponse::BLOCK );
    crateCollision->SetChannelAsCustom ( "Enemy", ECollisionResponse::OVERLAP );

    // Настраиваем ответы
    wallCollision->SetResponseToChannel ( "Enemy", ECollisionResponse::IGNORE );

    LOG_INFO ( "=== Test 4: Custom channels ===" );
    LOG_INFO ( "Player vs Enemy: ", wallCollision->CanCollideWith ( crateCollision ) );

    // НЕ вызываем overlapping вручную!
    // Система сама должна вызывать их при реальных коллизиях

    LOG_INFO ( "=== Test Complete ===" );
    GI.Shutdown ();
    }

bool bIsDebugfunc = false;

int main ()
	{
	setlocale ( LC_ALL, "ru" );
	LOG_INIT ( "Engine", false, true );
		// Установка уровня логирования в зависимости от конфигурации сборки
#ifdef _DEBUG
	LOG_SET_LEVEL ( CE::CLogger::CLogLevel::DEBUG );
	LOG_INFO ( "DEBUG CONFIG" );
#else
	LOG_SET_LEVEL ( CE::CLogger::CLogLevel::INFO );
	LOG_INFO ( "RELEASE CONFIG" );
#endif
	if (!bIsDebugfunc)
		{

		LOG_INFO ( "Initializing engine..." );
		if (!CEngine::InitializeEngine ())
			{
			LOG_FATAL ( "Failed to initialize engine!" );
			LOG_SHUTDOWN ();
			return EXIT_FAILURE;
			}

		auto & eng = CEngine::Get ();

		LOG_INFO ( "Starting engine..." );
		eng.Start ();

		LOG_INFO ( "Shutting down engine..." );
		CEngine::ShutdownEngine ();

		}
	else
		{
        TestCollisionScene ();
		}


	LOG_SHUTDOWN ();
	return EXIT_SUCCESS;
	};