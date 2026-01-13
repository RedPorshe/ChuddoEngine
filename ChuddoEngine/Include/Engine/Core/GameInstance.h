#pragma once
#include "Core/Object.h"
#include <memory>
#include <type_traits>
#include <iostream>
#include <cassert>

class CGameInstance : public CObject
{
public:
    
    CGameInstance(const CGameInstance&) = delete;
    CGameInstance& operator=(const CGameInstance&) = delete;

    
    static CGameInstance& GetInstance();
    static CGameInstance* GetInstancePtr();

    
    static bool CreateInstance(const std::string& Name = "GameInstance");
    static void DestroyInstance();

    
    virtual bool Init();
    virtual void Shutdown();

    
    bool IsInitialized() const { return bIsInitialized; }
    bool IsCreated() const { return bInstanceCreated; }

	void Tick(float DeltaTime);

protected:
    
    explicit CGameInstance(const std::string& inName = "GameInstance");
    CGameInstance(const CObject* Owner, const std::string& inName);
    virtual ~CGameInstance();

private:
    static std::unique_ptr<CGameInstance> Instance;
    static bool bInstanceCreated;
    bool bIsInitialized = false;

    friend struct std::default_delete<CGameInstance>;
};


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
        if (bInstanceCreated)
        {
            std::cerr << "Warning: GameInstance already created!" << std::endl;
            return false;
        }

        Instance = std::unique_ptr<CGameInstance>(new T(std::forward<Args>(args)...));
        bInstanceCreated = true;

        std::cout << "GameInstance of type '" << typeid(T).name()
            << "' created with name: '" << Instance->GetName() << "'" << std::endl;
        return true;
    }
};