#include "World/Level.h"
#include "World/World.h"
#include "Actors/Actor.h"

// system includes
#include <iostream>
#include <algorithm>

CLevel::CLevel ( CObject * owner, const std::string & inName )
    : CObject ( owner, inName )
    {
        // Получаем World из владельца
    OwningWorld = dynamic_cast< CWorld * >( owner );

    if (OwningWorld)
        {
        LOG_INFO ( "[LEVEL] Level created: ", inName, " [World: ", OwningWorld->GetName (), "]" );            
        }
    else
        {
        LOG_INFO ( "[LEVEL] Level created: " , inName , " [No world]");
        }
    }

CLevel::~CLevel ()
    {
    LOG_INFO ( "[LEVEL] Level destroyed: ", GetName () );
    DumpState ();
    if (bIsPlaying)
        {
        EndPlay ();
        }

        // Очищаем вектор сырых указателей (не удаляем объекты - они в OwnedObjects)
    Actors.clear ();
    }

void CLevel::BeginPlay ()
    {
    if (bIsPlaying)
        {
        LOG_ERROR  ( "[LEVEL] ERROR: Level is already playing!" );
        return;
        }

    bIsPlaying = true;
    LOG_DEBUG ( "[LEVEL] BeginPlay '" , GetName () ,"'");

    // Запускаем всех акторов
    for (CActor * actor : Actors)
        {
        if (actor)
            {
            actor->BeginPlay ();
            }
        }
    }

void CLevel::Tick ( float DeltaTime )
    {
    if (!bIsPlaying)
        {
        LOG_WARN( "[LEVEL] Level is not playing, skipping tick");
        return;
        }

        // Обновляем всех акторов
    for (CActor * actor : Actors)
        {
        if (actor && !actor->IsAttached ())
            {
            actor->Tick ( DeltaTime );
            }
        }
    }

void CLevel::EndPlay ()
    {
    if (!bIsPlaying)
        return;

    bIsPlaying = false;
    LOG_DEBUG( "[LEVEL] End Play for '", GetName (), "'" ) ;

    // Завершаем всех акторов
    for (CActor * actor : Actors)
        {
        if (actor)
            {
            actor->EndPlay ();
            }
        }
    }

bool CLevel::RemoveActorFromVector ( CActor * actor )
    {
    if (!actor)
        return false;

    auto it = std::find ( Actors.begin (), Actors.end (), actor );
    if (it != Actors.end ())
        {
        Actors.erase ( it );
        return true;
        }
    return false;
    }

bool CLevel::DestroyActor ( CActor * actor )
    {
    if (!actor)
        return false;

    // Удаляем актор из вектора сырых указателей
    if (!RemoveActorFromVector ( actor ))
        {
        LOG_ERROR ("[LEVEL] Error: Actor not found in level: "
            , actor->GetName () );
        return false;
        }

        // Завершаем актора если уровень играет
    if (bIsPlaying)
        {
        actor->EndPlay ();
        }

    LOG_DEBUG("[LEVEL] Actor destroyed: " , actor->GetName () );

    auto it = std::find_if ( OwnedObjects.begin (), OwnedObjects.end (),
                             [ actor ] ( const std::unique_ptr<CObject> & obj )
                             {
                             return obj.get () == actor;
                             } );

    if (it != OwnedObjects.end ())
        {
        OwnedObjects.erase ( it );
        return true;
        }
    else
        {
        LOG_ERROR( "[LEVEL] Warning: Actor not found in OwnedObjects: "
            , actor->GetName () );
        return false;
        }
    }

bool CLevel::DestroyActor ( const std::string & actorName )
    {
        // Ищем актор по имени
    for (CActor * actor : Actors)
        {
        if (actor && actor->GetName () == actorName)
            {
            return DestroyActor ( actor );
            }
        }

    LOG_ERROR( "[LEVEL] Actor not found: " , actorName );
    return false;
    }

CObject * CLevel::FindObjectByName ( const std::string & name ) const
    {
        // Проверяем себя
    if (GetName () == name)
        return const_cast< CLevel * >( this );

    // Ищем в акторах
    for (CActor * actor : Actors)
        {
        if (actor && actor->GetName () == name)
            return actor;
        }

    return nullptr;
    }

CObject * CLevel::FindObjectByUUID ( const std::string & uuid ) const
    {
        // Проверяем себя
    if (GetUUID () == uuid)
        return const_cast< CLevel * >( this );

    // Ищем в акторах
    for (CActor * actor : Actors)
        {
        if (actor && actor->GetUUID () == uuid)
            return actor;
        }

    return nullptr;
    }

void CLevel::DumpState () const
    {
    LOG_DEBUG ( " == = LEVEL STATE == = ");
    LOG_DEBUG ( "Name: " , GetName ()) ;
    LOG_DEBUG ("UUID: " , GetShortUUID ());
    LOG_DEBUG ("World: " , ( OwningWorld ? OwningWorld->GetName () : "None" )) ;
    LOG_DEBUG (  "Is Playing: " , ( bIsPlaying ? "Yes" : "No" ));
    LOG_DEBUG( "Total Actors : " , Actors.size ());
    LOG_DEBUG (  "Total Owned Objects: " , OwnedObjects.size ());

    LOG_DEBUG ("Actors list:");
    for (size_t i = 0; i < Actors.size (); ++i)
        {
        if (Actors[ i ])
            {
            LOG_DEBUG ( "  [" , i , "] " , Actors[ i ]->GetName ()
                , " (" , Actors[ i ]->GetObjectClassName () , ")" );
            }
        else
            {
            LOG_DEBUG( "  [" , i , "] NULL pointer!");
            }
        }

    LOG_DEBUG( "Owned Objects list:");
    for (size_t i = 0; i < OwnedObjects.size (); ++i)
        {
        if (OwnedObjects[ i ])
            {
            LOG_DEBUG ( "  [" , i , "] " , OwnedObjects[ i ]->GetName ()
                , " (" , OwnedObjects[ i ]->GetObjectClassName () , ")");
            }
        }

    LOG_DEBUG (  "===================");
    }
REGISTER_CLASS_FACTORY ( CLevel );