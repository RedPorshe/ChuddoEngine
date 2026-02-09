#include "tests.h"

#include "Components/SphereComponent.h"

void TestCollisionScene ()
     {
    LOG_INFO ( "=== SIMPLE COLLISION DETECTION TEST ===" );

    if (!CGameInstance::Create ())
        {
        LOG_ERROR ( "Failed to create game instance" );
        return;
        }

    auto & GI = CGameInstance::Get ();
    auto world = GI.CreateWorld ( "TestWorld" );
    auto level = world->CreateLevel<CLevel> ( "TestLevel" );
    GI.Init ();

    // Создаем ТОЛЬКО два актора для простоты
    CActor * obj1 = level->SpawnActor<CActor> ( "Object1" );
    CActor * obj2 = level->SpawnActor<CActor> ( "Object2" );

    // Добавляем коллизионные компоненты
    CSphereComponent * coll1 =
        obj1->AddSubObject<CSphereComponent> ( "Collision1", 25.0f ); // Радиус 25
    CSphereComponent * coll2 =
        obj2->AddSubObject<CSphereComponent> ( "Collision2", 25.0f ); // Радиус 25

    // Настраиваем каналы (должны коллайдить)
    coll1->SetChannelAsCharacter ();
    coll2->SetChannelAsPawn ();
    coll1->SetResponseToChannel ( "Pawn", ECollisionResponse::BLOCK );
    coll2->SetResponseToChannel ( "Character", ECollisionResponse::BLOCK );

    // Тест 1: Объекты далеко (нет коллизии)
    LOG_INFO ( "\n=== TEST 1: OBJECTS FAR APART ===" );
    obj1->SetActorLocation ( 0, 0, 0 );
    obj2->SetActorLocation ( 100, 0, 0 ); // Расстояние 100, радиусы 25+25=50

    GI.Tick ( 0.016f );
    COLLISION_SYSTEM.Update ( 0.016f );

    LOG_INFO ( "Distance: 100, Sum of radii: 50" );
    LOG_INFO ( "Should NOT collide" );

    // Тест 2: Объекты касаются (коллизия с depth=0)
    LOG_INFO ( "\n=== TEST 2: OBJECTS TOUCHING ===" );
    obj1->SetActorLocation ( 0, 0, 0 );
    obj2->SetActorLocation ( 50, 0, 0 ); // Расстояние 50, радиусы 25+25=50

    GI.Tick ( 0.016f );
    COLLISION_SYSTEM.Update ( 0.016f );

    LOG_INFO ( "Distance: 50, Sum of radii: 50" );
    LOG_INFO ( "Should collide with depth=0" );

    // Тест 3: Объекты пересекаются (сильная коллизия)
    LOG_INFO ( "\n=== TEST 3: OBJECTS OVERLAPPING ===" );
    obj1->SetActorLocation ( 0, 0, 0 );
    obj2->SetActorLocation ( 25, 0, 0 ); // Расстояние 25, радиусы 25+25=50

    GI.Tick ( 0.016f );
    COLLISION_SYSTEM.Update ( 0.016f );

    LOG_INFO ( "Distance: 25, Sum of radii: 50" );
    LOG_INFO ( "Should collide with depth=25" );

    // Тест 4: Объекты на одной позиции (максимальная коллизия)
    LOG_INFO ( "\n=== TEST 4: OBJECTS AT SAME POSITION ===" );
    obj1->SetActorLocation ( 0, 0, 0 );
    obj2->SetActorLocation ( 0, 0, 0 );

    GI.Tick ( 0.016f );
    COLLISION_SYSTEM.Update ( 0.016f );

    LOG_INFO ( "Distance: 0, Sum of radii: 50" );
    LOG_INFO ( "Should collide with depth=50" );

    // Проверяем вручную
    LOG_INFO ( "\n=== MANUAL COLLISION CHECK ===" );

    // Получаем позиции
    FVector pos1 = coll1->GetWorldLocation ();
    FVector pos2 = coll2->GetWorldLocation ();
    float distance = ( pos2 - pos1 ).Length ();

    LOG_INFO ( "Object1 at: (", pos1.x, ", ", pos1.y, ", ", pos1.z, ")" );
    LOG_INFO ( "Object2 at: (", pos2.x, ", ", pos2.y, ", ", pos2.z, ")" );
    LOG_INFO ( "Actual distance: ", distance );
    LOG_INFO ( "Sum of radii: 50" );

    // Простая математическая проверка
    if (distance < 50.0f)
        {
        LOG_INFO ( "MATHEMATICALLY: Should be colliding (distance < sum of radii)" );
        }
    else if (distance == 50.0f)
        {
        LOG_INFO ( "MATHEMATICALLY: Touching (distance = sum of radii)" );
        }
    else
        {
        LOG_INFO ( "MATHEMATICALLY: Not colliding (distance > sum of radii)" );
        }

    LOG_INFO ( "\n=== SYSTEM STATS ===" );
    LOG_INFO ( "Registered components: ", COLLISION_SYSTEM.GetRegisteredComponentsCount () );
    LOG_INFO ( "Active collisions: ", COLLISION_SYSTEM.GetActiveCollisionsCount () );

    LOG_INFO ( "\n=== TEST COMPLETE ===" );
    GI.Shutdown ();
    }
