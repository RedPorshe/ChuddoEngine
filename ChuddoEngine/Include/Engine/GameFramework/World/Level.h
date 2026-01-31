// Level.h - исправленная версия
#pragma once
#include "Core/Object.h"
#include <vector>
#include <memory>

// Forward declarations
class CWorld;
class CActor;

class CLevel : public CObject
    {
    CHUDDO_DECLARE_CLASS ( CLevel, CObject )

    private:
        std::vector<std::unique_ptr<CActor>> Actors;

    public:
        CLevel ( CObject * owner = nullptr, const std::string & inName = "Level" );
        virtual ~CLevel ();

        virtual void BeginPlay ();
        virtual void Tick ( float DeltaTime );
        virtual void EndPlay ();

        // World access
        CWorld * GetOwningWorld () const { return OwningWorld; }

        // Actor management - ШАБЛОННЫЙ МЕТОД В .H ФАЙЛЕ!
        template<typename ActorType, typename... Args>
        ActorType * SpawnActor ( const std::string & name = "Actor", Args&&... args )
            {
            static_assert( std::is_base_of<CActor, ActorType>::value,
                           "ActorType must be derived from CActor" );

            auto actor = std::make_unique<ActorType> ( this, name, std::forward<Args> ( args )... );
            ActorType * rawPtr = actor.get ();

            Actors.push_back ( std::move ( actor ) );

            std::cout << "[LEVEL] Actor spawned: " << name
                << " (Total actors: " << Actors.size () << ")\n";

      
            if (bIsPlaying)
                {
                rawPtr->BeginPlay ();
                }

            return rawPtr;
            }

        bool DestroyActor ( CActor * actor );
        bool DestroyActor ( const std::string & actorName );

        // Search
        CObject * FindObjectByName ( const std::string & name ) const;
        CObject * FindObjectByUUID ( const std::string & uuid ) const;

        // Debug
        virtual void DumpState () const;

        CWorld * OwningWorld = nullptr;

    private:
        bool bIsPlaying = false;
    };

