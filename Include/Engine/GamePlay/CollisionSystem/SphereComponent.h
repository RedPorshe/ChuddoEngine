#pragma once

#include "Engine/GamePlay/CollisionSystem/CollisionComponent.h"

namespace CE
{
    class CSphereComponent : public CCollisionComponent
    {
    public:
        CSphereComponent(CObject* Owner = nullptr, FString Name = "SphereComponent");
        virtual ~CSphereComponent() = default;

        void SetRadius(float InRadius) { m_Radius = InRadius; }
        float GetRadius() const { return m_Radius; }

        virtual ECollisionShape GetCollisionShape() const override { return ECollisionShape::Sphere; }
        virtual bool CheckCollision(const CCollisionComponent* other) const override;
        virtual Math::Vector3f GetBoundingBoxMin() const override;
        virtual Math::Vector3f GetBoundingBoxMax() const override;

    protected:
        float m_Radius = 1.0f;
    };
}  // namespace CE
