#include "Core/GameInstance.h"


std::unique_ptr<CGameInstance> CGameInstance::Instance = nullptr;
bool CGameInstance::bInstanceCreated = false;



CGameInstance::CGameInstance(const std::string& inName)
    : CObject(inName)
{
    
}

CGameInstance::CGameInstance(const CObject* Owner, const std::string& inName)
    : CObject(Owner, inName)
{
   
}

CGameInstance::~CGameInstance()
{
  
}

CGameInstance& CGameInstance::GetInstance()
{
    if (!Instance)
    {
        std::cerr << "Fatal Error: GameInstance not created! "
            << "Call CreateInstance() before GetInstance()." << std::endl;
        std::terminate();
    }
    return *Instance;
}

CGameInstance* CGameInstance::GetInstancePtr()
{
    return Instance.get();
}

bool CGameInstance::CreateInstance(const std::string& Name)
{
    if (bInstanceCreated)
    {
        std::cerr << "Warning: GameInstance already created! "
            << "Ignoring duplicate creation." << std::endl;
        return false;
    }

    try
    {
        Instance = std::unique_ptr<CGameInstance>(new CGameInstance(Name));
        bInstanceCreated = true;

        
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error creating GameInstance: " << e.what() << std::endl;
        return false;
    }
    catch (...)
    {
        std::cerr << "Unknown error creating GameInstance!" << std::endl;
        return false;
    }
}

void CGameInstance::DestroyInstance()
{
    if (!Instance)
    {
        std::cout << "GameInstance already destroyed or never created." << std::endl;
        return;
    }
         
    if (Instance->bIsInitialized)
    {
        Instance->Shutdown();
    }

    
    Instance.reset();
    bInstanceCreated = false;

    
}

bool CGameInstance::Init()
{
    if (bIsInitialized)
    {
        std::cout << "GameInstance '" << GetName() << "' already initialized." << std::endl;
        return true;
    }

    std::cout << "Initializing GameInstance '" << GetName() << "'..." << std::endl;

    try
    {
        // Здесь должна быть ваша логика инициализации:
        // 1. Загрузка конфигурации
        // 2. Инициализация подсистем
        // 3. Создание менеджеров ресурсов
        // 4. И т.д.

        bIsInitialized = true;
        std::cout << "GameInstance '" << GetName() << "' initialized successfully." << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to initialize GameInstance: " << e.what() << std::endl;
        bIsInitialized = false;
        return false;
    }
}

void CGameInstance::Shutdown()
{
    if (!bIsInitialized)
    {
        std::cout << "GameInstance '" << GetName() << "' not initialized, nothing to shutdown." << std::endl;
        return;
    }

    std::cout << "Shutting down GameInstance '" << GetName() << "'..." << std::endl;

    try
    {
        // Здесь должна быть ваша логика завершения:
        // 1. Сохранение состояния
        // 2. Очистка ресурсов
        // 3. Уничтожение подсистем
        // 4. И т.д.

        bIsInitialized = false;
        std::cout << "GameInstance '" << GetName() << "' shutdown complete." << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error during GameInstance shutdown: " << e.what() << std::endl;
    }
}