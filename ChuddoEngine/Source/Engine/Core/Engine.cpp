#include "Core/Engine.h"
#include "GameFramework/GameInstance.h"
#include "World/World.h"
#include "World/Level.h"
#include "Actors/Actor.h"
#include "Components/BaseCollisionComponent.h"
#include "Core/CollisionSystem.h"

#include "tests.h"



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
	LOG_WARN ("Engine Start");
	CreateTestWorld ();
	
	MainLoop ();
	
	}

void CEngine::RequestExit ()
	{
	bIsRunning = false;
	}


void CEngine::MainLoop ()
    {
	LOG_WARN ("Main loop Start");
	
    CGameInstance::Get ().Init ();
	bIsRunning = true;
    static int frame = 0;

    while (bIsRunning)  
        {        
		CalculateDeltaTime ();
		
        Tick ( m_DeltaTime );
	
        frame++;
		//if (frame > 15) break;
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
	auto word = CGameInstance::Get ().CreateWorld (  "Super" );
	word->CreateLevel<CTestLevel> ( "SuperLevel" );
	}

CEngine::CEngine () : CollisionSystem( COLLISION_SYSTEM )
	{
	
	};


