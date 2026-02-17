#include "Components/Collisions/TerrainComponent.h"
#include "Actors/Actor.h"
#include "Core/CollisionSystem.h"
#include <cmath>
#include <algorithm>
#include "Render/Mesh.h"

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
    }

    // ============================================================================
    // Terrain Generation
    // ============================================================================

void CTerrainComponent::GenerateFlat ( int32 width, int32 height, float cellSize, float heightValue )
    {
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

    // Also build simple flat render mesh
    m_RenderMesh = std::make_unique<CStaticMesh> ();
    auto & verts = m_RenderMesh->GetVerticesRef ();
    auto & idx = m_RenderMesh->GetIndicesRef ();
    verts.clear ();
    idx.clear ();

    for (int32 z = 0; z < height; ++z)
        {
        for (int32 x = 0; x < width; ++x)
            {
            Vertex v{};
            FVector pos = GetWorldPositionAt ( x, z );
            v.Position[0] = pos.x;
            v.Position[1] = pos.y;
            v.Position[2] = pos.z;
            v.UV[0] = static_cast<float> ( x ) / ( width - 1 );
            v.UV[1] = static_cast<float> ( z ) / ( height - 1 );
            v.Normal[0] = v.Normal[1] = v.Normal[2] = 0.0f;
            verts.push_back ( v );
            }
        }

    for (int32 z = 0; z < height - 1; ++z)
        {
        for (int32 x = 0; x < width - 1; ++x)
            {
            uint32_t i0 = z * width + x;
            uint32_t i1 = i0 + 1;
            uint32_t i2 = ( z + 1 ) * width + x;
            uint32_t i3 = i2 + 1;
            idx.push_back ( i0 ); idx.push_back ( i1 ); idx.push_back ( i3 );
            idx.push_back ( i0 ); idx.push_back ( i3 ); idx.push_back ( i2 );
            }
        }

    LOG_DEBUG ( "Terrain flat render mesh generated: vertices=", verts.size (), " indices=", idx.size () );
    }

void CTerrainComponent::GenerateFromHeightmap ( const std::vector<float> & heights,
                                                int32 width, int32 height, float cellSize )
    {
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

    // Build simple render mesh (grid of quads -> triangles)
    m_RenderMesh = std::make_unique<CStaticMesh> ();
    auto & verts = m_RenderMesh->GetVerticesRef ();
    auto & idx = m_RenderMesh->GetIndicesRef ();

    verts.clear ();
    idx.clear ();

    // Create vertices (one per grid point)
    for (int32 z = 0; z < height; ++z)
        {
        for (int32 x = 0; x < width; ++x)
            {
            Vertex v{};
            FVector pos = GetWorldPositionAt ( x, z );
            v.Position[0] = pos.x;
            v.Position[1] = pos.y;
            v.Position[2] = pos.z;
            v.UV[0] = static_cast<float> ( x ) / ( width - 1 );
            v.UV[1] = static_cast<float> ( z ) / ( height - 1 );
            v.Normal[0] = v.Normal[1] = v.Normal[2] = 0.0f; // placeholder; can be computed later
            verts.push_back ( v );
            }
        }

    // Create indices (two triangles per cell)
    for (int32 z = 0; z < height - 1; ++z)
        {
        for (int32 x = 0; x < width - 1; ++x)
            {
            uint32_t i0 = z * width + x;
            uint32_t i1 = i0 + 1;
            uint32_t i2 = ( z + 1 ) * width + x;
            uint32_t i3 = i2 + 1;

            // triangle 1: i0, i1, i3
            idx.push_back ( i0 );
            idx.push_back ( i1 );
            idx.push_back ( i3 );

            // triangle 2: i0, i3, i2
            idx.push_back ( i0 );
            idx.push_back ( i3 );
            idx.push_back ( i2 );
            }
        }

    LOG_DEBUG ( "Terrain render mesh generated: vertices=", verts.size (), " indices=", idx.size () );
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
                {
                return false;
                }

            case ECollisionShape::SPHERE:
                {
                return COLLISION_SYSTEM.CheckSphereTerrain ( other, nonConstThis, outInfo );
                }

            case ECollisionShape::BOX:
                {
                return COLLISION_SYSTEM.CheckBoxTerrain ( other, nonConstThis, outInfo );
                }

            case ECollisionShape::CAPSULE:
                {
                return COLLISION_SYSTEM.CheckCapsuleTerrain ( other, nonConstThis,  outInfo );
                }

            case ECollisionShape::CYLINDER:
                {                
                return COLLISION_SYSTEM.CheckCylinderTerrain(other,nonConstThis,outInfo);
                }

            case ECollisionShape::CONE:
                {                
                return COLLISION_SYSTEM.CheckConeTerrain(other,nonConstThis,outInfo);
                }

            case ECollisionShape::COMPOUND:
                {
                LOG_DEBUG ( "stub for Terrain-Compound collision" );
                return false;
                }

            case ECollisionShape::MESH:
                {                    
                LOG_DEBUG ( "stub for Terrain-Mesh collision" );
                return false;
                }

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
                {                
                LOG_DEBUG ( "stub for Terrain-Ray collision" );
                return false;
                }

            case ECollisionShape::PLANE:
                {                
                LOG_DEBUG ( "stub for Terrain-Plane collision" );
                return false;
                }

            case ECollisionShape::MAX:
            default:
                break;
        }
    return false;
    }
 
bool CTerrainComponent::RaycastTerrain ( const FVector & start, const FVector & dir, float maxDist,
                                         FVector & outHit, FVector & outNormal, float & outDist ) const
    {
        // Простой рейкаст по сетке террейна
        // TODO: Реализовать более эффективный алгоритм (например, DDA)

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