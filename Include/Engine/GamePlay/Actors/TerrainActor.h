#pragma once

#include <vector>

#include "Engine/Core/Object.h"
#include "Engine/Core/CoreTypes.h"
#include "Engine/GamePlay/Actors/Actor.h"



  class CMeshComponent;

  class TerrainActor : public CActor
  {
   public:
    TerrainActor(CObject* Owner, FString NewName);
    virtual ~TerrainActor() = default;

    virtual void BeginPlay() override;
    virtual void Update(float DeltaTime) override;

    // Get terrain height at world position (for heightmap collision)
    float GetHeightAtPosition(const FVector& WorldPosition) const;

    // Check if heightmap is initialized and ready
    bool IsHeightmapReady() const
    {
      return !m_HeightMap.empty() && m_HeightMap[0].size() > 0;
    }

   private:
    CMeshComponent* m_TerrainMesh = nullptr;

    // Terrain properties
    int m_GridWidth = 100;
    int m_GridHeight = 100;
    float m_GridSpacing = 1.0f;
    float m_HeightScale = 0.5f;

    // Height map data for collision
    std::vector<std::vector<float>> m_HeightMap;

    // Terrain bounds
    float m_TerrainOffsetX = 0.0f;
    float m_TerrainOffsetZ = 0.0f;

    // Generate procedural terrain mesh with height variation
    void GenerateTerrainMesh();
  };