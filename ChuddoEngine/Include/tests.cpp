#include "tests.h"
#include "Engine/GameFramework/Actors/Actor.h"
#include "Engine/GameFramework/Actors/TerrainActor.h"
#include "Engine/GameFramework/Components/Collisions/BaseCollisionComponent.h"
#include "Engine/GameFramework/Components/Collisions/BoxComponent.h"
#include "Engine/GameFramework/Components/Collisions/SphereComponent.h"
#include "Engine/GameFramework/Components/Collisions/CapsuleComponent.h"
#include "Engine/GameFramework/Components/Collisions/CylinderComponent.h"
#include "Engine/GameFramework/Components/Collisions/ConeComponent.h"
#include "Engine/GameFramework/GameInstance.h"
#include "Engine/Core/Engine.h"
#include "Engine/GameFramework/Actors/PlayerController.h"
#include "Engine/GameFramework/Actors/HUD.h"
#include "World/World.h"
#include "Actors/PlayerStart.h"
#include "TestCubeActor.h"

// Вспомогательный класс для тестирования цилиндра
class CTestCylinderActor : public CActor
    {
    CHUDDO_DECLARE_CLASS ( CTestCylinderActor, CActor );
    public:
        CTestCylinderActor ( CObject * inOwner = nullptr, const std::string InName = "TestCylinder" )
            : Super ( inOwner, InName )
            {
            if (GetRootComponent ())
                GetRootComponent ()->SetAutoGenerateCollisionComponent ( false );

            auto CylinderCollision = AddDefaultSubObject<CCylinderComponent> ( "Cylinder" );
            if (CylinderCollision)
                {
                CylinderCollision->SetRadius ( 30.0f );
                CylinderCollision->SetHeight ( 80.0f );
                CylinderCollision->SetCollisionEnabled ( true );
                CylinderCollision->SetChannelAsDynamic ();
                

                if (GetRootComponent ())
                    GetRootComponent ()->SetCollisionComponent ( CylinderCollision );

                LOG_DEBUG ( "[TESTCYLINDER] Created Cylinder: ", CylinderCollision->GetName () );
                }
            }
    };
REGISTER_CLASS_FACTORY ( CTestCylinderActor );

// Вспомогательный класс для тестирования конуса
class CTestConeActor : public CActor
    {
    CHUDDO_DECLARE_CLASS ( CTestConeActor, CActor );
    public:
        CTestConeActor ( CObject * inOwner = nullptr, const std::string InName = "TestCone" )
            : Super ( inOwner, InName )
            {
            if (GetRootComponent ())
                GetRootComponent ()->SetAutoGenerateCollisionComponent ( false );

            auto ConeCollision = AddDefaultSubObject<CConeComponent> ( "Cone" );
            if (ConeCollision)
                {
                ConeCollision->SetRadius ( 30.0f );
                ConeCollision->SetHeight ( 80.0f );
                ConeCollision->SetCollisionEnabled ( true );
                ConeCollision->SetChannelAsDynamic ();

                if (GetRootComponent ())
                    GetRootComponent ()->SetCollisionComponent ( ConeCollision );

                LOG_DEBUG ( "[TESTCONE] Created Cone: ", ConeCollision->GetName () );
                }
            }
    };
