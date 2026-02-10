#include "Core/Engine.h"
#include "GameFramework/GameInstance.h"
#include "World/World.h"
#include "World/Level.h"
#include "Actors/Actor.h"
#include "Components/BaseCollisionComponent.h"
#include "Core/CollisionSystem.h"





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
	CreateTestWorld ();

	MainLoop ();
	
	}


void CEngine::MainLoop ()
    {
	

    CGameInstance::Get ().Init ();
    auto level = CGameInstance::Get ().GetWorld ()->GetCurrentLevel ();

   

	

    static int frame = 0;

    while (frame < 12)  // 12 кадров чтобы увидеть все изменения
        {        
        Tick ( 0.016f );
        frame++;
        }
	
    }

void CEngine::Tick ( float deltaTime )
	{
	CGameInstance::Get ().Tick ( deltaTime );
	CollisionSystem.Update ( deltaTime );
	}


void CEngine::CreateTestWorld ()
	{
	auto word = CGameInstance::Get ().CreateWorld (  "Super" );
	word->CreateLevel<CLevel> ( "New Level" );
	}

CEngine::CEngine () : CollisionSystem( COLLISION_SYSTEM )
	{
	
	};


