#include "Core/EngineLoop.h"
#include <chrono>
#include "Core/GameInstance.h"
#include "Core/World.h"



FEngineLoop::FEngineLoop()
{
	bIsRunning = false;
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

	
	if (!CGameInstance::CreateInstance("MainGameInstance"))
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
	if (GameInstance.IsCreated())
	{
		// Create TestWorld as unique_ptr and transfer ownership to GameInstance.
		// Construct with no owner (nullptr) so AddOwnedObject will set proper OwnerObject.
		auto worldUP = std::make_unique<CWorld>(nullptr, "MainTestWorld");
		CWorld* gameworld = static_cast<CWorld*>(GameInstance.AddOwnedObject(std::move(worldUP)));
		GameInstance.SetWorld(gameworld);
	}
	else
	{
		throw std::runtime_error("GameInstance not created!");
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

		
		CGameInstance::DestroyInstance();

		std::cout << "Engine Shutdown" << std::endl;
	}
}

void FEngineLoop::MainLoop()
{
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
	(void)DT;
	auto GI = CGameInstance::GetInstancePtr();
	auto world = GI->GetWorld();
	world->Tick(DT);
	static int TickCount = 0;
	TickCount++;
	if (TickCount >= 10) 
	{
		GIsRequestingExit = true;
		std::cout << "Tick in Stub mode:  " << TickCount << " ticks" << std::endl;
	}
}

