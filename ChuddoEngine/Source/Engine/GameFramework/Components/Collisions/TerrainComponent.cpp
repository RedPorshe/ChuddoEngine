#include "Components/Collisions/TerrainComponent.h"
#include "Actors/Actor.h"
#include "Render/Vulkan/Managers/BufferManager.h"
#include "Render/Renderer.h"
#include "Core/CollisionSystem.h"
#include "Core/Engine.h"
#include <cmath>
#include <algorithm>


// ============================================================================
// FTerrainData Implementation
// ============================================================================

float FTerrainData::GetInterpolatedHeight ( float worldX, float worldZ ) const
    {
    if (Width < 2 || Height < 2)
        return 0.0f;

    // Преобразуем мировые координаты в локальные
    float localX = worldX - Origin.x;
    float localZ = worldZ - Origin.z;

    // Находим ячейку
    float cellX = localX / CellSize;
    float cellZ = localZ / CellSize;

    int32 x1 = static_cast< int32 >( std::floor ( cellX ) );
    int32 z1 = static_cast< int32 >( std::floor ( cellZ ) );
    int32 x2 = x1 + 1;
    int32 z2 = z1 + 1;

    // Проверяем границы
    if (x1 < 0 || x2 >= Width || z1 < 0 || z2 >= Height)
        return 0.0f;

    // Веса для интерполяции
    float fx = cellX - x1;
    float fz = cellZ - z1;

    // Получаем высоты четырёх углов
    float h11 = GetHeight ( x1, z1 );
    float h21 = GetHeight ( x2, z1 );
    float h12 = GetHeight ( x1, z2 );
    float h22 = GetHeight ( x2, z2 );

    // Билинейная интерполяция
    float h1 = h11 * ( 1.0f - fx ) + h21 * fx;
    float h2 = h12 * ( 1.0f - fx ) + h22 * fx;

    return h1 * ( 1.0f - fz ) + h2 * fz;
    }

// ============================================================================
// CTerrainComponent Implementation
// ============================================================================

CTerrainComponent::CTerrainComponent ( CObject * inOwner, const std::string & InName )
    : Super ( inOwner, InName )
    {
    SetShapeType ( ECollisionShape::TERRAIN );
    SetChannelAsStatic ();
    LOG_DEBUG ( "TerrainComponent created: ", GetName () );
    }

CTerrainComponent::~CTerrainComponent ()
    {
    // Очищаем рендер ресурсы
    DestroyRenderResources ();

    m_TerrainData.Heights.clear ();
    }

void CTerrainComponent::InitComponent ()
    {
    Super::InitComponent ();
    }

void CTerrainComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }

void CTerrainComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();

    // Создаём рендер ресурсы после того, как террейн сгенерирован
    auto&  engine = CEngine::Get ();
    if ( engine.GetRenderer ())
        {
        auto  bufferManager = engine.GetRenderer ()->GetBufferManager ();
        if (bufferManager)
            {
            CreateRenderResources ( bufferManager );
            }
        }
    }

// ============================================================================
// Terrain Generation
// ============================================================================

void CTerrainComponent::GenerateFlat ( int32 width, int32 height, float cellSize, float heightValue )
    {
    // Очищаем старые рендер ресурсы
    DestroyRenderResources ();

    m_TerrainData.Width = width;
    m_TerrainData.Height = height;
    m_TerrainData.CellSize = cellSize;
    m_TerrainData.MinHeight = heightValue;
    m_TerrainData.MaxHeight = heightValue;

    size_t totalPoints = width * height;
    m_TerrainData.Heights.resize ( totalPoints, heightValue );

    // Вычисляем центр
    float totalWidth = ( width - 1 ) * cellSize;
    float totalHeight = ( height - 1 ) * cellSize;
    m_TerrainData.Origin = FVector ( totalWidth * 0.5f, 0.0f, totalHeight * 0.5f );

    LOG_DEBUG ( "Terrain generated: ", width, "x", height, " cells, size: ",
                totalWidth, " x ", totalHeight );
    }

