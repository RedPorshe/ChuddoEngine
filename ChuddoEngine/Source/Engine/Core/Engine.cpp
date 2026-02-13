#include "Core/Engine.h"
#include "GameFramework/GameInstance.h"
#include "GameFramework/World/World.h"
#include "GameFramework/World/Level.h"
#include "GameFramework/Actors/Actor.h"
#include "GameFramework/GameMode.h"
#include "Components/Collisions/BaseCollisionComponent.h"
#include "Core/CollisionSystem.h"
#include "tests.h"
#include <iostream>
#include <fstream>

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

    COLLISION_SYSTEM;
    bIsInitialized = true;
    LOG_INFO ( "Engine initialized" );
    return true;
    }

void CEngine::Shutdown ()
    {
    if (!bIsInitialized)
        return;

    LOG_INFO ( "Engine shutting down..." );

    // Shutdown GameInstance first
    auto & GameInstance = CGameInstance::Get ();

    if (GameInstance.IsMustSaveState ())
        {
        LOG_INFO ( "saving gameInstance state" );
        GameInstance.SaveGameInstanceState ();
        }

    GameInstance.Shutdown ();
    CGameInstance::Destroy ();

    bIsInitialized = false;
    bIsRunning = false;

    LOG_DEBUG ( "Engine shutdown complete" );
    }

void CEngine::Start ()
    {
   
    CreateTestWorld ();

    auto & GameInstance = CGameInstance::Get ();
    GameInstance.Init ();

    
    MainLoop ();
    }

void CEngine::RequestExit ()
    {
    bIsRunning = false;
    }

CGameInstance & CEngine::GetGameInstance ()
    
    {
             return CGameInstance::Get (); 
    }

void CEngine::MainLoop ()
    {
   

    bIsRunning = true;
    int MaxFrames = 1000; // Ограничим для теста

    while (bIsRunning)
        {
        CalculateDeltaTime ();
        Tick ( m_DeltaTime );

        // Автоматический выход после 10 секунд
        static float TotalTime = 0;
        TotalTime += m_DeltaTime;
        if (TotalTime > 10.0f)
            {
            LOG_DEBUG ( "[ENGINE] Auto exit after 10 seconds" );
            RequestExit ();
            }
        }
    }

void CEngine::Tick ( float deltaTime )
    {
    CGameInstance::Get ().Tick ( deltaTime );
    CollisionSystem.Update ( deltaTime );
    }

void CEngine::CalculateDeltaTime ()
    {
    auto currentTime = std::chrono::steady_clock::now ();

    if (m_LastFrameTime.time_since_epoch ().count () != 0)
        {
        auto delta = std::chrono::duration_cast< std::chrono::microseconds >(
            currentTime - m_LastFrameTime
        ).count ();

        m_DeltaTime = delta * 0.000001f;

        if (m_DeltaTime > 0.033f)
            m_DeltaTime = 0.033f;
        }
    else
        {
        m_DeltaTime = 0.016f;
        }

    m_LastFrameTime = currentTime;
    }

void CEngine::CreateTestWorld ()
    {
    auto world = CGameInstance::Get ().CreateWorld ( "Super" );
    if (world)
        {
        world->CreateLevel<CTestLevel> ( "SuperLevel" );
        LOG_DEBUG ( "[ENGINE] Test world created: Super with level: SuperLevel" );
        }
    }

CEngine::CEngine () : CollisionSystem ( COLLISION_SYSTEM )
    {}