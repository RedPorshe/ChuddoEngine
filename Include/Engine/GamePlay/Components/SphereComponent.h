// SphereComponent.h
#pragma once

#include "Engine/GamePlay/Components/CollisionComponent.h"

namespace CE
{
  class CEBoxComponent;
  class CECapsuleComponent;
  class MeshCollisionComponent;

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
    virtual bool CheckCollisionWithBox(const CEBoxComponent* Other) const override;
    virtual bool CheckCollisionWithSphere(const SphereComponent* Other) const override;
    virtual bool CheckCollisionWithCapsule(const CECapsuleComponent* Other) const override;
    virtual bool CheckCollisionWithMesh(const MeshCollisionComponent* Other) const override;
    virtual glm::vec3 GetBoundingBoxMin() const override;
    virtual glm::vec3 GetBoundingBoxMax() const override;

    virtual void Update(float DeltaTime) override;

   private:
    // Вспомогательные методы для точных проверок
    bool CheckSphereSphereCollision(const SphereComponent* Other) const;
    bool CheckSphereCapsuleCollision(const CECapsuleComponent* Other) const;
    friend class CEBoxComponent;
    friend class CECapsuleComponent;
    friend class MeshCollisionComponent;

    float m_Radius = 50.0f;
  };
}  // namespace CE