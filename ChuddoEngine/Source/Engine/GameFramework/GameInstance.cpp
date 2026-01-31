// GameInstance.cpp
#include "GameInstance.h"
#include "World/World.h"
#include <iostream>

// Статический член
CGameInstance * CGameInstance::Instance = nullptr;

CGameInstance::CGameInstance ( CObject * owner, const std::string & displayName )
    : Super ( owner, displayName )
    {
    std::cout << "[GAME] GameInstance created: " << displayName
        << " [UUID: " << GetShortUUID () << "]\n";
    }

CGameInstance::~CGameInstance ()
    {
    std::cout << "[GAME] GameInstance destroyed: " << GetName () << "\n";

    // Уничтожаем мир при уничтожении GameInstance
    if (CurrentWorld)
        {
            // World будет удален автоматически как дочерний объект
        CurrentWorld = nullptr;
        }

        // Сбрасываем singleton
    if (Instance == this)
        {
        Instance = nullptr;
        }
    }

    // ========== SINGLETON METHODS ==========

CGameInstance & CGameInstance::Get ()
    {
    if (!Instance)
        {
        std::cerr << "[ERROR] GameInstance not created! Call Create() first.\n";
        // Можно создать автоматически или assert
        Create ();
        }
    return *Instance;
    }

bool CGameInstance::Create ()
    {
    if (Instance)
        {
        std::cerr << "[WARNING] GameInstance already exists!\n";
        return false;
        }

        // Создаем как root объект (без владельца)
    Instance = new CGameInstance ( nullptr, "MainGameInstance" );
    return true;
    }

void CGameInstance::Destroy ()
    {
    if (Instance)
        {
        delete Instance;
        Instance = nullptr;
        std::cout << "[GAME] GameInstance destroyed\n";
        }
    }

    // ========== WORLD MANAGEMENT ==========

CWorld * CGameInstance::CreateWorld ( const std::string & worldName )
    {
    if (CurrentWorld)
        {
        std::cerr << "[GAME] World already exists! Destroy current world first.\n";
        return nullptr;
        }

        // Создаем World с правильным владельцем
    CurrentWorld = new CWorld ( this, worldName );  // this - CGameInstance*
    AddOwnedObject ( CurrentWorld );

  

    std::cout << "[GAME] World created: " << worldName << "\n";
    return CurrentWorld;
    }

void CGameInstance::DestroyWorld ()
    {
    if (CurrentWorld)
        {
            // Удаляем из дочерних объектов
        RemoveOwnedObject ( CurrentWorld->GetName () );
        CurrentWorld = nullptr;
        std::cout << "[GAME] World destroyed\n";
        }
    }

    // ========== GAME LIFECYCLE ==========

void CGameInstance::Init ()
    {
    std::cout << "[GAME] Initializing GameInstance...\n";
    GameTime = 0.0f;
    DeltaTime = 0.0f;

    CurrentWorld->BeginPlay ();
    }

void CGameInstance::Tick ( float deltaTime )
    {
    std::cout << "GameInstance tick with " << deltaTime << "\n";
    DeltaTime = deltaTime;
    GameTime += deltaTime;

    // Tick world если существует
    if (CurrentWorld)
        {
        CurrentWorld->Tick ( deltaTime );
        }

    }

void CGameInstance::Shutdown ()
    {
    std::cout << "[GAME] Shutting down GameInstance...\n";

    if (CurrentWorld)
        {
        DestroyWorld ();
        }
    }

void CGameInstance::DumpState () const
    {
    std::cout << "\n=== GAME INSTANCE STATE ===\n";
    std::cout << "Name: " << GetName () << "\n";
    std::cout << "UUID: " << GetShortUUID () << "\n";
    std::cout << "Game Time: " << GameTime << "s\n";
    std::cout << "Delta Time: " << DeltaTime << "s\n";
    std::cout << "Has World: " << ( CurrentWorld ? "Yes" : "No" ) << "\n";

    if (CurrentWorld)
        {
        std::cout << "World: " << CurrentWorld->GetName () << "\n";
        }

    std::cout << "Child Objects: " << GetNumOwnedObjects () << "\n";
    std::cout << "===========================\n";
    }
namespace
    {
    struct CGameInstanceRegistrar
        {
        CGameInstanceRegistrar ()
            {
            CObjectFactory::GetInstance ().RegisterClass<CGameInstance> ();
            }
        };
    static CGameInstanceRegistrar CGameInstance_AutoReg;
    }