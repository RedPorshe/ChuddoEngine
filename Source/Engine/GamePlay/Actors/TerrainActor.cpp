#include "Engine/GamePlay/Actors/TerrainActor.h"

#include <cmath>
#include <vector>

#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/GamePlay/World/World.h"
#include "Engine/Utils/Logger.h"
#include "glm/glm.hpp"

namespace CE
{
  TerrainActor::TerrainActor(CObject* Owner, FString NewName)
      : CActor(Owner, NewName)
  {
  }

  void TerrainActor::BeginPlay()
  {
    CActor::BeginPlay();
    CE_CORE_DEBUG("TerrainActor BeginPlay: ", GetName());

    // Create terrain mesh
    m_TerrainMesh = AddDefaultSubObject<CMeshComponent>("TerrainMesh");
    if (m_TerrainMesh)
    {
      // Generate and set procedural terrain (including heightmap data)
      GenerateTerrainMesh();

      // Position terrain at origin
      m_TerrainMesh->SetRelativePosition(glm::vec3(0.0f, -2.0f, 0.0f));

      // Set terrain color to green
      m_TerrainMesh->SetColor(glm::vec4(0.2f, 0.7f, 0.2f, 1.0f));
    }

    CE_LOG("[LogTemp] Terrain heightmap collision ready with ", m_GridWidth, "x", m_GridHeight, " heightmap");
  }

  void TerrainActor::Update(float DeltaTime)
  {
    CActor::Update(DeltaTime);
    // Terrain is static, no updates needed
  }

  void TerrainActor::GenerateTerrainMesh()
  {
    if (!m_TerrainMesh)
      return;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Initialize heightmap
    m_HeightMap.clear();
    m_HeightMap.resize(m_GridHeight + 1);
    for (int i = 0; i <= m_GridHeight; ++i)
    {
      m_HeightMap[i].resize(m_GridWidth + 1);
    }

    // Calculate terrain offsets for collision queries
    m_TerrainOffsetX = (m_GridWidth * 0.5f) * m_GridSpacing;
    m_TerrainOffsetZ = (m_GridHeight * 0.5f) * m_GridSpacing;

    // Generate grid vertices with height variation using Perlin-like noise simulation
    for (int z = 0; z <= m_GridHeight; ++z)
    {
      for (int x = 0; x <= m_GridWidth; ++x)
      {
        float posX = (x - m_GridWidth * 0.5f) * m_GridSpacing;
        float posZ = (z - m_GridHeight * 0.5f) * m_GridSpacing;

        // Simple height variation using sine/cosine for natural-looking terrain
        float height = std::sin(posX * 0.1f) * m_HeightScale + std::cos(posZ * 0.1f) * m_HeightScale * 0.5f;
        height += std::sin((posX + posZ) * 0.05f) * m_HeightScale * 0.3f;

        // Store height in heightmap for collision
        m_HeightMap[z][x] = height;

        Vertex vertex;
        vertex.position = glm::vec3(posX, height, posZ);
        vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);  // Default up normal
        vertex.texCoord = glm::vec2(x * 0.1f, z * 0.1f);
        vertex.color = glm::vec3(0.2f, 0.7f, 0.2f);

        vertices.push_back(vertex);
      }
    }

    // Generate triangles from grid
    for (int z = 0; z < m_GridHeight; ++z)
    {
      for (int x = 0; x < m_GridWidth; ++x)
      {
        int topLeft = z * (m_GridWidth + 1) + x;
        int topRight = topLeft + 1;
        int bottomLeft = (z + 1) * (m_GridWidth + 1) + x;
        int bottomRight = bottomLeft + 1;

        // First triangle
        indices.push_back(topLeft);
        indices.push_back(bottomLeft);
        indices.push_back(topRight);

        // Second triangle
        indices.push_back(topRight);
        indices.push_back(bottomLeft);
        indices.push_back(bottomRight);
      }
    }

    // Create static mesh and assign to component
    StaticMesh terrainMesh;
    terrainMesh.vertices = vertices;
    terrainMesh.indices = indices;
    terrainMesh.color = glm::vec3(0.2f, 0.7f, 0.2f);

    m_TerrainMesh->SetStaticMesh(terrainMesh);

    CE_LOG("[LogTemp] Terrain generated: ", m_GridWidth, "x", m_GridHeight, " grid with ", static_cast<int>(indices.size() / 3), " triangles");
  }

  float TerrainActor::GetHeightAtPosition(const glm::vec3& WorldPosition) const
  {
    // Validate heightmap exists and is properly initialized
    if (m_HeightMap.empty() || m_HeightMap[0].empty())
      return 0.0f;

    // Convert world position to terrain local position
    glm::vec3 localPos = WorldPosition - GetActorLocation();

    // Convert to grid coordinates
    float gridX = (localPos.x + m_TerrainOffsetX) / m_GridSpacing;
    float gridZ = (localPos.z + m_TerrainOffsetZ) / m_GridSpacing;

    // Clamp to grid bounds (inclusive of edges)
    if (gridX < 0.0f || gridX > static_cast<float>(m_GridWidth) ||
        gridZ < 0.0f || gridZ > static_cast<float>(m_GridHeight))
    {
      return GetActorLocation().y;  // Return terrain base height instead of 0
    }

    // Get integer grid coordinates
    int x0 = static_cast<int>(gridX);
    int z0 = static_cast<int>(gridZ);
    int x1 = x0 + 1;
    int z1 = z0 + 1;

    // Clamp indices to valid range if needed
    if (x0 < 0)
      x0 = 0;
    if (x1 > m_GridWidth)
      x1 = m_GridWidth;
    if (z0 < 0)
      z0 = 0;
    if (z1 > m_GridHeight)
      z1 = m_GridHeight;

    // Final vector bounds check
    if (z0 < 0 || z1 >= static_cast<int>(m_HeightMap.size()) ||
        x0 < 0 || x1 >= static_cast<int>(m_HeightMap[0].size()))
      return GetActorLocation().y;

    // Get fractional parts for interpolation
    float fx = gridX - x0;
    float fz = gridZ - z0;

    // Bilinear interpolation of height values
    float h00 = m_HeightMap[z0][x0];
    float h10 = m_HeightMap[z0][x1];
    float h01 = m_HeightMap[z1][x0];
    float h11 = m_HeightMap[z1][x1];

    float h0 = h00 * (1.0f - fx) + h10 * fx;
    float h1 = h01 * (1.0f - fx) + h11 * fx;
    float height = h0 * (1.0f - fz) + h1 * fz;

    return GetActorLocation().y + height;
  }
}  // namespace CE
