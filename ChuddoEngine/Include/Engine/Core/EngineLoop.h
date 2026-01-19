#pragma once
//engine includes
#include "CoreMinimal.h"


//system
#include <chrono>



class FEngineLoop
{
public:
	FEngineLoop();
	~FEngineLoop();

	bool Init(/*need add cmd line*/);
	void Start();

private:
	//functions
	void Shutdown();
	void MainLoop();
	void CalculateDeltaTime();	
	void Tick(float DT);
	//variables
	bool bIsRunning;
	bool bIsInitialized;
	float DeltaTime;

	std::chrono::steady_clock::time_point LastFrameTime{};
};

extern FEngineLoop GEngine;

extern bool GIsRequestingExit;
