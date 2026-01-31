// Engine includes
#include "World/World.h"
#include "GameFramework/GameInstance.h"
#include "GameFramework/World/Level.h"

//system includes
#include <iostream>
#include <algorithm>

CWorld::CWorld ( CObject * inOwner, const std::string & displayName )
    : Super ( inOwner, displayName )
    {
        // Получаем GameInstance из владельца
    OwningGameInstance = dynamic_cast< CGameInstance * >( inOwner );

    if (OwningGameInstance)
        {
        std::cout << "[WORLD] World created: " << displayName
            << " [Owner: " << OwningGameInstance->GetName () << "]\n";
        }
    else if (inOwner)
        {
        std::cout << "[WORLD] World created: " << displayName
            << " [Owner: " << inOwner->GetObjectClassName () << "]\n";
        }
    else
        {
        std::cout << "[WORLD] World created: " << displayName << " [No owner]\n";
        }
    }

CWorld::~CWorld ()
    {
    std::cout << "[WORLD] World destroyed: " << GetName () << "\n";

    // Завершаем игру если запущена
    if (bIsPlaying)
        {
        EndPlay ();
        }

        // Очищаем уровни
    Levels.clear ();
    CurrentLevel = nullptr;
    OwningGameInstance = nullptr;
    }

    // ========== LEVEL MANAGEMENT ==========

CWorld * CWorld::GetWorld ()
    {
    return CGameInstance::Get().GetWorld();
    }

CLevel * CWorld::CreateLevel ( const std::string & levelName )
    {
        // Создаем уровень с этим миром как владельцем
    auto level = std::make_unique<CLevel> ( this, levelName );
    CLevel * rawPtr = level.get ();

    Levels.push_back ( std::move ( level ) );

    // Если это первый уровень, делаем его текущим
    if (Levels.size () == 1)
        {
        SetCurrentLevel ( rawPtr );
        }

    std::cout << "[WORLD] Level created: " << levelName
        << " (Total levels: " << Levels.size () << ")\n";

    return rawPtr;
    }

void CWorld::AddLevel ( std::unique_ptr<CLevel> level )
    {
    if (!level)
        return;

    // Устанавливаем этот мир как владельца уровня
    level->OwningWorld = this;
    CLevel * rawPtr = level.get ();

    Levels.push_back ( std::move ( level ) );

    std::cout << "[WORLD] Level added: " << rawPtr->GetName ()
        << " (Total levels: " << Levels.size () << ")\n";
    }

bool CWorld::RemoveLevel ( const std::string & levelName )
    {
    auto it = std::find_if ( Levels.begin (), Levels.end (),
                             [ &levelName ] ( const std::unique_ptr<CLevel> & level )
                             {
                             return level && level->GetName () == levelName;
                             } );

    if (it != Levels.end ())
        {
            // Если удаляем текущий уровень, сбрасываем текущий
        if (CurrentLevel == it->get ())
            {
            CurrentLevel = nullptr;
            }

        std::cout << "[WORLD] Level removed: " << ( *it )->GetName () << "\n";
        Levels.erase ( it );
        return true;
        }

    std::cout << "[WORLD] Level not found: " << levelName << "\n";
    return false;
    }

bool CWorld::RemoveLevel ( CLevel * level )
    {
    if (!level)
        return false;

    return RemoveLevel ( level->GetName () );
    }

void CWorld::SetCurrentLevel ( CLevel * level )
    {
        // Проверяем, что уровень принадлежит этому миру
    bool belongsToWorld = false;
    for (const auto & lvl : Levels)
        {
        if (lvl.get () == level)
            {
            belongsToWorld = true;
            break;
            }
        }

    if (!belongsToWorld && level != nullptr)
        {
        std::cerr << "[WORLD] ERROR: Level '" << ( level ? level->GetName () : "null" )
            << "' does not belong to this world!\n";
        return;
        }

    CurrentLevel = level;
    std::cout << "[WORLD] Current level set to: "
        << ( CurrentLevel ? CurrentLevel->GetName () : "None" ) << "\n";
    }

    // ========== WORLD LIFECYCLE ==========

void CWorld::BeginPlay ()
    {
    if (bIsPlaying)
        {
        std::cerr << "[WORLD] ERROR: World is already playing!\n";
        return;
        }

    bIsPlaying = true;
    std::cout << "[WORLD] BeginPlay: " << GetName () << "\n";

    // Запускаем все уровни
    for (auto & level : Levels)
        {
        level->BeginPlay ();
        }
    }

void CWorld::Tick ( float deltaTime )
    {
    if (!bIsPlaying)
        {
        std::cout << "[WORLD] World is not playing, skipping tick\n";
        return;
        }

    std::cout << "[WORLD] Tick: " << GetName () << " (delta: " << deltaTime << ")\n";

    // Обновляем текущий уровень
    if (CurrentLevel)
        {
        CurrentLevel->Tick ( deltaTime );
        }
    else
        {
        std::cout << "[WORLD] No current level to tick\n";
        }
    }

void CWorld::EndPlay ()
    {
    if (!bIsPlaying)
        return;

    bIsPlaying = false;
    std::cout << "[WORLD] EndPlay: " << GetName () << "\n";

    // Завершаем все уровни
    for (auto & level : Levels)
        {
        level->EndPlay ();
        }
    }

    // ========== SEARCH/QUERY ==========

CObject * CWorld::FindObjectByName ( const std::string & name ) const
    {
        // Ищем в уровнях
    for (const auto & level : Levels)
        {
        CObject * found = level->FindObjectByName ( name );
        if (found)
            return found;
        }

    return nullptr;
    }

CObject * CWorld::FindObjectByUUID ( const std::string & uuid ) const
    {
        // Ищем в уровнях
    for (const auto & level : Levels)
        {
        CObject * found = level->FindObjectByUUID ( uuid );
        if (found)
            return found;
        }

    return nullptr;
    }

template<typename T>
T * CWorld::FindObjectOfType () const
    {
    for (const auto & level : Levels)
        {
            // Реализуем когда будет система компонентов
            // T* found = level->FindObjectOfType<T>();
            // if (found) return found;
        }

    return nullptr;
    }

    // ========== DEBUG/UTILS ==========

void CWorld::DumpState () const
    {
    std::cout << "\n=== WORLD STATE ===\n";
    std::cout << "Name: " << GetName () << "\n";
    std::cout << "UUID: " << GetShortUUID () << "\n";
    std::cout << "GameInstance: "
        << ( OwningGameInstance ? OwningGameInstance->GetName () : "None" ) << "\n";
    std::cout << "Is Playing: " << ( bIsPlaying ? "Yes" : "No" ) << "\n";
    std::cout << "Current Level: "
        << ( CurrentLevel ? CurrentLevel->GetName () : "None" ) << "\n";
    std::cout << "Total Levels: " << Levels.size () << "\n";

    for (size_t i = 0; i < Levels.size (); ++i)
        {
        std::cout << "  [" << i << "] " << Levels[ i ]->GetName ()
            << " (Active: " << ( Levels[ i ].get () == CurrentLevel ? "Yes" : "No" ) << ")\n";
        }

    std::cout << "===================\n";
    }

    // ========== FACTORY REGISTRATION ==========

namespace
    {
    struct CWorldRegistrar
        {
        CWorldRegistrar ()
            {
            CObjectFactory::GetInstance ().RegisterClass<CWorld> ();
            }
        };
    static CWorldRegistrar CWorld_AutoReg;
    }