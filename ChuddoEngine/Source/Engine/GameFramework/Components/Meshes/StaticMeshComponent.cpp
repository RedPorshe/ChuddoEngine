#include "Components/Meshes/StaticMeshComponent.h"
#include "Render/Vulkan/VertexStructs/AllVertices.h"
#include "Render/Renderer.h"
#include "Core/Engine.h"

CStaticMeshComponent::CStaticMeshComponent ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {
    LOG_DEBUG ( "StaticMeshComponent created: ", GetName () );
   
    }

CStaticMeshComponent::~CStaticMeshComponent ()
    {}

void CStaticMeshComponent::InitComponent ()
    {
    if (this == nullptr) return;
    Super::InitComponent ();
    LOG_DEBUG ( "Initializing Mesh component" );
    SetPipelineName ( "StaticMesh" ); // Убедитесь, что это совпадает с именем в PipelineManager
   // LOG_DEBUG ( "StaticMeshComponent created: ", GetName (),
    //            ", pipeline: ", GetPipelineName () );
    auto * bufferManager = CEngine::Get ().GetRenderer ()->GetBufferManager ();
    if (bufferManager)
        {
            // Если нет загруженного меша, создаём fallback куб
        if (StaticMesh_vertices.empty ())
            {
            CreateFallBackCube ();
            }

        CreateRenderResources ( bufferManager );
        }

    }

void CStaticMeshComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }

void CStaticMeshComponent::OnBeginPlay ()
    {
 
    Super::OnBeginPlay ();
    }

void CStaticMeshComponent::GenerateVertices ( std::vector<FMeshVertex> & OutVertices ) const
    {
    if (StaticMesh_vertices.empty ())
        {
            // Fallback куб (если по какой-то причине CreateFallBackCube не был вызван)
        OutVertices.clear ();

        OutVertices.push_back ( { {-0.5f, -0.5f,  0.5f}, {0,0,1}, {1,0,0}, {0,0} } );
        OutVertices.push_back ( { { 0.5f, -0.5f,  0.5f}, {0,0,1}, {0,1,0}, {1,0} } );
        OutVertices.push_back ( { { 0.5f,  0.5f,  0.5f}, {0,0,1}, {0,0,1}, {1,1} } );
        OutVertices.push_back ( { {-0.5f,  0.5f,  0.5f}, {0,0,1}, {1,1,0}, {0,1} } );
        OutVertices.push_back ( { {-0.5f, -0.5f, -0.5f}, {0,0,-1}, {0,1,1}, {1,0} } );
        OutVertices.push_back ( { { 0.5f, -0.5f, -0.5f}, {0,0,-1}, {1,0,1}, {0,0} } );
        OutVertices.push_back ( { { 0.5f,  0.5f, -0.5f}, {0,0,-1}, {1,1,1}, {0,1} } );
        OutVertices.push_back ( { {-0.5f,  0.5f, -0.5f}, {0,0,-1}, {0.5,0.5,0.5}, {1,1} } );

        LOG_DEBUG ( "[", GetName (), "] Generated fallback ", OutVertices.size (), " vertices for cube" );
        }
    else
        {
        OutVertices = StaticMesh_vertices; // Используем существующие вершины
        }
    }

void CStaticMeshComponent::GenerateIndices ( std::vector<uint32_t> & OutIndices ) const
    {
    if (StaticMesh_indices.empty ())
        {
            // Fallback индексы
        OutIndices.clear ();

        OutIndices.push_back ( 0 ); OutIndices.push_back ( 1 ); OutIndices.push_back ( 2 );
        OutIndices.push_back ( 0 ); OutIndices.push_back ( 2 ); OutIndices.push_back ( 3 );
        OutIndices.push_back ( 1 ); OutIndices.push_back ( 5 ); OutIndices.push_back ( 6 );
        OutIndices.push_back ( 1 ); OutIndices.push_back ( 6 ); OutIndices.push_back ( 2 );
        OutIndices.push_back ( 5 ); OutIndices.push_back ( 4 ); OutIndices.push_back ( 7 );
        OutIndices.push_back ( 5 ); OutIndices.push_back ( 7 ); OutIndices.push_back ( 6 );
        OutIndices.push_back ( 4 ); OutIndices.push_back ( 0 ); OutIndices.push_back ( 3 );
        OutIndices.push_back ( 4 ); OutIndices.push_back ( 3 ); OutIndices.push_back ( 7 );
        OutIndices.push_back ( 3 ); OutIndices.push_back ( 2 ); OutIndices.push_back ( 6 );
        OutIndices.push_back ( 3 ); OutIndices.push_back ( 6 ); OutIndices.push_back ( 7 );
        OutIndices.push_back ( 4 ); OutIndices.push_back ( 5 ); OutIndices.push_back ( 1 );
        OutIndices.push_back ( 4 ); OutIndices.push_back ( 1 ); OutIndices.push_back ( 0 );

        LOG_DEBUG ( "[", GetName (), "] Generated fallback ", OutIndices.size (), " indices for cube" );
        }
    else
        {
        OutIndices = StaticMesh_indices; // Используем существующие индексы
        }
    }