void CTerrainComponent::GenerateFromHeightmap ( const std::vector<float> & heights,
                                                int32 width, int32 height, float cellSize )
    {
    // Очищаем старые рендер ресурсы
    DestroyRenderResources ();

    if (heights.size () != static_cast< size_t >( width * height ))
        {
        LOG_ERROR ( "Heightmap size mismatch! Expected ", width * height, " got ", heights.size () );
        return;
        }

    m_TerrainData.Heights = heights;
    m_TerrainData.Width = width;
    m_TerrainData.Height = height;
    m_TerrainData.CellSize = cellSize;

    // Находим мин/макс высоты
    m_TerrainData.MinHeight = *std::min_element ( heights.begin (), heights.end () );
    m_TerrainData.MaxHeight = *std::max_element ( heights.begin (), heights.end () );

    // Вычисляем центр
    float totalWidth = ( width - 1 ) * cellSize;
    float totalHeight = ( height - 1 ) * cellSize;
    m_TerrainData.Origin = FVector ( totalWidth * 0.5f, 0.0f, totalHeight * 0.5f );

    LOG_DEBUG ( "Terrain generated from heightmap: ", width, "x", height,
                ", height range: ", m_TerrainData.MinHeight, " - ", m_TerrainData.MaxHeight );
    }

void CTerrainComponent::SetHeightAt ( int32 x, int32 z, float height )
    {
    if (x >= 0 && x < m_TerrainData.Width && z >= 0 && z < m_TerrainData.Height)
        {
        m_TerrainData.Heights[ z * m_TerrainData.Width + x ] = height;

        // Обновляем мин/макс
        m_TerrainData.MinHeight = std::min ( m_TerrainData.MinHeight, height );
        m_TerrainData.MaxHeight = std::max ( m_TerrainData.MaxHeight, height );
        }
    }

// ============================================================================
// Height Queries
// ============================================================================

float CTerrainComponent::GetHeightAtWorld ( const FVector & worldPos ) const
    {
    return m_TerrainData.GetInterpolatedHeight ( worldPos.x, worldPos.z );
    }

float CTerrainComponent::GetHeightAtLocal ( float localX, float localZ ) const
    {
    return m_TerrainData.GetInterpolatedHeight (
        localX + m_TerrainData.Origin.x,
        localZ + m_TerrainData.Origin.z
    );
    }

FVector CTerrainComponent::GetWorldPositionAt ( int32 x, int32 z ) const
    {
    float worldX = m_TerrainData.Origin.x + x * m_TerrainData.CellSize;
    float worldZ = m_TerrainData.Origin.z + z * m_TerrainData.CellSize;
    float worldY = m_TerrainData.GetHeight ( x, z );

    return FVector ( worldX, worldY, worldZ );
    }

FVector CTerrainComponent::GetBoundingBox () const
    {
    float totalWidth = ( m_TerrainData.Width - 1 ) * m_TerrainData.CellSize;
    float totalDepth = ( m_TerrainData.Height - 1 ) * m_TerrainData.CellSize;
    float heightRange = m_TerrainData.MaxHeight - m_TerrainData.MinHeight;

    return FVector ( totalWidth, heightRange, totalDepth );
    }

// ============================================================================
// Collision Checks
// ============================================================================

bool CTerrainComponent::CheckCollision ( CBaseCollisionComponent * other, FCollisionInfo & outInfo ) const
    {
    if (!other || !IsCollisionEnabled () || !other->IsCollisionEnabled ())
        return false;

    if (!CanCollideWith ( other ))
        return false;

    ECollisionShape otherShape = other->GetShapeType ();
    CTerrainComponent * nonConstThis = const_cast< CTerrainComponent * >( this );

    switch (otherShape)
        {
            case ECollisionShape::NONE:
                return false;

            case ECollisionShape::SPHERE:
                return COLLISION_SYSTEM.CheckSphereTerrain ( other, nonConstThis, outInfo );

            case ECollisionShape::BOX:
                return COLLISION_SYSTEM.CheckBoxTerrain ( other, nonConstThis, outInfo );

            case ECollisionShape::CAPSULE:
                return COLLISION_SYSTEM.CheckCapsuleTerrain ( other, nonConstThis, outInfo );

            case ECollisionShape::CYLINDER:
                return COLLISION_SYSTEM.CheckCylinderTerrain ( other, nonConstThis, outInfo );

            case ECollisionShape::CONE:
                return COLLISION_SYSTEM.CheckConeTerrain ( other, nonConstThis, outInfo );

            case ECollisionShape::COMPOUND:
                LOG_DEBUG ( "stub for Terrain-Compound collision" );
                return false;

            case ECollisionShape::MESH:
                LOG_DEBUG ( "stub for Terrain-Mesh collision" );
                return false;

            case ECollisionShape::TERRAIN:
                {
                static int warnCount = 0;
                if (warnCount < 1)
                    {
                    LOG_DEBUG ( "Terrain-Terrain collision not implemented" );
                    warnCount++;
                    }
                return false;
                }

            case ECollisionShape::RAY:
                LOG_DEBUG ( "stub for Terrain-Ray collision" );
                return false;

            case ECollisionShape::PLANE:
                LOG_DEBUG ( "stub for Terrain-Plane collision" );
                return false;

            case ECollisionShape::MAX:
            default:
                break;
        }
    return false;
    }

