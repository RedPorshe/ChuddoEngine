#pragma once
#include "CoreMinimal.h"

#include "Core/Object.h"
#include "Core/GameInstance.h"
// Forward declarations

class CLevel;

class CWorld : public CObject
{
public:
	CWorld(const CObject* Owner, const std::string& inName = "World");	
	virtual ~CWorld();
	// World-specific methods can be added here
	virtual void Tick(float DeltaTime);
	virtual void BeginPlay();
	// GameInstance management
	CGameInstance* GetGameInstance() const { return CGameInstance::GetInstancePtr(); }
	void LoadLevel(const std::string& LevelName);
	void SetCurrentLevel(CLevel* NewLevel);
	void UnloadLevel(const std::string& LevelName);
	void SetPersistentLevel(CLevel* NewLevel) { PersistentLevel = NewLevel; }
	CLevel* GetCurrentLevel() const { return CurrentLevel; }
	CLevel* GetPersistentLevel() const { return PersistentLevel; }
private:
	CGameInstance* GameInstance = nullptr;
	CLevel* PersistentLevel = nullptr;
	CLevel* CurrentLevel = nullptr;
	std::vector<CLevel*> LoadedLevels;
};
