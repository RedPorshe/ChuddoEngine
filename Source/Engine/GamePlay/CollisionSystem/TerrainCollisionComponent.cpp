#include "Engine/GamePlay/CollisionSystem/TerrainCollisionComponent.h"
#include "Engine/Utils/Math/AABB.h"

namespace CE
{
    CTerrainCollisionComponent::CTerrainCollisionComponent(CObject* Owner, FString Name) : CCollisionComponent(Owner, Name)
    {
        SetCollisionProfileName("BlockAll");
    }

    bool CTerrainCollisionComponent::CheckCollision(const CCollisionComponent* other) const
    {
        if (this == other) return false;
        if (!other || !this->bIsCollisionEnabled || !other->IsCollisionEnabled()) return false;

        ECollisionResponse responseToOther = GetCollisionResponseToChannel(other->GetCollisionChannel());
        ECollisionResponse otherToThisResponse = other->GetCollisionResponseToChannel(this->m_CollisionChannel);

        if (responseToOther == ECollisionResponse::Ignore || otherToThisResponse == ECollisionResponse::Ignore) { return false; }

        // For terrain, we need to check if the other component intersects with the terrain surface
        Math::Vector3f otherMin = other->GetBoundingBoxMin();
        Math::Vector3f otherMax = other->GetBoundingBoxMax();

        // Check if the other component's bounding box intersects with terrain bounds
        Math::Vector3f terrainMin = GetBoundingBoxMin();
        Math::Vector3f terrainMax = GetBoundingBoxMax();

        if (otherMax.x < terrainMin.x || otherMin.x > terrainMax.x ||
            otherMax.z < terrainMin.z || otherMin.z > terrainMax.z) {
            return false; // No intersection in XZ plane
        }

        // Check Y intersection with terrain height
        float maxTerrainHeight = terrainMax.y;
        float minOtherHeight = otherMin.y;

        if (minOtherHeight > maxTerrainHeight) {
            return false; // Other component is completely above terrain
        }

        // For more precise collision, check if the bottom center of the other component is below terrain surface
        Math::Vector3f otherCenter = (otherMin + otherMax) * 0.5f;
        Math::Vector3f otherBottomCenter = Math::Vector3f(otherCenter.x, otherMin.y, otherCenter.z);

        if (IsPointOnTerrain(otherBottomCenter)) {
            Math::Vector3f localPos = otherBottomCenter - GetWorldLocation();
            float terrainHeight = GetHeightAtPosition(localPos);
            float worldTerrainHeight = GetWorldLocation().y + terrainHeight;

            // If the bottom of the other component is below terrain surface, it's colliding
            if (otherMin.y <= worldTerrainHeight) {
                // Debug logging
                CE_LOG("[TerrainCollisionCheck] Actor at (%.2f, %.2f, %.2f), terrain height: %.2f, collision detected",
                       otherBottomCenter.x, otherBottomCenter.y, otherBottomCenter.z, worldTerrainHeight);
                return (responseToOther == ECollisionResponse::Block) || (otherToThisResponse == ECollisionResponse::Block) ||
                       (responseToOther == ECollisionResponse::Overlap) || (otherToThisResponse == ECollisionResponse::Overlap);
            }
        }

        return false;
    }

    Math::Vector3f CTerrainCollisionComponent::GetBoundingBoxMin() const
    {
        Math::Vector3f location = this->GetWorldLocation();
        float minHeight = location.y;
        if (!m_HeightMap.empty()) {
            for (const auto& row : m_HeightMap) {
                for (float height : row) {
                    minHeight = std::min(minHeight, location.y + height);
                }
            }
        }
        return Math::Vector3f(location.x - m_TerrainOffsetX, minHeight, location.z - m_TerrainOffsetZ);
    }

    Math::Vector3f CTerrainCollisionComponent::GetBoundingBoxMax() const
    {
        Math::Vector3f location = this->GetWorldLocation();
        float maxHeight = location.y;
        if (!m_HeightMap.empty()) {
            for (const auto& row : m_HeightMap) {
                for (float height : row) {
                    maxHeight = std::max(maxHeight, location.y + height);
                }
            }
        }
        return Math::Vector3f(location.x + m_TerrainOffsetX, maxHeight, location.z + m_TerrainOffsetZ);
    }

    void CTerrainCollisionComponent::SetHeightMap(const std::vector<std::vector<float>>& heightMap, float gridSpacing, float heightScale)
    {
        m_HeightMap = heightMap;
        m_GridSpacing = gridSpacing;
        m_HeightScale = heightScale;
        m_GridWidth = heightMap.empty() ? 0 : heightMap[0].size() - 1;
        m_GridHeight = heightMap.size() - 1;
        m_TerrainOffsetX = (m_GridWidth * 0.5f) * m_GridSpacing;
        m_TerrainOffsetZ = (m_GridHeight * 0.5f) * m_GridSpacing;
    }

    float CTerrainCollisionComponent::GetHeightAtPosition(const Math::Vector3f& localPosition) const
    {
        if (m_HeightMap.empty() || m_HeightMap[0].empty())
            return 0.0f;

        // Convert local position to grid coordinates
        float gridX = (localPosition.x + m_TerrainOffsetX) / m_GridSpacing;
        float gridZ = (localPosition.z + m_TerrainOffsetZ) / m_GridSpacing;

        // Clamp to grid bounds
        if (gridX < 0.0f || gridX > static_cast<float>(m_GridWidth) ||
            gridZ < 0.0f || gridZ > static_cast<float>(m_GridHeight))
        {
            return 0.0f;
        }

        // Get integer grid coordinates
        int x0 = static_cast<int>(gridX);
        int z0 = static_cast<int>(gridZ);
        int x1 = x0 + 1;
        int z1 = z0 + 1;

        // Clamp indices
        if (x0 < 0) x0 = 0;
        if (x1 > m_GridWidth) x1 = m_GridWidth;
        if (z0 < 0) z0 = 0;
        if (z1 > m_GridHeight) z1 = m_GridHeight;

        if (z0 < 0 || z1 >= static_cast<int>(m_HeightMap.size()) ||
            x0 < 0 || x1 >= static_cast<int>(m_HeightMap[0].size()))
            return 0.0f;

        // Bilinear interpolation
        float fx = gridX - x0;
        float fz = gridZ - z0;

        float h00 = m_HeightMap[z0][x0];
        float h10 = m_HeightMap[z0][x1];
        float h01 = m_HeightMap[z1][x0];
        float h11 = m_HeightMap[z1][x1];

        float h0 = h00 * (1.0f - fx) + h10 * fx;
        float h1 = h01 * (1.0f - fx) + h11 * fx;
        return h0 * (1.0f - fz) + h1 * fz;
    }

    bool CTerrainCollisionComponent::IsPointOnTerrain(const Math::Vector3f& worldPosition) const
    {
        Math::Vector3f localPos = worldPosition - GetWorldLocation();
        float gridX = (localPos.x + m_TerrainOffsetX) / m_GridSpacing;
        float gridZ = (localPos.z + m_TerrainOffsetZ) / m_GridSpacing;

        return (gridX >= 0.0f && gridX <= static_cast<float>(m_GridWidth) &&
                gridZ >= 0.0f && gridZ <= static_cast<float>(m_GridHeight));
    }
}
