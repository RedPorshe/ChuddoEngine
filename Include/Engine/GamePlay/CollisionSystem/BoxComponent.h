#pragma once

#include "Engine/GamePlay/CollisionSystem/CollisionComponent.h"

namespace CE
{
    class CBoxComponent : public CCollisionComponent
    {
public:
CBoxComponent(CObject* Owner = nullptr, FString Name = "BoxComponent");
virtual ~CBoxComponent() = default;
 void SetExtents(const Math::Vector3f& Extents) { m_Extents = Extents; }
virtual ECollisionShape GetCollisionShape() const override { return ECollisionShape::Box;}
virtual bool CheckCollision(const CCollisionComponent* other) const override;
 virtual Math::Vector3f GetBoundingBoxMin() const override;
 virtual Math::Vector3f GetBoundingBoxMax() const override;

 protected:
 Math::Vector3f m_Extents = Math::Vector3f(1.0f);

    }; 
}