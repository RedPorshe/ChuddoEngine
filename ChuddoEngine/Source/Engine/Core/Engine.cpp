#include "Core/Engine.h"
#include "GameFramework/GameInstance.h"
#include "GameFramework/World/World.h"
#include "GameFramework/World/Level.h"
#include "GameFramework/Actors/Actor.h"
#include "GameFramework/GameMode.h"
#include "Components/Collisions/BaseCollisionComponent.h"

#include "Core/CollisionSystem.h"
#include "Render/RenderInterFace.h"
#include "Render/RenderInfo.h"
#include "Render/GLFWWindow.h"
#include "Render/Vulkan/VulkanRenderer.h"

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
    Renderer = std::make_unique<VulkanRenderer>();
    if(Renderer)
        {
        if (!Renderer->Initialize ())
            {
            LOG_FATAL ( "Failed to initialize Renderer" );
            return false;
            }
        }
    Window = Renderer->GetWindow ();
    if (Window == nullptr)
        {
        LOG_ERROR ( "Failed to get window from Renderer" );
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

    // Shutdown Renderer first to ensure all rendering/Vulkan resources are freed
    // before tearing down game objects which may reference them.
    if (Renderer != nullptr)
        {
        LOG_DEBUG ( "Engine::Shutdown() - calling Renderer->Shutdown()" );
        Renderer.get ()->Shutdown ();
        LOG_DEBUG ( "Engine::Shutdown() - Renderer shutdown returned" );
        }

    // Shutdown GameInstance after renderer has been stopped
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
    LOG_DEBUG ( "Exit requested from window or by reached 10 sec" );
    bIsRunning = false;
    }

CGameInstance & CEngine::GetGameInstance ()
    
    {
             return CGameInstance::Get (); 
    }

void CEngine::MainLoop ()
    {
   
    RenderScene* scene = new RenderScene(); // Пакет данных инициализация для передачи в гейм-инстанс и получения оттуда для рендера
    bIsRunning = true;
    int MaxFrames = 1000; // Ограничим для теста
   VulkanRenderer * vulkanRenderer = dynamic_cast<VulkanRenderer*>(Renderer.get());


    while (bIsRunning)
        {
        vulkanRenderer->GetWindowPtr().get()->PollEvents(); // Poll events to handle window close and other input
        CalculateDeltaTime ();
        Tick ( m_DeltaTime );
        scene->DeltaTime = m_DeltaTime;
        GetGameInstance ().RequestRenderData ( *scene );
        Renderer->Render ( *scene );
        // Автоматический выход после 10 секунд
        static float TotalTime = 0;
        TotalTime += m_DeltaTime;
        if (TotalTime > 10.0f)
            {
            LOG_DEBUG ( "[ENGINE] Auto exit after 10 seconds" );
            RequestExit ();
            }
        }
    if (vulkanRenderer->GetWindowPtr ().get ()->ShouldClose ()) RequestExit();
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