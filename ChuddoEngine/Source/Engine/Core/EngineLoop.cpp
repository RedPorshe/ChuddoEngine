#include "Core/EngineLoop.h"
#include <chrono>
#include "Core/GameInstance.h"

#include "Tests/Test.h"


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
	GI->Tick(DT);
	static int TickCount = 0;
	TickCount++;
	if (TickCount >= 10) 
	{
		GIsRequestingExit = true;
		std::cout << "Tick in Stub mode:  " << TickCount << " ticks" << std::endl;
	}
}