REGISTER_CLASS_FACTORY ( CTestConeActor );

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

        // ============================================================================
        // 1. SPHERE - уже есть
        // ============================================================================
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

        if (SphereCollision)
            {
            SphereCollision->SetChannelAsTrigger ();
            SphereCollision->SetResponseToChannel ( "Static", ECollisionResponse::BLOCK );
            SphereCollision->SetResponseToChannel ( "Dynamic", ECollisionResponse::OVERLAP ); // OVERLAP с другими динамическими
            LOG_DEBUG ( "[TESTLEVEL] Sphere configured: OVERLAP with Dynamic" );
            }
        }

        // ============================================================================
        // 2. BOX (капсула в тесте) - уже есть
        // ============================================================================
    ParentActor = SpawnActor<CTestCube> ( "TestCubeActor" );
    if (ParentActor)
        {
        CCapsuleComponent * CapsuleCollision = nullptr;
        auto comps = ParentActor->GetActorComponents ();
        for (auto comp : comps)
            {
            if (CCapsuleComponent * capsule = dynamic_cast< CCapsuleComponent * >( comp ))
                {
                CapsuleCollision = capsule;
                break;
                }
            }

             // Для капсулы
        if (CapsuleCollision)
            {
            CapsuleCollision->SetChannelAsPawn ();
            CapsuleCollision->SetResponseToChannel ( "Static", ECollisionResponse::BLOCK );
            CapsuleCollision->SetResponseToChannel ( "Dynamic", ECollisionResponse::OVERLAP ); // OVERLAP с другими динамическими
            LOG_DEBUG ( "[TESTLEVEL] Capsule configured: OVERLAP with Dynamic" );
            }
        }

        // ============================================================================
        // 3. CYLINDER
        // ============================================================================
    CTestCylinderActor * CylinderActor = SpawnActor<CTestCylinderActor> ( "TestCylinderActor" );
    if (CylinderActor)
        {
        CylinderActor->SetActorLocation ( 300.f, 100.f, 0.f );

        CCylinderComponent * CylinderCollision = nullptr;
        auto comps = CylinderActor->GetActorComponents ();
        for (auto comp : comps)
            {
            if (CCylinderComponent * cylinder = dynamic_cast< CCylinderComponent * >( comp ))
                {
                CylinderCollision = cylinder;
                break;
                }
            }

        if (CylinderCollision)
            {
            CylinderCollision->SetChannelAsDynamic ();
            CylinderCollision->SetResponseToChannel ( "Static", ECollisionResponse::BLOCK );
            CylinderCollision->SetResponseToChannel ( "Dynamic", ECollisionResponse::OVERLAP );
            LOG_DEBUG ( "[TESTLEVEL] Cylinder configured: OVERLAP with Dynamic" );
            }
        }

        // ============================================================================
        // 4. CONE
        // ============================================================================
    CTestConeActor * ConeActor = SpawnActor<CTestConeActor> ( "TestConeActor" );
    if (ConeActor)
        {
        ConeActor->SetActorLocation ( 400.f, 100.f, 0.f );

        CConeComponent * ConeCollision = nullptr;
        auto comps = ConeActor->GetActorComponents ();
        for (auto comp : comps)
            {
            if (CConeComponent * cone = dynamic_cast< CConeComponent * >( comp ))
                {
                ConeCollision = cone;
                break;
                }
            }

        if (ConeCollision)
            {
            ConeCollision->SetChannelAsDynamic ();
            ConeCollision->SetResponseToChannel ( "Static", ECollisionResponse::BLOCK );
            ConeCollision->SetResponseToChannel ( "Dynamic", ECollisionResponse::OVERLAP );
            LOG_DEBUG ( "[TESTLEVEL] Cone configured: OVERLAP with Dynamic" );
            }
        }

        // ============================================================================
        // 5. TERRAIN (3 вида)
        // ============================================================================

        // Плоский террейн
    CTerrainActor * flatTerrain = SpawnTerrainActor ( "FlatTerrain", 50, 50, 200.0f, -50.0f );
    if (flatTerrain)
        {
        flatTerrain->SetActorLocation ( -500.f, 0.f, -500.f );
        }

        // Холмистый террейн из карты высот
    std::vector<float> heights ( 100 * 100 );
    for (int z = 0; z < 100; z++)
        {
        for (int x = 0; x < 100; x++)
            {
            float h = sin ( x * 0.1f ) * cos ( z * 0.1f ) * 20.0f;
            heights[ z * 100 + x ] = h;
            }
        }
    CTerrainActor * hillyTerrain = SpawnTerrainActorFromHeightmap ( "HillyTerrain", heights, 100, 100, 50.0f );
    if (hillyTerrain)
        {
        hillyTerrain->SetActorLocation ( 500.f, 0.f, -500.f );
        }

        // Стандартный террейн
    CTerrainActor * defaultTerrain = SpawnTerrainActor ( "DefaultTerrain", 100, 100, 100.0f, 0.0f );
    if (defaultTerrain)
        {
        defaultTerrain->SetActorLocation ( 0.f, 0.f, -1000.f );
        }

    // Для сферы
    

   

        // Для цилиндра
  

        // Для конуса
   

    LOG_DEBUG ( "[TESTLEVEL] All test actors created" );
    }
  
    void CTestLevel::BeginPlay ()
        {
        Super::BeginPlay ();

        CActor * sphere = dynamic_cast< CActor * >( FindObjectByName ( "TestSphereActor" ) );
        CActor * capsule = dynamic_cast< CActor * >( FindObjectByName ( "TestCubeActor" ) );
        CActor * cylinder = dynamic_cast< CActor * >( FindObjectByName ( "TestCylinderActor" ) );
        CActor * cone = dynamic_cast< CActor * >( FindObjectByName ( "TestConeActor" ) );

        if (sphere) sphere->SetActorLocation ( 200.f, 100.f, 0.f );
        if (capsule) capsule->SetActorLocation ( 200.f, 100.f, 0.f ); // То же место для немедленного Overlap
        if (cylinder) cylinder->SetActorLocation ( 300.f, 100.f, 0.f );
        if (cone) cone->SetActorLocation ( 400.f, 100.f, 0.f );

        LOG_DEBUG ( "[TESTLEVEL] BeginPlay completed with all shape types" );
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
    }

