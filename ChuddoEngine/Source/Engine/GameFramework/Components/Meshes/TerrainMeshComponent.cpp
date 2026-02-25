#include "Components/Meshes/TerrainMeshComponent.h"
#include "Components/Collisions/TerrainComponent.h"
#include "Render/Renderer.h"
#include "Core/Engine.h"
#include "Actors/Actor.h"

CTerrainMeshComponent::CTerrainMeshComponent ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {
    LOG_DEBUG ( "TerrainMeshComponent created: ", GetName () );
    }

CTerrainMeshComponent::~CTerrainMeshComponent ()
    {
    m_TerrainComponent = nullptr;
    m_CachedData = nullptr;
    }

void CTerrainMeshComponent::InitComponent ()
    {
    Super::InitComponent ();
    if (!m_TerrainComponent)
        {
        CActor * owner = GetOwnerActor ();
        if (owner)
            {

            m_TerrainComponent = owner->FindComponent<CTerrainComponent> ();
            }
        }

        // Создаём ресурсы после того, как террейн настроен
    if (m_TerrainComponent)
        {
        UpdateFromTerrain ();
        }
    }

void CTerrainMeshComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }

void CTerrainMeshComponent::OnBeginPlay ()
    {   
    Super::OnBeginPlay ();
    }

void CTerrainMeshComponent::UpdateFromTerrain ()
    {
    if (!m_TerrainComponent)
        {
        LOG_ERROR ( "[", GetName (), "] No terrain component set!" );
        return;
        }

        // Уничтожаем старые ресурсы
    DestroyRenderResources ();

    // Обновляем кэш данных
    m_CachedData = &m_TerrainComponent->GetTerrainData ();

    
    
    if (CEngine::Get ().GetRenderer ())
        {
        auto * bufferManager = CEngine::Get (). GetRenderer ()->GetBufferManager ();
        if (bufferManager)
            {
            CreateRenderResources ( bufferManager );
            }
        }

    LOG_DEBUG ( "[", GetName (), "] Updated from terrain: ",
                m_TerrainComponent->GetTerrainData ().Width, "x",
                m_TerrainComponent->GetTerrainData ().Height );
    }

void CTerrainMeshComponent::GenerateVertices ( std::vector<FMeshVertex> & OutVertices ) const
    {
    OutVertices.clear ();

    if (!m_TerrainComponent)
        {
        LOG_ERROR ( "[", GetName (), "] Cannot generate vertices - no terrain component!" );
        return;
        }

    const FTerrainData & data = m_TerrainComponent->GetTerrainData ();

    if (data.Width <= 0 || data.Height <= 0)
        {
        LOG_ERROR ( "[", GetName (), "] Invalid terrain dimensions!" );
        return;
        }

    OutVertices.reserve ( data.Width * data.Height );

    for (int32 z = 0; z < data.Height; ++z)
        {
        for (int32 x = 0; x < data.Width; ++x)
            {
            FMeshVertex vert;

            // Позиция
            vert.Position = m_TerrainComponent->GetWorldPositionAt ( x, z );

            // Нормаль
            vert.Normal = CalculateNormal ( x, z );

            // Цвет на основе высоты
            vert.Color = CalculateColor ( vert.Position.y );

            // UV координаты
            vert.UV.x = static_cast< float >( x ) / ( data.Width - 1 );
            vert.UV.y = static_cast< float >( z ) / ( data.Height - 1 );

            OutVertices.push_back ( vert );
            }
        }
    }

void CTerrainMeshComponent::GenerateIndices ( std::vector<uint32_t> & OutIndices ) const
    {
    OutIndices.clear ();

    if (!m_TerrainComponent)
        {
        return;
        }

    const FTerrainData & data = m_TerrainComponent->GetTerrainData ();

    if (data.Width < 2 || data.Height < 2)
        {
        return;
        }

    OutIndices.reserve ( ( data.Width - 1 ) * ( data.Height - 1 ) * 6 );

    for (int32 z = 0; z < data.Height - 1; ++z)
        {
        for (int32 x = 0; x < data.Width - 1; ++x)
            {
            uint32_t i0 = z * data.Width + x;
            uint32_t i1 = z * data.Width + x + 1;
            uint32_t i2 = ( z + 1 ) * data.Width + x;
            uint32_t i3 = ( z + 1 ) * data.Width + x + 1;

            // Первый треугольник (верхний левый)
            OutIndices.push_back ( i0 );
            OutIndices.push_back ( i1 );
            OutIndices.push_back ( i2 );

            // Второй треугольник (нижний правый)
            OutIndices.push_back ( i1 );
            OutIndices.push_back ( i3 );
            OutIndices.push_back ( i2 );
            }
        }
    }

FVector CTerrainMeshComponent::CalculateNormal ( int32 x, int32 z ) const
    {
    if (!m_TerrainComponent)
        {
        return FVector ( 0.0f, 1.0f, 0.0f ); // Нормаль вверх по умолчанию
        }

    const FTerrainData & data = m_TerrainComponent->GetTerrainData ();
    float cellSize = data.CellSize;

    // Получаем высоты соседних точек с проверкой границ
    float hL, hR, hD, hU;
    FVector pos = m_TerrainComponent->GetWorldPositionAt ( x, z );

    // Лево
    if (x > 0)
        hL = m_TerrainComponent->GetHeightAtWorld ( FVector ( pos.x - cellSize, 0, pos.z ) );
    else
        hL = pos.y;  // На границе используем текущую высоту

    // Право
    if (x < data.Width - 1)
        hR = m_TerrainComponent->GetHeightAtWorld ( FVector ( pos.x + cellSize, 0, pos.z ) );
    else
        hR = pos.y;

    // Низ (по Z)
    if (z > 0)
        hD = m_TerrainComponent->GetHeightAtWorld ( FVector ( pos.x, 0, pos.z - cellSize ) );
    else
        hD = pos.y;

    // Верх (по Z)
    if (z < data.Height - 1)
        hU = m_TerrainComponent->GetHeightAtWorld ( FVector ( pos.x, 0, pos.z + cellSize ) );
    else
        hU = pos.y;

    // Вычисляем нормаль через разности высот
    FVector normal ( hL - hR, 2.0f * cellSize, hD - hU );

    // Проверяем на нулевую длину
    float length = normal.Length ();
    if (length < 0.0001f)
        {
            // Если нормаль нулевая (плоский участок), возвращаем вектор вверх
        return FVector ( 0.0f, 1.0f, 0.0f );
        }

    return normal / length;
    }

FVector CTerrainMeshComponent::CalculateColor ( float Height ) const
    {
    if (!m_TerrainComponent)
        {
        return FVector ( 0.5f, 0.5f, 0.5f );
        }

    const FTerrainData & data = m_TerrainComponent->GetTerrainData ();
    float heightRange = data.MaxHeight - data.MinHeight;

    if (heightRange < 0.0001f) // Защита от деления на ноль
        {
        return FVector ( 0.5f, 0.5f, 0.5f );
        }

    float t = ( Height - data.MinHeight ) / heightRange;
    t = CEMath::Clamp ( t, 0.0f, 1.0f ); // Ограничиваем диапазон

    // Градиент: синий (низ) -> зелёный (средний) -> красный (высокий)
    if (t < 0.5f)
        {
        float t2 = t * 2.0f;
        return FVector ( 0.0f, t2, 1.0f - t2 );
        }
    else
        {
        float t2 = ( t - 0.5f ) * 2.0f;
        return FVector ( t2, 1.0f - t2, 0.0f );
        }
    }