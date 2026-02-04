// Engine includes
#include "World/Level.h"
#include "World/World.h"
#include "Actors/Actor.h"

//system includes
#include <iostream>
#include <algorithm>

CLevel::CLevel ( CObject * owner, const std::string & inName )
    : CObject ( owner, inName )
    {
        // Получаем World из владельца
    OwningWorld = dynamic_cast< CWorld * >( owner );

    if (OwningWorld)
        {
        std::cout << "[LEVEL] Level created: " << inName
            << " [World: " << OwningWorld->GetName () << "]\n";
        }
    else
        {
        std::cout << "[LEVEL] Level created: " << inName << " [No world]\n";
        }
    }

CLevel::~CLevel ()
    {
    std::cout << "[LEVEL] Level destroyed: " << GetName () << "\n";

    if (bIsPlaying)
        {
        EndPlay ();
        }

        // Очищаем акторов
    Actors.clear ();
    }

void CLevel::BeginPlay ()
    {
    if (bIsPlaying)
        {
        std::cerr << "[LEVEL] ERROR: Level is already playing!\n";
        return;
        }

    bIsPlaying = true;
    std::cout << "[LEVEL] BeginPlay '" << GetName () << "'\n";

    // Запускаем всех акторов
    for (auto & actor : Actors)
        {
        actor->BeginPlay ();
        }
    }

void CLevel::Tick ( float DeltaTime )
    {
    if (!bIsPlaying)
        {
        std::cout << "[LEVEL] Level is not playing, skipping tick\n";
        return;
        }

    std::cout << "[LEVEL] " << GetName () << " is ticking with " << DeltaTime << " ms\n";

    // Обновляем всех акторов
    for (auto & actor : Actors)
        {
        actor->Tick ( DeltaTime );
        }
    }

void CLevel::EndPlay ()
    {
    if (!bIsPlaying)
        return;

    bIsPlaying = false;
    std::cout << "[LEVEL] End Play for '" << GetName () << "'\n";

    // Завершаем всех акторов
    for (auto & actor : Actors)
        {
        actor->EndPlay ();
        }
    }

bool CLevel::DestroyActor ( CActor * actor )
    {
    if (!actor)
        return false;

    auto it = std::find_if ( Actors.begin (), Actors.end (),
                             [ actor ] ( const std::unique_ptr<CActor> & a )
                             {
                             return a.get () == actor;
                             } );

    if (it != Actors.end ())
        {
            // Завершаем актора если уровень играет
        if (bIsPlaying)
            {
            ( *it )->EndPlay ();
            }

        std::cout << "[LEVEL] Actor destroyed: " << ( *it )->GetName () << "\n";
        Actors.erase ( it );
        return true;
        }

    return false;
    }

bool CLevel::DestroyActor ( const std::string & actorName )
    {
    auto it = std::find_if ( Actors.begin (), Actors.end (),
                             [ &actorName ] ( const std::unique_ptr<CActor> & actor )
                             {
                             return actor && actor->GetName () == actorName;
                             } );

    if (it != Actors.end ())
        {
        return DestroyActor ( it->get () );
        }

    return false;
    }

CObject * CLevel::FindObjectByName ( const std::string & name ) const
    {
        // Проверяем себя
    if (GetName () == name)
        return const_cast< CLevel * >( this );

    // Ищем в акторах
    for (const auto & actor : Actors)
        {
        if (actor->GetName () == name)
            return actor.get ();

        // Можно добавить рекурсивный поиск в компонентах актора позже
        }

    return nullptr;
    }

CObject * CLevel::FindObjectByUUID ( const std::string & uuid ) const
    {
        // Проверяем себя
    if (GetUUID () == uuid)
        return const_cast< CLevel * >( this );

    // Ищем в акторах
    for (const auto & actor : Actors)
        {
        if (actor->GetUUID () == uuid)
            return actor.get ();
        }

    return nullptr;
    }

void CLevel::DumpState () const
    {
    std::cout << "\n=== LEVEL STATE ===\n";
    std::cout << "Name: " << GetName () << "\n";
    std::cout << "UUID: " << GetShortUUID () << "\n";
    std::cout << "World: " << ( OwningWorld ? OwningWorld->GetName () : "None" ) << "\n";
    std::cout << "Is Playing: " << ( bIsPlaying ? "Yes" : "No" ) << "\n";
    std::cout << "Total Actors: " << Actors.size () << "\n";

    for (size_t i = 0; i < Actors.size (); ++i)
        {
        std::cout << "  [" << i << "] " << Actors[ i ]->GetName ()
            << " (" << Actors[ i ]->GetObjectClassName () << ")\n";
        }

    std::cout << "===================\n";
    }

    // ========== FACTORY REGISTRATION ==========


namespace
    {
    struct CLevelRegistrar
        {
        CLevelRegistrar ()
            {
            CObjectFactory::GetInstance ().RegisterClass<CLevel> ();
            CObjectFactory::GetInstance ().RegisterClass<CActor> ();
            }
        };
    static CLevelRegistrar CLevel_AutoReg;
    }