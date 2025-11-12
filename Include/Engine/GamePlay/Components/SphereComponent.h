// SphereComponent.h
#pragma once

#include "Engine/GamePlay/Components/CollisionComponent.h"

namespace CE
{
  class SphereComponent : public CollisionComponent
  {
   public:
    SphereComponent(CEObject* Owner = nullptr, FString NewName = "SphereComponent");
    virtual ~SphereComponent() = default;

    void SetRadius(float Radius);
    float GetRadius() const
    {
      return m_Radius;
    }

    // CollisionComponent interface
    virtual ECollisionShape GetCollisionShape() const override
    {
      return ECollisionShape::Sphere;
    }
    virtual bool CheckCollision(const CollisionComponent* Other) const override;
    virtual glm::vec3 GetBoundingBoxMin() const override;
    virtual glm::vec3 GetBoundingBoxMax() const override;

    virtual void Update(float DeltaTime) override;

   private:
    float m_Radius = 50.0f;
  };
}  // namespace CE