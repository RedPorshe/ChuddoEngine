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
    // Не создаём ресурсы здесь, ждём OnBeginPlay
    if (!m_TerrainComponent)
        {
        CActor * owner = GetOwnerActor ();
        if (owner)
            {
            m_TerrainComponent = owner->FindComponent<CTerrainComponent> ();
            if (m_TerrainComponent)
                {
                LOG_DEBUG ( "[", GetName (), "] Found TerrainComponent: ", m_TerrainComponent->GetName () );
                }
            }
        }
    }

void CTerrainMeshComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }

void CTerrainMeshComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();

    // ВАЖНО: Создаём ресурсы здесь, после того как террейн сгенерирован
    if (m_TerrainComponent && !m_bRenderResourcesCreated)
        {
        LOG_DEBUG ( "[", GetName (), "] Creating terrain resources in OnBeginPlay" );
        UpdateFromTerrain ();
        }
    else if (!m_TerrainComponent)
        {
        LOG_WARN ( "[", GetName (), "] No terrain component in OnBeginPlay" );
        }
    }

FTerrainRenderInfo CTerrainMeshComponent::GetTerrainInfo () const
    {
    FTerrainRenderInfo Info;

    if (!IsReadyForRender ())
        {
        LOG_DEBUG ( "[", GetName (), "] Not ready for render" );
        return Info;
        }

   

      // Базовые Vulkan ресурсы
    Info.VertexBuffer = m_VertexBuffer;
    Info.VertexCount = m_VertexCount;
    Info.IndexBuffer = m_IndexBuffer;
    Info.IndexCount = m_IndexCount;

    // Матрица трансформации
    Info.Model = GetTransformMatrix ();

    // Данные о размерах террейна
    if (m_TerrainComponent)
        {
        const FTerrainData & data = m_TerrainComponent->GetTerrainData ();

        Info.Width = data.Width;
        Info.Height = data.Height;
        Info.CellSize = data.CellSize;
        Info.MinHeight = data.MinHeight;
        Info.MaxHeight = data.MaxHeight;

        // Настраиваем параметры для шейдера
        Info.Params.TilingFactor = 1.0f;
        Info.Params.HeightScale = 1.0f;
        Info.Params.FogDensity = 0.001f;
        Info.Params.UseTexture = 0.0f;

        // Высоты для слоёв текстурирования
        float range = data.MaxHeight - data.MinHeight;
        Info.Params.SandHeight = data.MinHeight + range * 0.1f;
        Info.Params.GrassHeight = data.MinHeight + range * 0.3f;
        Info.Params.RockHeight = data.MinHeight + range * 0.6f;
        Info.Params.SnowHeight = data.MinHeight + range * 0.8f;

      
        }

    Info.PipelineName = GetPipelineName ();
    return Info;
    }

void CTerrainMeshComponent::UpdateFromTerrain ()
    {
    if (!m_TerrainComponent)
        {
        LOG_ERROR ( "[", GetName (), "] No terrain component set!" );
        return;
        }

        // Проверяем, есть ли данные террейна
    const FTerrainData & data = m_TerrainComponent->GetTerrainData ();
    if (data.Width == 0 || data.Height == 0)
        {
        LOG_WARN ( "[", GetName (), "] Terrain data not generated yet!" );
        return;
        }

        // Проверяем, не созданы ли уже ресурсы
    if (m_bRenderResourcesCreated)
        {
        LOG_DEBUG ( "[", GetName (), "] Render resources already created, skipping..." );
        return;
        }

        // Уничтожаем старые ресурсы (на всякий случай)
    DestroyRenderResources ();

    // Обновляем кэш данных
    m_CachedData = &data;

    if (CEngine::Get ().GetRenderer ())
        {
        auto * bufferManager = CEngine::Get ().GetRenderer ()->GetBufferManager ();
        if (bufferManager)
            {
            CreateRenderResources ( bufferManager );
            }
        }

    LOG_DEBUG ( "[", GetName (), "] Updated from terrain: ",
                data.Width, "x", data.Height );
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

    LOG_DEBUG ( "[", GetName (), "] Generated ", OutVertices.size (), " vertices" );
    }

void CTerrainMeshComponent::GenerateIndices ( std::vector<uint32_t> & OutIndices ) const
    {
    OutIndices.clear ();

    if (!m_TerrainComponent) return;

    const FTerrainData & data = m_TerrainComponent->GetTerrainData ();

    if (data.Width < 2 || data.Height < 2) return;

    OutIndices.reserve ( ( data.Width - 1 ) * ( data.Height - 1 ) * 6 );

    for (int32 z = 0; z < data.Height - 1; ++z)
        {
        for (int32 x = 0; x < data.Width - 1; ++x)
            {
            uint32_t i0 = z * data.Width + x;
            uint32_t i1 = z * data.Width + x + 1;
            uint32_t i2 = ( z + 1 ) * data.Width + x;
            uint32_t i3 = ( z + 1 ) * data.Width + x + 1;

            // ДЛЯ CW (по часовой стрелке):
            // Первый треугольник (i0 -> i1 -> i3 -> i2?)
            // Давайте нарисуем:
            // i0 (0,0) -> i1 (1,0) -> i3 (1,1) -> обратно к i0
            OutIndices.push_back ( i0 );
            OutIndices.push_back ( i2 );
            OutIndices.push_back ( i1 );

            OutIndices.push_back ( i1 );
            OutIndices.push_back ( i2 );
            OutIndices.push_back ( i3 );

            // Для отладки - выведем первый треугольник
            static bool firstTime = true;
            if (firstTime && x == 0 && z == 0)
                {
                LOG_DEBUG ( "[TERRAIN] First triangle indices: ", i0, ", ", i1, ", ", i3 );
                LOG_DEBUG ( "[TERRAIN] Second triangle indices: ", i0, ", ", i3, ", ", i2 );
                firstTime = false;
                }
            }
        }
    }

FVector CTerrainMeshComponent::CalculateNormal ( int32 x, int32 z ) const
    {
    if (!m_TerrainComponent)
        {
        return FVector ( 0.0f, 1.0f, 0.0f );
        }

    const FTerrainData & data = m_TerrainComponent->GetTerrainData ();
    float cellSize = data.CellSize;

    float hL, hR, hD, hU;
    FVector pos = m_TerrainComponent->GetWorldPositionAt ( x, z );

    // Лево
    if (x > 0)
        hL = m_TerrainComponent->GetHeightAtWorld ( FVector ( pos.x - cellSize, 0, pos.z ) );
    else
        hL = pos.y;

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

    // Вычисляем нормаль
    FVector normal ( hL - hR, 2.0f * cellSize, hD - hU );

    float length = normal.Length ();
    if (length < 0.0001f)
        {
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

    if (heightRange < 0.0001f)
        {
        return FVector ( 0.5f, 0.5f, 0.5f );
        }

    float t = ( Height - data.MinHeight ) / heightRange;
    t = CEMath::Clamp ( t, 0.0f, 1.0f );

    // Градиент: синий (низ) -> зелёный -> красный (высокий)
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