bool CTerrainComponent::RaycastTerrain ( const FVector & start, const FVector & dir, float maxDist,
                                         FVector & outHit, FVector & outNormal, float & outDist ) const
    {
    float step = m_TerrainData.CellSize * 0.5f;
    FVector current = start;
    float traveled = 0.0f;

    while (traveled < maxDist)
        {
        float terrainY = GetHeightAtWorld ( current );

        if (current.y <= terrainY)
            {
            outHit = current;
            outNormal = FVector ( 0.0f, 1.0f, 0.0f );
            outDist = traveled;
            return true;
            }

        current += dir * step;
        traveled += step;
        }

    return false;
    }

// ============================================================================
// Render Resources
// ============================================================================

void CTerrainComponent::CreateRenderResources ( CBufferManager * BufferManager )
    {
    if (!BufferManager || m_TerrainData.Heights.empty ())
        {
        return;
        }

    // Если уже есть ресурсы, не создаём заново
    if (HasRenderResources ())
        {
        return;
        }

    // Генерируем вершины
    std::vector<FTerrainVertex> vertices;
    GenerateVertices ( vertices );
    m_VertexCount = static_cast< uint32_t > ( vertices.size () );

    // Генерируем индексы
    std::vector<uint32_t> indices;
    GenerateIndices ( indices );
    m_IndexCount = static_cast< uint32_t > ( indices.size () );

    // Создаём вершинный буфер
    FBuffer vertexBuffer = BufferManager->CreateVertexBuffer ( vertices );
    if (vertexBuffer.IsValid ())
        {
        m_VertexBuffer = vertexBuffer.Buffer;
        // BufferManager владеет памятью, мы только храним handle
        }

    // Создаём индексный буфер (если есть индексы)
    if (!indices.empty ())
        {
        FBuffer indexBuffer = BufferManager->CreateIndexBuffer ( indices );
        if (indexBuffer.IsValid ())
            {
            m_IndexBuffer = indexBuffer.Buffer;
            }
        }

    LOG_DEBUG ( "[", GetName (), "] Created render resources: ",
                m_VertexCount, " vertices, ", m_IndexCount, " indices" );
    }

void CTerrainComponent::DestroyRenderResources ()
    {
    // BufferManager сам удалит буферы при Shutdown
    // Мы только обнуляем указатели
    m_VertexBuffer = VK_NULL_HANDLE;
    m_IndexBuffer = VK_NULL_HANDLE;
    m_VertexCount = 0;
    m_IndexCount = 0;
    }

