#include "Core/Level.h"
#include "Actors/Actor.h"

CLevel::CLevel ( const CObject * Owner, const std::string & inName )
    : CObject ( Owner, inName )
    {
        // Создаем тестового актора
    testActor = SpawnActor<CActor> ( "Test Actor" );
    }

CLevel::~CLevel ()
    {
    std::cout << "Level '" << GetName () << "' starting destruction." << std::endl;

    // Уничтожаем всех акторов
    for (auto & actorUP : m_UniqActors)
        {
        if (actorUP)
            {
            CActor * actor = actorUP.get ();
            std::cout << "Destroying actor '" << actor->GetName ()
                << "' during level destruction." << std::endl;
            }
        }

        // Очищаем векторы
    m_UniqActors.clear ();
    m_UniqPendingDestroyActors.clear ();
    Actors.clear ();
    ActorNameMap.clear ();

    std::cout << "Level '" << GetName () << "' destroyed." << std::endl;
    }

void CLevel::BeginPlay ()
    {
    std::cout << "Level '" << GetName () << "' BeginPlay." << std::endl;

    // Начинаем игру для подуровней
    if (HasOwnedObjects ())
        {
        for (size_t i = 0; i < GetOwnedObjectsCount (); ++i)
            {
            CObject * Obj = OwnedObjects[ i ].get ();
            if (Obj)
                {
                CLevel * LevelObj = dynamic_cast< CLevel * > ( Obj );
                if (LevelObj)
                    {
                    std::cout << "Starting BeginPlay on sublevel: "
                        << LevelObj->GetName () << std::endl;
                    LevelObj->BeginPlay ();
                    }
                }
            }
        }

        // Начинаем игру для акторов
    for (CActor * Actor : Actors)
        {
        if (Actor && !Actor->IsPendingDestroy ())
            {
            Actor->BeginPlay ();
            }
        }
    }

void CLevel::Tick ( float DeltaTime )
    {
    static int countTick = 0;

    // Обрабатываем уничтожение акторов в начале кадра
    ProcessPendingDestroyActors ();

    // Тикаем подуровни
    if (HasOwnedObjects ())
        {
        for (size_t i = 0; i < GetOwnedObjectsCount (); ++i)
            {
            CObject * Obj = OwnedObjects[ i ].get ();
            if (Obj)
                {
                CLevel * LevelObj = dynamic_cast< CLevel * > ( Obj );
                if (LevelObj)
                    {
                    LevelObj->Tick ( DeltaTime );
                    }
                }
            }
        }

        // Тикаем акторы
    if (!Actors.empty ())
        {
        for (CActor * Actor : Actors)
            {
            if (Actor && !Actor->IsPendingDestroy ())
                {
                Actor->Tick ( DeltaTime );
                }
            }
        }
    else
        {
        std::cout << "Level '" << GetName () << "': no actors for tick\n";
        }

      
    countTick++;
    if (countTick == 5 && testActor)
        {
        std::cout << "\n=== Level '" << GetName ()
            << "': Destroying test actor after 5 ticks ===\n";
        DestroyActor ( testActor );
        testActor = nullptr;
        }
    }

void CLevel::DestroyActor ( CActor * ActorToDestroy )
    {
    if (ActorToDestroy && !ActorToDestroy->IsPendingDestroy ())
        {
        std::cout << "DestroyActor: Marking actor '"
            << ActorToDestroy->GetName ()
            << "' for destruction in level '"
            << GetName () << "'." << std::endl;

        ActorToDestroy->SetPendingDestroy ( true );
      
        for (auto it = m_UniqActors.begin (); it != m_UniqActors.end (); ++it)
            {
            if (it->get () == ActorToDestroy)
                {
                m_UniqPendingDestroyActors.push_back ( std::move ( *it ) );
                m_UniqActors.erase ( it );

                UnregisterActor ( ActorToDestroy );

                std::cout << "DestroyActor: Actor '"
                    << ActorToDestroy->GetName ()
                    << "' moved to pending destruction." << std::endl;
                return;
                }
            }

        std::cerr << "DestroyActor: ERROR - Actor '"
            << ActorToDestroy->GetName ()
            << "' not found in m_UniqActors!" << std::endl;
        }
    }

void CLevel::DestroyActor ( const std::string & ActorName )
    {
    CActor * ActorToDestroy = nullptr;
    auto it = ActorNameMap.find ( ActorName );
    if (it != ActorNameMap.end ())
        {
        ActorToDestroy = it->second;
        if (ActorToDestroy)
            {
            DestroyActor ( ActorToDestroy );
            }
        }
    else
        {
        std::cerr << "DestroyActor: ERROR - Actor with name '"
            << ActorName << "' not found!" << std::endl;
        }
    }

void CLevel::RegisterActor ( CActor * Actor )
    {
    if (!Actor) return;

    auto it = std::find ( Actors.begin (), Actors.end (), Actor );
    if (it == Actors.end ())
        {
        Actors.push_back ( Actor );
        ActorNameMap[ Actor->GetName () ] = Actor;
        std::cout << "RegisterActor: Actor '"
            << Actor->GetName () << "' registered in level '"
            << GetName () << "'." << std::endl;
        }
    }

void CLevel::UnregisterActor ( CActor * Actor )
    {
    if (!Actor) return;

   
    auto it = std::find ( Actors.begin (), Actors.end (), Actor );
    if (it != Actors.end ())
        {
        Actors.erase ( it );
        }

    ActorNameMap.erase ( Actor->GetName () );

    std::cout << "UnregisterActor: Actor '"
        << Actor->GetName () << "' unregistered from level '"
        << GetName () << "'." << std::endl;
    }

void CLevel::ProcessPendingDestroyActors ()
    {
    std::string DestroyingActorName {};
    for (auto & actorUP : m_UniqPendingDestroyActors)
        {
        if (actorUP)
            {
            CActor * actor = actorUP.get ();
            std::cout << "ProcessPendingDestroyActors: Destroying actor '"
                << actor->GetName () << "'" << std::endl;
           
            actorUP.reset ();
            }
        }
    m_UniqPendingDestroyActors.clear ();   
    }