#pragma once
#include <CoreMinimal.h>
#include <chrono>





class FEngineLoop
{
	public:
	FEngineLoop();
	~FEngineLoop();
	// Инициализация движка
	bool Init(/*need add cmd line*/);
	void Start();
	// Завершение работы движка
private:
	void Shutdown();
	void MainLoop();
	void CalculateDeltaTime();
	// Тик движка
	void Tick(float DT);
	bool bIsRunning;
	// Дополнительные члены класса
	float DeltaTime;
	
	std::chrono::steady_clock::time_point LastFrameTime{};


	
};

extern FEngineLoop GEngine;

extern bool GIsRequestingExit;


