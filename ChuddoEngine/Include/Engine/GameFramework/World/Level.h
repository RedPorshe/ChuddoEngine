// Level.h - исправленная версия с сырыми указателями
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
        std::vector<CActor *> Actors;  // Сырые указатели, как в CActor для компонентов

    public:
        CLevel ( CObject * owner = nullptr, const std::string & inName = "Level" );
        virtual ~CLevel ();

        virtual void BeginPlay ();
        virtual void Tick ( float DeltaTime );
        virtual void EndPlay ();

        // World access
        CWorld * GetOwningWorld () const { return OwningWorld; }

        // Основной метод создания актора по образцу CActor::AddDefaultSubObject
        template<typename ActorType, typename... Args>
        ActorType * SpawnActor ( const std::string & name = "Actor", Args&&... args )
            {
            static_assert( std::is_base_of<CActor, ActorType>::value,
                           "ActorType must be derived from CActor" );

                       // Создаем актор через фабрику (аналогично AddSubObject в CActor)
            ActorType * newActor = this->AddSubObject<ActorType> ( name, std::forward<Args> ( args )... );

            if (!newActor)
                {
                LOG_ERROR( "[LEVEL] Error: Failed to spawn actor '",  name, "'");
                return nullptr;
                }

                // Добавляем сырой указатель в вектор (как в CActor для компонентов)
            Actors.push_back ( newActor );
            

            if (bIsPlaying)
                {
                newActor->BeginPlay ();
                }

            return newActor;
            }

       

        bool DestroyActor ( CActor * actor );
        bool DestroyActor ( const std::string & actorName );

        // Получение списка акторов
        const std::vector<CActor *> & GetActors () const { return Actors; }
        size_t GetNumActors () const { return Actors.size (); }

        // Search
        CObject * FindObjectByName ( const std::string & name ) const;
        CObject * FindObjectByUUID ( const std::string & uuid ) const;

        // Поиск акторов по типу
        template<typename ActorType>
        std::vector<ActorType *> FindActorsByType () const
            {
            std::vector<ActorType *> result;
            for (CActor * actor : Actors)
                {
                if (ActorType * typedActor = dynamic_cast< ActorType * >( actor ))
                    {
                    result.push_back ( typedActor );
                    }
                }
            return result;
            }

        template<typename ActorType>
        ActorType * FindActorByType () const
            {
            for (CActor * actor : Actors)
                {
                if (ActorType * typedActor = dynamic_cast< ActorType * >( actor ))
                    {
                    return typedActor;
                    }
                }
            return nullptr;
            }

            // Debug
        virtual void DumpState () const;

        CWorld * OwningWorld = nullptr;

    private:
        bool bIsPlaying = false;

        // Вспомогательный метод для удаления актора из вектора
        bool RemoveActorFromVector ( CActor * actor );
    };