void CTerrainComponent::GenerateVertices ( std::vector<FTerrainVertex> & vertices ) const
    {
    vertices.clear ();
    vertices.reserve ( m_TerrainData.Width * m_TerrainData.Height );

    for (int32 z = 0; z < m_TerrainData.Height; ++z)
        {
        for (int32 x = 0; x < m_TerrainData.Width; ++x)
            {
            FTerrainVertex vert;

            // Позиция
            vert.Position = GetWorldPositionAt ( x, z );

            // Нормаль (вычисляем через соседние точки)
            float hL, hR, hD, hU;
            float step = m_TerrainData.CellSize;

            // Лево
            if (x > 0)
                hL = GetHeightAtWorld ( FVector ( vert.Position.x - step, 0, vert.Position.z ) );
            else
                hL = vert.Position.y;

            // Право
            if (x < m_TerrainData.Width - 1)
                hR = GetHeightAtWorld ( FVector ( vert.Position.x + step, 0, vert.Position.z ) );
            else
                hR = vert.Position.y;

            // Низ (по Z)
            if (z > 0)
                hD = GetHeightAtWorld ( FVector ( vert.Position.x, 0, vert.Position.z - step ) );
            else
                hD = vert.Position.y;

            // Верх (по Z)
            if (z < m_TerrainData.Height - 1)
                hU = GetHeightAtWorld ( FVector ( vert.Position.x, 0, vert.Position.z + step ) );
            else
                hU = vert.Position.y;

            FVector normal ( hL - hR, 2.0f * step, hD - hU );
            float normalLength = normal.Length ();
            if (normalLength > 0.0001f)
                vert.Normal = normal / normalLength;
            else
                vert.Normal = FVector ( 0.0f, 1.0f, 0.0f );

            // Цвет по высоте (для визуализации)
            float heightRange = m_TerrainData.MaxHeight - m_TerrainData.MinHeight;
            float t = ( heightRange > 0.0001f ) ?
                ( vert.Position.y - m_TerrainData.MinHeight ) / heightRange : 0.5f;

      // Градиент от синего (низ) к зелёному (средний) к красному (высокий)
            if (t < 0.5f)
                {
                // От синего к зелёному
                float t2 = t * 2.0f;
                vert.Color = FVector ( 0.0f, t2, 1.0f - t2 );
                }
            else
                {
                // От зелёного к красному
                float t2 = ( t - 0.5f ) * 2.0f;
                vert.Color = FVector ( t2, 1.0f - t2, 0.0f );
                }

            // UV координаты (для текстурирования)
            vert.UV.x = static_cast< float > ( x ) / ( m_TerrainData.Width - 1 );
            vert.UV.y = static_cast< float > ( z ) / ( m_TerrainData.Height - 1 );

            vertices.push_back ( vert );
            }
        }
    }

void CTerrainComponent::GenerateIndices ( std::vector<uint32_t> & indices ) const
    {
    indices.clear ();

    if (m_TerrainData.Width < 2 || m_TerrainData.Height < 2)
        return;

    indices.reserve ( ( m_TerrainData.Width - 1 ) * ( m_TerrainData.Height - 1 ) * 6 );

    for (int32 z = 0; z < m_TerrainData.Height - 1; ++z)
        {
        for (int32 x = 0; x < m_TerrainData.Width - 1; ++x)
            {
            uint32_t i0 = z * m_TerrainData.Width + x;
            uint32_t i1 = z * m_TerrainData.Width + x + 1;
            uint32_t i2 = ( z + 1 ) * m_TerrainData.Width + x;
            uint32_t i3 = ( z + 1 ) * m_TerrainData.Width + x + 1;

            // Первый треугольник (верхний левый)
            indices.push_back ( i0 );
            indices.push_back ( i1 );
            indices.push_back ( i2 );

            // Второй треугольник (нижний правый)
            indices.push_back ( i1 );
            indices.push_back ( i3 );
            indices.push_back ( i2 );
            }
        }
    }

// Добавьте эти методы в CTerrainComponent.cpp

void CTerrainComponent::GenerateHilly ( int32 width, int32 height, float cellSize,
                                        float amplitude, float frequency )
    {
        // Очищаем старые рендер ресурсы
    DestroyRenderResources ();

    m_TerrainData.Width = width;
    m_TerrainData.Height = height;
    m_TerrainData.CellSize = cellSize;

    size_t totalPoints = width * height;
    m_TerrainData.Heights.resize ( totalPoints );

    m_TerrainData.MinHeight = FLT_MAX;
    m_TerrainData.MaxHeight = -FLT_MAX;

    // Генерируем холмы с помощью синусоид
    for (int32 z = 0; z < height; ++z)
        {
        for (int32 x = 0; x < width; ++x)
            {
                // Комбинация нескольких синусоид для более естественного вида
            float h1 = sin ( x * frequency ) * cos ( z * frequency ) * amplitude;
            float h2 = sin ( x * frequency * 2.3f ) * cos ( z * frequency * 2.3f ) * amplitude * 0.5f;
            float h3 = sin ( x * frequency * 0.7f ) * cos ( z * frequency * 0.7f ) * amplitude * 0.8f;

            float heightValue = h1 + h2 + h3 + amplitude * 0.5f; // Смещаем вверх

            m_TerrainData.Heights[ z * width + x ] = heightValue;

            // Обновляем мин/макс
            if (heightValue < m_TerrainData.MinHeight) m_TerrainData.MinHeight = heightValue;
            if (heightValue > m_TerrainData.MaxHeight) m_TerrainData.MaxHeight = heightValue;
            }
        }

        // Вычисляем центр
    float totalWidth = ( width - 1 ) * cellSize;
    float totalHeight = ( height - 1 ) * cellSize;
    m_TerrainData.Origin = FVector ( totalWidth * 0.5f, 0.0f, totalHeight * 0.5f );

    LOG_DEBUG ( "Hilly terrain generated: ", width, "x", height,
                ", height range: ", m_TerrainData.MinHeight, " - ", m_TerrainData.MaxHeight );
    }

    // Простая реализация шума (можно заменить на настоящий Perlin noise)
