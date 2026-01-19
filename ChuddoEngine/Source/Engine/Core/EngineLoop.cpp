#include "Core/EngineLoop.h"
#include <chrono>
#include "Core/GameInstance.h"
#include "Core/World.h"




FEngineLoop::FEngineLoop()
{
	bIsRunning = false;
	bIsInitialized = false;
	DeltaTime = 0.0f;
	LastFrameTime = std::chrono::steady_clock::now();
}

FEngineLoop::~FEngineLoop()
{
}

bool FEngineLoop::Init()
{
	std::cout << "Engine Initialized" << std::endl;
	return true;
}

void FEngineLoop::Start()
{
	std::string configName = CGameInstance::LoadNameFromConfig();

	if (configName.empty())
	{
		// Если не удалось загрузить имя из конфига
		std::cout << "starting Engine in stub file config not found\n";
		configName = "GameInstance"; // Имя по умолчанию
	}
	else
	{
		std::cout << "starting Engine from file with GameInstance name is : " << configName << "\n";
	}

	// Создаем GameInstance с именем из конфига или по умолчанию
	if (!CGameInstance::CreateInstance(configName))
	{
		std::cerr << "Failed to create GameInstance!" << std::endl;
		return;
	}

	auto& GameInstance = CGameInstance::GetInstance();

	if (!GameInstance.Init())
	{
		std::cerr << "Failed to initialize GameInstance!" << std::endl;
		CGameInstance::DestroyInstance();
		return;
	}

	std::cout << "Engine Started" << std::endl;
	bIsRunning = true;

	MainLoop();
}

void FEngineLoop::Shutdown()
{
	if (bIsRunning)
	{
		bIsRunning = false;
		//shutdown render
		//gameinstance last
		if (auto GI = CGameInstance::GetInstancePtr())
		{
			std::cout << "saving in Engine\n";
			GI->SaveConfig();
		}
		CGameInstance::DestroyInstance();

		std::cout << "Engine Shutdown" << std::endl;
	}
}

void FEngineLoop::MainLoop()
{
	LastFrameTime = std::chrono::steady_clock::now();
	std::cout << "Entering Main Loop" << std::endl;
	auto& GameInstance = CGameInstance::GetInstance();
	if (auto GameWorld = GameInstance.GetWorld())
	{
		GameWorld->BeginPlay();
	}
	while (!GIsRequestingExit && bIsRunning)
	{

		CalculateDeltaTime();
		Tick(DeltaTime);
	}
	std::cout << "Exiting Main Loop" << std::endl;	
	Shutdown();
}

void FEngineLoop::CalculateDeltaTime()
{
	auto CurrentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> ElapsedTime = CurrentTime - LastFrameTime;
	DeltaTime = ElapsedTime.count();

	LastFrameTime = CurrentTime;
}



void FEngineLoop::Tick(float DT)
{
	if (auto GI = CGameInstance::GetInstancePtr())
	{
		if (auto world = GI->GetWorld())
		{
			world->Tick(DT);

			static int TickCount = 0;
			TickCount++;

			if (TickCount >= 10)
			{
				GIsRequestingExit = true;
				std::cout << "Tick in Stub mode: " << TickCount << " ticks" << std::endl;
			}
		}
		else
		{			
			static int NoWorldWarningCount = 0;
			static float LastWarningTime = 0.0f;

			if (DT > 0) 
			{
				LastWarningTime += DT;
				if (LastWarningTime >= 1.0f) 
				{
					NoWorldWarningCount++;
					std::cout << "WARNING: No World loaded ("
						<< NoWorldWarningCount << " warnings)" << std::endl;
					LastWarningTime = 0.0f;
					if (NoWorldWarningCount == 10) GIsRequestingExit = true;
				}
			}
		}
	}
	else
	{	
		std::cerr << "FATAL ERROR: GameInstance is null during Tick!" << std::endl;
		GIsRequestingExit = true;
	}
}

