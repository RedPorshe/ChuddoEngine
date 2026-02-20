#include "Core/Engine.h"
#include "Core/InputSystem.h"
#include "GameFramework/GameInstance.h"
#include "GameFramework/World/World.h"
#include "GameFramework/World/Level.h"

#include "GameFramework/Actors/Actor.h"
#include "GameFramework/GameMode.h"
#include "Components/Collisions/BaseCollisionComponent.h"

#include "Core/CollisionSystem.h"

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
	COLLISION_SYSTEM.Shutdown ();

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

		
	//LOG_DEBUG ( "[ENGINE] Initializing InputSystem..." );
	//if (!INPUT_SYSTEM->Initialize ( static_cast< GLFWwindow * >( Window ) ))
	//	{
	//	LOG_FATAL ( "Failed to initialize InputSystem" );
	//	ShutdownEngine ();
	//	return false;
	//	}

		// Создаём GameInstance
	if (!CGameInstance::Create ())
		{
		LOG_FATAL ( "Failed to create GameInstance" );
		ShutdownEngine ();
		return false;
		}

		// Инициализируем CollisionSystem
	COLLISION_SYSTEM;

	bIsInitialized = true;
	LOG_INFO ( "Engine initialized successfully" );
	return true;
	}

void CEngine::Shutdown ()
	{
	if (!bIsInitialized)
		return;

	LOG_INFO ( "Engine shutting down..." );

	auto & GameInstance = CGameInstance::Get ();

	if (GameInstance.IsMustSaveState ())
		{
		LOG_INFO ( "saving gameInstance state" );
		GameInstance.SaveGameInstanceState ();
		}

	GameInstance.Shutdown ();
	CGameInstance::Destroy ();
	  
	if (INPUT_SYSTEM)
		{
		INPUT_SYSTEM->ShutdownSystem ();
		}

		
	
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
	bIsRunning = true;

	// Засекаем реальное время
	auto realStartTime = std::chrono::steady_clock::now ();
	float gameTime = 0.0f;

	while (bIsRunning)
		{
			// Проверяем, не запрошено ли закрытие окна
	

		CalculateDeltaTime ();
		Tick ( m_DeltaTime );

		gameTime += m_DeltaTime;

		// Для теста - выход через 10 секунд
		if (gameTime > 10.0f)
			{
			LOG_DEBUG ( "[ENGINE] Auto exit after 10 game seconds" );
			RequestExit ();
			}
		}
	}

void CEngine::Tick ( float deltaTime )
	{
	// Обновляем InputSystem
	INPUT_SYSTEM->Update ( deltaTime );

	// Обрабатываем ввод для PlayerController (если есть)
	// TODO: Получить текущего PlayerController и обработать его ввод
	// INPUT_SYSTEM->ProcessControllerInput(PlayerController, deltaTime);

	// Обновляем GameInstance
	CGameInstance::Get ().Tick ( deltaTime );

	// Обновляем CollisionSystem
	CollisionSystem.Update ( deltaTime );

	// Здесь будет рендер
	// Renderer->Render();

	
	}

void CEngine::CalculateDeltaTime ()
	{
	auto currentTime = std::chrono::steady_clock::now ();

	if (m_LastFrameTime.time_since_epoch ().count () != 0)
		{
			// Явно указываем типы
		auto delta = std::chrono::duration_cast< std::chrono::microseconds >(
			currentTime - m_LastFrameTime
		).count ();

		
		m_DeltaTime = static_cast< float >( delta ) / 1000000.0f;

		
		constexpr float MAX_DELTA = 1.0f / 10.0f;  
		constexpr float MIN_DELTA = 1.0f / 244.0f; // 244 FPS максимум

		m_DeltaTime = std::clamp ( m_DeltaTime, MIN_DELTA, MAX_DELTA );

		}
	else
		{
			// Первый кадр
		m_DeltaTime = 1.0f / 60.0f;
		}

	m_LastFrameTime = currentTime;
	}

void CEngine::CreateTestWorld ()
	{
	auto world = CGameInstance::Get ().CreateWorld ( "Super" );
	if (world)
		{
		world->CreateLevel<CLevel> ( "SuperLevel" );
		LOG_DEBUG ( "[ENGINE] Test world created: Super with level: SuperLevel" );
		}
	}

CEngine::CEngine () :
	CollisionSystem ( COLLISION_SYSTEM ),
	InputSystem ( *INPUT_SYSTEM )
	
	{
	}