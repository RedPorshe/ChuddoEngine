#include "Actors/TerrainActor.h"
#include "Components/Collisions/TerrainComponent.h"
#include "Components/GravityComponent.h"

CTerrainActor::CTerrainActor ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {
        // Создаём компонент террейна при создании актора
    m_TerrainComponent = AddDefaultSubObject<CTerrainComponent> ( inDisplayName + "_TerrainComponent" );

    if (m_TerrainComponent && GetRootComponent ())
        {
        GetRootComponent ()->SetCollisionComponent ( m_TerrainComponent );
        }

        // Террейну не нужна гравитация - удаляем её
    if (m_Gravity)
        {
        auto it = std::find ( ActorComponents.begin (), ActorComponents.end (), m_Gravity );
        if (it != ActorComponents.end ())
            {
            ActorComponents.erase ( it );
            }

            // Важно: не вызываем delete, так как объект всё ещё во владении!
            // Просто обнуляем указатель
        m_Gravity = nullptr;
        }

    LOG_DEBUG ( "[TERRAIN ACTOR] Created: ", GetName () );
    }

CTerrainActor::~CTerrainActor ()
    {
    m_TerrainComponent = nullptr;
    }

void CTerrainActor::BeginPlay ()
    {
    Super::BeginPlay ();
    }

void CTerrainActor::Tick ( float deltaTime )
    {
    Super::Tick ( deltaTime );
    }

void CTerrainActor::EndPlay ()
    {
    Super::EndPlay ();
    }

void CTerrainActor::GenerateFlat ( int32 width, int32 height, float cellSize, float heightValue )
    {
    if (m_TerrainComponent)
        {
        m_TerrainComponent->GenerateFlat ( width, height, cellSize, heightValue );
        }
    }

void CTerrainActor::GenerateFromHeightmap ( const std::vector<float> & heights, int32 width, int32 height, float cellSize )
    {
    if (m_TerrainComponent)
        {
        m_TerrainComponent->GenerateFromHeightmap ( heights, width, height, cellSize );
        }
    }