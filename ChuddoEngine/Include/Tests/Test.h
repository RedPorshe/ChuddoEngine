#pragma once

#include "Core/World.h"
#include "Core/Level.h"
#include "Actors/Actor.h"

// Example: Custom Actor class derived from CActor
class CPlayerActor : public CActor
{
	public:
	CPlayerActor(const CObject* Owner = nullptr, const std::string& inName = "Player")
		: CActor(Owner, inName)
	{
		std::cout << "CPlayerActor '" << inName << "' created." << std::endl;
	}

	virtual void BeginPlay() override
	{
		std::cout << "CPlayerActor '" << GetName() << "' BeginPlay." << std::endl;
		CActor::BeginPlay();
	}

	virtual void Tick(float DeltaTime) override
	{
		std::cout << "CPlayerActor '" << GetName() << "' Tick: " << DeltaTime << std::endl;
		CActor::Tick(DeltaTime);
	}
};

// Example: Enemy Actor class
class CEnemyActor : public CActor
{
	public:
	CEnemyActor(const CObject* Owner = nullptr, const std::string& inName = "Enemy")
		: CActor(Owner, inName)
	{
		std::cout << "CEnemyActor '" << inName << "' created." << std::endl;
	}

	virtual void BeginPlay() override
	{
		std::cout << "CEnemyActor '" << GetName() << "' BeginPlay." << std::endl;
		CActor::BeginPlay();
	}

	virtual void Tick(float DeltaTime) override
	{
		std::cout << "CEnemyActor '" << GetName() << "' Tick: " << DeltaTime << std::endl;
		CActor::Tick(DeltaTime);
	}
};

class TestWorld : public CWorld
{
	public:
	TestWorld(const CObject* Owner, const std::string& inName = "TestWorld")
		: CWorld(Owner, inName)
	{
		std::cout << "TestWorld '" << GetName() << "' created." << std::endl;
		LevelsForLoad = { "TestLevel1", "TestLevel2", "TestLevel3" };
		for (auto& LevelName : LevelsForLoad)
		{
			LoadLevel(LevelName);
		}
		// Level-specific initialization can be added here
		std::cout << "Level '" << GetName() << "' created." << std::endl;
		// For example, loading level data from a file or database
		std::cout << "Loading level data for '" << GetName() << "'..." << std::endl;
		
		CLevel* PersistentLevel = GetPersistentLevel();

		// ============ ПРИМЕРЫ ИСПОЛЬЗОВАНИЯ НОВЫХ ШАБЛОННЫХ ФУНКЦИЙ ============

		// Пример 1: SpawnActor с параметрами по умолчанию
		std::cout << "\n=== Example 1: SpawnActor with default parameters ===" << std::endl;
		CPlayerActor* Player = PersistentLevel->SpawnActor<CPlayerActor>();

		// Пример 2: SpawnActorAt с трансформацией (название по умолчанию)
		std::cout << "\n=== Example 2: SpawnActorAt with transform ===" << std::endl;
		FTransform EnemyTransform1(FVector(100, 0, 0), FQuat(0, 1, 5), FVector(1, 1, 1));
		CEnemyActor* Enemy1 = PersistentLevel->SpawnActorAt<CEnemyActor>(EnemyTransform1);

		// Пример 3: SpawnActor с именем и трансформацией
		std::cout << "\n=== Example 3: SpawnActor with name and transform ===" << std::endl;
		FTransform EnemyTransform2(FVector(-100, 0, 0), FQuat(0, 5, 5), FVector(1, 1, 1));
		CEnemyActor* Enemy2 = PersistentLevel->SpawnActor<CEnemyActor>("EliteEnemy", EnemyTransform2);

	}
	virtual ~TestWorld()
	{
		
		
	}
	virtual void Tick(float DeltaTime) override
	{
		std::cout << "Tick on TestWorld: " << GetName() << " with DeltaTime: " << DeltaTime << std::endl;
		CWorld::Tick(DeltaTime); // Call base class Tick
	}
	virtual void BeginPlay() override
	{
		std::cout << "Begin Play on TestWorld: " << GetName() << std::endl;
		CWorld::BeginPlay(); // Call base class BeginPlay
	}
	private:
	std::vector<std::string> LevelsForLoad;
};