void CTestGameMode::StartPlay ()
    {
    Super::StartPlay ();
    }

void CTestGameMode::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }

void CTestLevel::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );

    // Двигаем объекты для проверки Overlap
    static float time = 0.0f;
    time += DeltaTime;

    // Находим все тестовые акторы
    CActor * sphere = dynamic_cast< CActor * >( FindObjectByName ( "TestSphereActor" ) );
    CActor * capsule = dynamic_cast< CActor * >( FindObjectByName ( "TestCubeActor" ) );
    CActor * cylinder = dynamic_cast< CActor * >( FindObjectByName ( "TestCylinderActor" ) );
    CActor * cone = dynamic_cast< CActor * >( FindObjectByName ( "TestConeActor" ) );

    if (sphere && capsule)
        {
            // Двигаем сферу по кругу вокруг капсулы
        float offsetX = sin ( time * 2.0f ) * 150.0f;
        float offsetZ = cos ( time * 2.0f ) * 150.0f;
        sphere->SetActorLocation ( 200.0f + offsetX, 100.0f, offsetZ );
        }

    if (cylinder && cone)
        {
            // Двигаем цилиндр и конус навстречу друг другу
        float cylinderX = 300.0f + sin ( time * 3.0f ) * 100.0f;
        float coneX = 400.0f - sin ( time * 3.0f ) * 100.0f;

        cylinder->SetActorLocation ( cylinderX, 100.0f, 0.0f );
        cone->SetActorLocation ( coneX, 100.0f, 0.0f );
        }

        // Логируем расстояния для отладки
    static float logTimer = 0.0f;
    logTimer += DeltaTime;
    if (logTimer >= 1.0f)
        {
        logTimer = 0.0f;

        if (sphere && capsule)
            {
            float dist = ( sphere->GetActorLocation () - capsule->GetActorLocation () ).Length ();
            LOG_DEBUG ( "[TEST] Sphere-Capsule distance: ", dist );
            }

        if (cylinder && cone)
            {
            float dist = ( cylinder->GetActorLocation () - cone->GetActorLocation () ).Length ();
            LOG_DEBUG ( "[TEST] Cylinder-Cone distance: ", dist );
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
