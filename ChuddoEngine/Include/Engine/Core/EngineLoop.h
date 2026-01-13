#pragma once
#include <CoreMinimal.h>
#include <chrono>





class FEngineLoop
{
	public:
	FEngineLoop();
	~FEngineLoop();
	
	bool Init(/*need add cmd line*/);
	void Start();
	
private:
	void Shutdown();
	void MainLoop();
	void CalculateDeltaTime();
	
	void Tick(float DT);
	bool bIsRunning;
	
	float DeltaTime;
	
	std::chrono::steady_clock::time_point LastFrameTime{};


	
};

extern FEngineLoop GEngine;

extern bool GIsRequestingExit;


