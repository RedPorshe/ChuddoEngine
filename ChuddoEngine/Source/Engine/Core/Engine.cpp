#include "Core/Engine.h"
#include "GameFramework/GameInstance.h"
#include "World/World.h"
#include "World/Level.h"
#include "Actors/Actor.h"
#include "Components/BaseComponent.h"


class CTestCharacter : public CActor
	{
	CHUDDO_DECLARE_CLASS ( CTestCharacter, CActor );
	public:
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

REGISTER_CLASS_FACTORY ( CTestCharacter );




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

	CGameInstance::Get ().GetWorld ()->RemoveLevel ( "Empty Level" );
	MainLoop ();
	}

void testGetActors ()
	{
	LOG_WARN ( "============================================================================" );
	LOG_WARN ( " TEST func GetAllActorsOfClass  class to find CTestCharacter" );
	LOG_WARN ( "============================================================================" );
	auto level = CGameInstance::Get ().GetWorld ()->GetCurrentLevel ();

	auto foundedActors = level->GetAllActorsOfClass< CTestCharacter> ();
	if (foundedActors.empty ())
		{
		LOG_INFO ( "level has no actors by type : ", typeid( CTestCharacter ).name () );
		}
	for (auto act : foundedActors)
		{
		if (act)
			{
			LOG_INFO ( "Founded actor of type CTestCharacter ", act->GetName () );
			}
		}

	LOG_WARN ( "============================================================================" );
	LOG_WARN ( "============================================================================" );
	std::cout << "\n\n\n";
	LOG_WARN ( "============================================================================" );
	LOG_WARN ( " TEST func GetAllActorsOfClass  class to find CActor" );
	LOG_WARN ( "============================================================================" );

	auto foundedActors2 = level->GetAllActorsOfClass< CActor> ();
	if (foundedActors2.empty ())
		{
		LOG_INFO ( "level has no actors by type : ", typeid( CActor ).name () );
		}
	for (auto act : foundedActors2)
		{
		if (act)
			{
			LOG_INFO ( "Founded actor of type CActor ", act->GetName () );
			}
		}

	LOG_WARN ( "============================================================================" );
	LOG_WARN ( "============================================================================" );
	}

void TestMassSpawn ()
	{
	auto start = std::chrono::high_resolution_clock::now ();
	auto level = CGameInstance::Get ().GetWorld ()->GetCurrentLevel ();

	// Создаем все имена заранее
	std::vector<std::string> names;
	names.reserve ( 10 );

	for (int i = 0; i < 10; i++)
		{
		if (i == 0)
			names.push_back ( "Enemy" );
		else
			names.push_back ( "Enemy_" + std::to_string ( i ) );
		}

		// Спавним с готовыми именами
	for (const auto & name : names)
		{
		level->SpawnActor<CActor> ( name );
		}
	auto end = std::chrono::high_resolution_clock::now ();
	auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end - start );
	LOG_INFO ( "Mass spawn 10 actors took: ", duration.count (), "ms" );
	}

void TestMassDestoy ()
	{
	std::string Name = "Enemy";
	auto level = CGameInstance::Get ().GetWorld ()->GetCurrentLevel ();

	// Уничтожаем Enemy (без суффикса)
	level->DestroyActor ( Name );

	// Уничтожаем Enemy_1 .. Enemy_29
	for (int i = 1; i < 10; i++)
		{
		std::string finalName = Name + "_" + std::to_string ( i );
		level->DestroyActor ( finalName );
		}
	}


void CEngine::MainLoop ()
	{
	CGameInstance::Get ().Init ();
	auto level = CGameInstance::Get ().GetWorld ()->GetCurrentLevel ();

	LOG_WARN ( "level->SetMaxActorsPerTick (5);" );
	level->SetMaxActorsPerTick ( 5 );
	level->SpawnActor<CActor> ();
	auto player = level->SpawnActor<CActor> ( "Player" );
	auto cat = level->SpawnActor<CTestCharacter> ( "Cat" );
	auto LOHARA = level->SpawnActorByClass ( "CActor", "LOHARA" );
	auto LOH = level->SpawnActorByClass ( "CTestCharacter", "LOH" );	
	//testGetActors ();
	static int count = 0;
	while (count < 12)
		{
		LOG_INFO ( count, " frame" );
		if (count == 1)
			{
			//TestMassSpawn ();
			}
		if (count == 3)
			{
			auto actorToDestroy = level->FindObjectByName ( "Actor" );
			if (CActor * forDest = dynamic_cast< CActor * > ( actorToDestroy ))
				{
				LOG_DEBUG ( "test destoy actor in runtime" );
				level->DestroyActor ( forDest );
				}
			}
		if (count == 5)
			{
			LOG_INFO ( "Test Actor self destroy" );
			player->Destroy ();
			LOG_INFO ( "End test" );
			}
		if (count == 8)
			{ 
			auto at = level->SpawnActorAtLocation("CActor","ActorAtLocation",{50.f,45.f,454.f });
			level->DestroyActor ( at );
			}
		if (count == 9)
			{
			
			level->DestroyActor ( LOH );
			level->DestroyActor ( LOHARA );
			level->DestroyActor ( "Cat" );
			level->DestroyActor ( "Player" );
			level->DestroyActor ( "Actor" );
			}

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
	word->CreateLevel<CLevel> ( "New Level" );
	}


