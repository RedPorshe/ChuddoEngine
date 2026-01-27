#pragma once

#include "CoreMinimal.h"
#include "Core/Object.h"

class CActor;

class CLevel : public CObject
    {
    public:
        CLevel ( const CObject * Owner, const std::string & inName = "Level" );

        virtual ~CLevel ();

        // Level-specific methods
        virtual void BeginPlay ();
        virtual void Tick ( float DeltaTime );

        // Spawn actor methods
        template<typename ActorClass>
        ActorClass * SpawnActor ()
            {
            return SpawnActor<ActorClass> ( std::string ( "Actor" ), FTransform::Identity () );
            }

        template<typename ActorClass>
        ActorClass * SpawnActorAt ( const FTransform & SpawnTransform )
            {
            return SpawnActor<ActorClass> ( std::string ( "Actor" ), SpawnTransform );
            }

            // Actor management
        void DestroyActor ( CActor * ActorToDestroy );
        void DestroyActor ( const std::string & ActorName );
        std::vector<CActor *> GetAllActors () const { return Actors; }

    private:
        void RegisterActor ( CActor * Actor );
        void UnregisterActor ( CActor * Actor );
        void ProcessPendingDestroyActors ();

        template<typename ActorClass>
        ActorClass * SpawnActor ( const std::string & inName, const FTransform & SpawnTransform = FTransform::Identity () )
            {
            static_assert( std::is_base_of<CActor, ActorClass>::value, "ActorClass must be derived from CActor" );

            // Создаем актора
            std::unique_ptr<ActorClass> NewActorUP = std::make_unique<ActorClass> ( this, inName );
            ActorClass * NewActor = NewActorUP.get ();

            std::cout << "Spawning Actor '" << NewActor->GetName ()
                << "' at location (" << SpawnTransform.Location
                << ") with Rotation (" << SpawnTransform.Rotation
                << ") and Scale (" << SpawnTransform.Scale
                << ") in Level '" << GetName () << "'." << std::endl;

      // Устанавливаем трансформацию
            NewActor->SetActorTransform ( SpawnTransform );

            // Сохраняем unique_ptr в m_UniqActors
            m_UniqActors.push_back ( std::move ( NewActorUP ) );

            // Регистрируем актора
            RegisterActor ( NewActor );

            return NewActor;
            }

    private:
        std::vector<CActor *> Actors;  // Raw pointers для быстрого доступа
        std::unordered_map<std::string, CActor *> ActorNameMap;
        std::vector<std::unique_ptr<CActor>> m_UniqActors;  // Владение акторами
        std::vector<std::unique_ptr<CActor>> m_UniqPendingDestroyActors;  // Акторы на уничтожение

        // Тестовый указатель (удалить в финальной версии)
        CActor * testActor = nullptr;
    };