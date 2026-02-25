#include "Actors/TerrainActor.h"
#include "Components/Collisions/TerrainComponent.h"
#include "Components/Meshes/TerrainMeshComponent.h"  // Добавляем
#include "Components/GravityComponent.h"

CTerrainActor::CTerrainActor ( CObject * inOwner, const std::string & inDisplayName )
	: Super ( inOwner, inDisplayName )
	{
	// Создаём компонент террейна для коллизий
	m_TerrainComponent = AddDefaultSubObject<CTerrainComponent> ( inDisplayName + "_TerrainComponent" );

	// Создаём mesh компонент для рендера
	m_TerrainMeshComponent = AddDefaultSubObject<CTerrainMeshComponent> ( inDisplayName + "_TerrainMesh" );

	if (m_TerrainComponent && GetRootComponent ())
		{
		GetRootComponent ()->SetCollisionComponent ( m_TerrainComponent );
		}

	// Привязываем mesh компонент к данным террейна
	if (m_TerrainMeshComponent && m_TerrainComponent)
		{
		m_TerrainMeshComponent->SetTerrainComponent ( m_TerrainComponent );
		}

	// Террейну не нужна гравитация - удаляем её
	if (m_Gravity)
		{
		auto it = std::find ( ActorComponents.begin (), ActorComponents.end (), m_Gravity );
		if (it != ActorComponents.end ())
			{
			ActorComponents.erase ( it );
			}
		m_Gravity = nullptr;
		}

	LOG_DEBUG ( "[TERRAIN ACTOR] Created: ", GetName () );
	}

CTerrainActor::~CTerrainActor ()
	{}

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

std::vector<FMeshInfo> CTerrainActor::GetRenderMeshes () const
	{
	return Super::GetRenderMeshes ();
	}

// Добавляем геттер
CTerrainMeshComponent * CTerrainActor::GetTerrainMeshComponent () const
	{
	return m_TerrainMeshComponent;
	}

	// Обновляем Generate методы
void CTerrainActor::GenerateFlat ( int32 width, int32 height, float cellSize, float heightValue )
	{
	if (m_TerrainComponent)
		{
		m_TerrainComponent->GenerateFlat ( width, height, cellSize, heightValue );

		// Обновляем mesh после генерации
		if (m_TerrainMeshComponent)
			{
			m_TerrainMeshComponent->UpdateFromTerrain ();
			}
		}
	}

void CTerrainActor::GenerateFromHeightmap ( const std::vector<float> & heights, int32 width, int32 height, float cellSize )
	{
	if (m_TerrainComponent)
		{
		m_TerrainComponent->GenerateFromHeightmap ( heights, width, height, cellSize );

		// Обновляем mesh после генерации
		if (m_TerrainMeshComponent)
			{
			m_TerrainMeshComponent->UpdateFromTerrain ();
			}
		}
	}

void CTerrainActor::GenerateHilly ( int32 width, int32 height, float cellSize,
									float amplitude, float frequency )
	{
	if (m_TerrainComponent)
		{
		m_TerrainComponent->GenerateHilly ( width, height, cellSize, amplitude, frequency );

		if (m_TerrainMeshComponent)
			{
			m_TerrainMeshComponent->UpdateFromTerrain ();
			}
		}
	}

void CTerrainActor::GenerateNoise ( int32 width, int32 height, float cellSize, int32 seed )
	{
	if (m_TerrainComponent)
		{
		m_TerrainComponent->GenerateNoise ( width, height, cellSize, seed );

		if (m_TerrainMeshComponent)
			{
			m_TerrainMeshComponent->UpdateFromTerrain ();
			}
		}
	}

void CTerrainActor::GenerateCustom ( int32 width, int32 height, float cellSize,
									 std::function<float ( int32 x, int32 z )> heightFunc )
	{
	if (m_TerrainComponent)
		{
		m_TerrainComponent->GenerateCustom ( width, height, cellSize, heightFunc );

		if (m_TerrainMeshComponent)
			{
			m_TerrainMeshComponent->UpdateFromTerrain ();
			}
		}
	}