#pragma once

#include "Engine/GamePlay/CollisionSystem/CollisionComponent.h"

namespace CE
{
    class CTerrainCollisionComponent : public CCollisionComponent
    {
    public:
        CTerrainCollisionComponent(CObject* Owner = nullptr, FString Name = "TerrainCollisionComponent");
        virtual ~CTerrainCollisionComponent() = default;

        virtual ECollisionShape GetCollisionShape() const override { return ECollisionShape::Mesh; }
        virtual bool CheckCollision(const CCollisionComponent* other) const override;
        virtual Math::Vector3f GetBoundingBoxMin() const override;
        virtual Math::Vector3f GetBoundingBoxMax() const override;

        void SetHeightMap(const std::vector<std::vector<float>>& heightMap, float gridSpacing, float heightScale);
        float GetHeightAtPosition(const Math::Vector3f& localPosition) const;
        bool IsPointOnTerrain(const Math::Vector3f& worldPosition) const;

    private:
        std::vector<std::vector<float>> m_HeightMap;
        float m_GridSpacing = 1.0f;
        float m_HeightScale = 0.5f;
        int m_GridWidth = 0;
        int m_GridHeight = 0;
        float m_TerrainOffsetX = 0.0f;
        float m_TerrainOffsetZ = 0.0f;
    };
}