void CStaticMeshComponent::CreateFallBackCube ()
    {
    StaticMesh_vertices.clear ();
    StaticMesh_indices.clear ();

    // Вершины
    StaticMesh_vertices.push_back ( { {-0.5f, -0.5f,  0.5f}, {0,0,1}, {1,0,0}, {0,0} } ); // 0: красный
    StaticMesh_vertices.push_back ( { { 0.5f, -0.5f,  0.5f}, {0,0,1}, {0,1,0}, {1,0} } ); // 1: зелёный
    StaticMesh_vertices.push_back ( { { 0.5f,  0.5f,  0.5f}, {0,0,1}, {0,0,1}, {1,1} } ); // 2: синий
    StaticMesh_vertices.push_back ( { {-0.5f,  0.5f,  0.5f}, {0,0,1}, {1,1,0}, {0,1} } ); // 3: жёлтый
    StaticMesh_vertices.push_back ( { {-0.5f, -0.5f, -0.5f}, {0,0,-1}, {0,1,1}, {1,0} } ); // 4: голубой
    StaticMesh_vertices.push_back ( { { 0.5f, -0.5f, -0.5f}, {0,0,-1}, {1,0,1}, {0,0} } ); // 5: розовый
    StaticMesh_vertices.push_back ( { { 0.5f,  0.5f, -0.5f}, {0,0,-1}, {1,1,1}, {0,1} } ); // 6: белый
    StaticMesh_vertices.push_back ( { {-0.5f,  0.5f, -0.5f}, {0,0,-1}, {0.5,0.5,0.5}, {1,1} } ); // 7: серый

    // Индексы
    StaticMesh_indices.push_back ( 0 ); StaticMesh_indices.push_back ( 1 ); StaticMesh_indices.push_back ( 2 );
    StaticMesh_indices.push_back ( 0 ); StaticMesh_indices.push_back ( 2 ); StaticMesh_indices.push_back ( 3 );
    StaticMesh_indices.push_back ( 1 ); StaticMesh_indices.push_back ( 5 ); StaticMesh_indices.push_back ( 6 );
    StaticMesh_indices.push_back ( 1 ); StaticMesh_indices.push_back ( 6 ); StaticMesh_indices.push_back ( 2 );
    StaticMesh_indices.push_back ( 5 ); StaticMesh_indices.push_back ( 4 ); StaticMesh_indices.push_back ( 7 );
    StaticMesh_indices.push_back ( 5 ); StaticMesh_indices.push_back ( 7 ); StaticMesh_indices.push_back ( 6 );
    StaticMesh_indices.push_back ( 4 ); StaticMesh_indices.push_back ( 0 ); StaticMesh_indices.push_back ( 3 );
    StaticMesh_indices.push_back ( 4 ); StaticMesh_indices.push_back ( 3 ); StaticMesh_indices.push_back ( 7 );
    StaticMesh_indices.push_back ( 3 ); StaticMesh_indices.push_back ( 2 ); StaticMesh_indices.push_back ( 6 );
    StaticMesh_indices.push_back ( 3 ); StaticMesh_indices.push_back ( 6 ); StaticMesh_indices.push_back ( 7 );
    StaticMesh_indices.push_back ( 4 ); StaticMesh_indices.push_back ( 5 ); StaticMesh_indices.push_back ( 1 );
    StaticMesh_indices.push_back ( 4 ); StaticMesh_indices.push_back ( 1 ); StaticMesh_indices.push_back ( 0 );

    LOG_DEBUG ( "[", GetName (), "] Created fallback cube with ",
                StaticMesh_vertices.size (), " vertices and ",
                StaticMesh_indices.size (), " indices" );
    }