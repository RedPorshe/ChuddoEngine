#include "Core/Engine.h"
#include "GameFramework/GameInstance.h"
#include "World/World.h"
#include "World/Level.h"
#include "Actors/Actor.h"

CEngine * CEngine::Instance = nullptr;

CEngine::~CEngine ()
    {
    std::cout << "Engine destroyed\n";

    if (bIsInitialized)
        {
        Shutdown ();
        }
    }

CEngine & CEngine::Get ()
    {
    return *Instance;
    }

// Статические методы для управления жизненным циклом
bool CEngine::InitializeEngine ()
    {
    if (Instance)
        {
        std::cerr << "Engine already initialized!\n";
        return false;
        }

    Instance = new CEngine ();
    return Instance->Initialize ();
    }

void CEngine::ShutdownEngine ()
    {
    if (Instance)
        {
        delete Instance;  // Ключевая строка!
        Instance = nullptr;
        }
    }

bool CEngine::Initialize ()
    {
    if (bIsInitialized)
        {
        std::cout << "Engine already initialized\n";
        return true;
        }

    if (!CGameInstance::Create ())
        {
        std::cout << "Failed to create GameInstance\n";
        return false;
        }

    bIsInitialized = true;
    std::cout << "Engine initialized\n";
    return true;
    }

void CEngine::Shutdown ()
    {
    if (!bIsInitialized)
        return;

    std::cout << "Engine shutting down...\n";
    // сохраняем состояние если нужно
    if (CGameInstance::Get ().IsMustSaveState ())
        {
        std::cout << "saving gameInstance state\n";
        CGameInstance::Get ().SaveGameInstanceState ();
        }
    // Уничтожаем GameInstance
    CGameInstance::Destroy ();

    bIsInitialized = false;
    bIsRunning = false;

    std::cout << "Engine shutdown complete\n";
    }

void CEngine::Start ()
    {
   
    CreateTestWorld ();
    
    MainLoop ();
    }

void CEngine::MainLoop ()
    {
    CGameInstance::Get ().Init ();
    static int count = 0;
    while (count < 5)
        {
        std::cout << count << " frame\n";
        Tick ( 0.016f );
        count++;
        }
    }

void CEngine::Tick ( float deltaTime )
    {
    CGameInstance::Get ().Tick ( deltaTime );
    

    }

void CEngine::CreateTestWorld ()
    {
    std::cout << "========================================\n";
    std::cout << "   CHUDDO ENGINE - TEST CREATE WORLD   \n";
    std::cout << "========================================\n";
    
    auto word = CGameInstance::Get ().CreateWorld ( "testworld" );
    auto level = word->CreateLevel ( "Test level" );
    auto pla = level->SpawnActor<CActor> ( "Player" );
    auto enem = level->SpawnActor<CActor> ( "Enemy" );
    pla->AddSubObject<CObject> ( "Camera" );
    pla->AddSubObject<CObject> ( "Inventory" );
    enem->AddSubObject<CObject> ( "Axe" );
    enem->AddSubObject<CObject> ( "Inventory" );

    std::cout << "\n========================================\n";
    std::cout << "            TEST CREATION  COMPLETE             \n";
    std::cout << "========================================\n";
    }
