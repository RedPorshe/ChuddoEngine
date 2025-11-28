#pragma once

#include "Engine/GamePlay/CollisionSystem/CollisionComponent.h"
#include "Engine/Utils/Math/Ray.h"


namespace CE
{
    class CCapsuleComponent : public CCollisionComponent
    {
    public:
        CCapsuleComponent(CObject* Owner = nullptr, FString Name = "CapsuleComponent");
        virtual ~CCapsuleComponent() = default;

        void SetRadius(float Radius) { m_Radius = Radius; }
        void SetHalfHeight(float HalfHeight) { m_HalfHeight = HalfHeight; }

        float GetRadius() const { return m_Radius; }
        float GetHalfHeight() const { return m_HalfHeight; }

        virtual ECollisionShape GetCollisionShape() const override { return ECollisionShape::Capsule; }
        virtual bool CheckCollision(const CCollisionComponent* other) const override;
        virtual Math::Vector3f GetBoundingBoxMin() const override;
        virtual Math::Vector3f GetBoundingBoxMax() const override;

        // Ray intersection for capsule
        bool IntersectsRay(const Math::Ray& ray, float& t) const;

    protected:
        float m_Radius = 0.5f;
        float m_HalfHeight = 1.0f;
    };
}