static float Noise ( int32 x, int32 y, int32 seed )
    {
    int32 n = x + y * 57 + seed * 131;
    n = ( n << 13 ) ^ n;
    return ( 1.0f - ( ( n * ( n * n * 60493 + 19990303 ) + 1376312589 ) & 0x7fffffff ) / 1073741824.0f );
    }

void CTerrainComponent::GenerateNoise ( int32 width, int32 height, float cellSize, int32 seed )
    {
    DestroyRenderResources ();

    m_TerrainData.Width = width;
    m_TerrainData.Height = height;
    m_TerrainData.CellSize = cellSize;

    size_t totalPoints = width * height;
    m_TerrainData.Heights.resize ( totalPoints );

    m_TerrainData.MinHeight = FLT_MAX;
    m_TerrainData.MaxHeight = -FLT_MAX;

    float frequency = 0.05f;
    float amplitude = 50.0f;

    for (int32 z = 0; z < height; ++z)
        {
        for (int32 x = 0; x < width; ++x)
            {
                // Простой шум с несколькими октавами
            float heightValue = 0;
            float amp = amplitude;
            float freq = frequency;

            for (int octave = 0; octave < 4; ++octave)
                {
                heightValue += Noise ( static_cast< int32 > ( x * freq ),
                                       static_cast< int32 > ( z * freq ), seed ) * amp;
                amp *= 0.5f;
                freq *= 2.0f;
                }

            heightValue += amplitude * 0.5f; // Смещаем вверх

            m_TerrainData.Heights[ z * width + x ] = heightValue;

            if (heightValue < m_TerrainData.MinHeight) m_TerrainData.MinHeight = heightValue;
            if (heightValue > m_TerrainData.MaxHeight) m_TerrainData.MaxHeight = heightValue;
            }
        }

    float totalWidth = ( width - 1 ) * cellSize;
    float totalHeight = ( height - 1 ) * cellSize;
    m_TerrainData.Origin = FVector ( totalWidth * 0.5f, 0.0f, totalHeight * 0.5f );

    LOG_DEBUG ( "Noise terrain generated: ", width, "x", height,
                ", height range: ", m_TerrainData.MinHeight, " - ", m_TerrainData.MaxHeight );
    }

void CTerrainComponent::GenerateCustom ( int32 width, int32 height, float cellSize,
                                         std::function<float ( int32 x, int32 z )> heightFunc )
    {
    DestroyRenderResources ();

    m_TerrainData.Width = width;
    m_TerrainData.Height = height;
    m_TerrainData.CellSize = cellSize;

    size_t totalPoints = width * height;
    m_TerrainData.Heights.resize ( totalPoints );

    m_TerrainData.MinHeight = FLT_MAX;
    m_TerrainData.MaxHeight = -FLT_MAX;

    for (int32 z = 0; z < height; ++z)
        {
        for (int32 x = 0; x < width; ++x)
            {
            float heightValue = heightFunc ( x, z );
            m_TerrainData.Heights[ z * width + x ] = heightValue;

            if (heightValue < m_TerrainData.MinHeight) m_TerrainData.MinHeight = heightValue;
            if (heightValue > m_TerrainData.MaxHeight) m_TerrainData.MaxHeight = heightValue;
            }
        }

    float totalWidth = ( width - 1 ) * cellSize;
    float totalHeight = ( height - 1 ) * cellSize;
    m_TerrainData.Origin = FVector ( totalWidth * 0.5f, 0.0f, totalHeight * 0.5f );

    LOG_DEBUG ( "Custom terrain generated: ", width, "x", height,
                ", height range: ", m_TerrainData.MinHeight, " - ", m_TerrainData.MaxHeight );
    }

bool CTerrainComponent::LoadFromHeightmap ( const std::string & filename, float cellSize )
    {
        // TODO: Реализовать загрузку изображения
        // Пока заглушка
    LOG_ERROR ( "LoadFromHeightmap not implemented yet" );
    return false;
    }