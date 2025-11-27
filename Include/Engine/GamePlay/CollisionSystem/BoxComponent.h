#pragma once

#include "Engine/GamePlay/CollisionSystem/CollisionComponent.h"

namespace CE
{
    class CBoxComponent : public CCollisionComponent
    {
public:
CBoxComponent(CObject* Owner = nullptr, FString Name = "BoxComponent");
virtual ~CBoxComponent() = default;
 void SetExtents(const glm::vec3& Extents) { m_Extents = Extents; }
virtual ECollisionShape GetCollisionShape() const override { return ECollisionShape::Box;}
virtual bool CheckCollision(const CCollisionComponent* other) const override;
 virtual glm::vec3 GetBoundingBoxMin() const override;
 virtual glm::vec3 GetBoundingBoxMax() const override;

 protected:
 glm::vec3 m_Extents = glm::vec3(1.0f);

    }; 
}