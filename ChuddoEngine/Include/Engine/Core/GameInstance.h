#pragma once
// engine
#include "Core/Object.h"

//system
#include <memory>
#include <type_traits>
#include <iostream>
#include <cassert>

// Forward declarations (if any)
class CWorld;


class CGameInstance : public CObject
{
public:

	CGameInstance(const CGameInstance&) = delete;
	CGameInstance& operator=(const CGameInstance&) = delete;


	static CGameInstance& GetInstance();
	static CGameInstance* GetInstancePtr();

	virtual bool SaveConfig();

	static bool CreateInstance(const std::string& Name = "GameInstance");
	static void DestroyInstance();

	static std::string GetConfigPath() { return m_ConfigFilePath; }
	virtual bool Init();
	virtual void Shutdown();

	CWorld* GetWorld() const { return CurrentWorld; }
	void SetWorld(CWorld* NewWorld) { CurrentWorld = NewWorld; }

	bool IsInitialized() const { return bIsInitialized; }
	bool IsCreated() const { return bInstanceCreated; }

	void DefaultInitialization();
	static std::string LoadNameFromConfig();

	virtual bool LoadFromConfig();
	static  bool CheckConfigFile();
protected:
	static  std::string m_ConfigFilePath;
	explicit CGameInstance(const std::string& inName = "GameInstance");
	CGameInstance(const CObject* Owner, const std::string& inName);
	virtual ~CGameInstance();

private:
	static std::unique_ptr<CGameInstance> Instance;
	static bool bInstanceCreated;
	bool bIsInitialized = false;
	CWorld* CurrentWorld = nullptr;
	friend struct std::default_delete<CGameInstance>;
	static std::string Encript(const std::string& data, const std::string& key);
	static std::string MyKey;

};
const  int    MAX_PATH = 256;


template<typename T>
class TGameInstance : public CGameInstance
{
public:
	static_assert(std::is_base_of_v<CGameInstance, T>,
		"T must be derived from CGameInstance");

	static T& Get()
	{
		return static_cast<T&>(CGameInstance::GetInstance());
	}

	static T* GetPtr()
	{
		return static_cast<T*>(CGameInstance::GetInstancePtr());
	}

	template<typename... Args>
	static bool Create(Args&&... args)
	{

		if (CGameInstance::bInstanceCreated)
		{
			std::cerr << "Warning: GameInstance already created!" << std::endl;
			return false;
		}

		try
		{
			CGameInstance::Instance = std::unique_ptr<CGameInstance>(
				new T(std::forward<Args>(args)...));
			CGameInstance::bInstanceCreated = true;

			std::cout << "GameInstance of type '" << typeid(T).name()
				<< "' created with name: '"
				<< CGameInstance::Instance->GetName() << "'" << std::endl;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error creating GameInstance: " << e.what() << std::endl;
			return false;
		}
	}
};