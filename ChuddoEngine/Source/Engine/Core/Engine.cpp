#include "Core/Engine.h"
#include "GameFramework/GameInstance.h"
#include "World/World.h"
#include "World/Level.h"
#include "Actors/Actor.h"
#include "Components/BaseComponent.h"


class CTestCharacter : public CActor
	{
	CHUDDO_DECLARE_CLASS ( CTestCharacter, CActor );
	CTestCharacter ( CObject * owner = nullptr, const std::string & inName = "Actor" ) :Super ( owner, inName )
		{
		CameraComponent = AddDefaultSubObject<CBaseComponent> ( "CameraComp" );
		mesh = AddDefaultSubObject<CSceneComponent> ( "mesh" );
		}
	virtual void BeginPlay () override { Super::BeginPlay (); }
	virtual void Tick ( float DeltaTime ) override { Super::Tick ( DeltaTime ); }
	virtual void EndPlay () override { Super::EndPlay (); }
	CBaseComponent * CameraComponent = nullptr;
	CSceneComponent * mesh = nullptr;
	};

void MassSpawnTest ( int countForSpawn )
	{
	LOG_INFO( "====================================================");
	LOG_INFO( " TEST MASS SPAWN IN BEGIN PLAY" );
	LOG_INFO( "====================================================");
	
	auto world = CGameInstance::Get ().GetWorld ();
	auto level = world->GetCurrentLevel ();
	int countofMassSpawn = countForSpawn;
	LOG_INFO( "Current count of mass spawn = " , countofMassSpawn , "'");
	if (level)
		{
		for (int i {}; i < countofMassSpawn; i++)
			{
			level->SpawnActor<CTestCharacter> ( "MassSpawnCharacter" );
			}
		}
	LOG_INFO( "====================================================");
	LOG_INFO( " END TEST MASS SPAWN IN BEGIN PLAY" );
	LOG_INFO( "====================================================");	
	}



CEngine * CEngine::Instance = nullptr;

CEngine::~CEngine ()
	{
	LOG_INFO( "Engine destroyed");

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
		LOG_ERROR( "Engine already initialized!");
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
		LOG_WARN( "Engine already initialized");
		return true;
		}

	if (!CGameInstance::Create ())
		{
		LOG_FATAL( "Failed to create GameInstance");
		return false;
		}

	bIsInitialized = true;
	LOG_INFO( "Engine initialized");
	return true;
	}

void CEngine::Shutdown ()
	{
	if (!bIsInitialized)
		return;

	LOG_INFO( "Engine shutting down...");
	// сохраняем состояние если нужно
	if (CGameInstance::Get ().IsMustSaveState ())
		{
		LOG_INFO( "saving gameInstance state");
		CGameInstance::Get ().SaveGameInstanceState ();
		}
	// Уничтожаем GameInstance
	CGameInstance::Destroy ();

	bIsInitialized = false;
	bIsRunning = false;

	LOG_DEBUG( "Engine shutdown complete");
	}

void CEngine::Start ()
	{
	CreateTestWorld ();
	
	CGameInstance::Get ().GetWorld ()->RemoveLevel ( "Empty Level" );
	MainLoop ();
	}

void CEngine::MainLoop ()
	{
	CGameInstance::Get ().Init ();
	MassSpawnTest ( 5 );
	static int count = 0;
	while (count < 5)
		{
		LOG_INFO(  count , " frame");

		Tick ( 0.016f );
		count++;
		}
	}

void CEngine::Tick ( float deltaTime )
	{
	CGameInstance::Get ().Tick ( deltaTime );


	}
#include "tests.h"

void CEngine::CreateTestWorld ()
	{
	auto word = CGameInstance::Get ().CreateWorld ( "testworld" );
	auto level = word->CreateDefaultEmptyLevel ();
	auto level2 = word->CreateLevel<CLevel> ( "New Level" );
	auto level3 = word->CreateLevel<CLevel> ( " newNExtLevel" );
	}


