// CapsuleComponent.h
#pragma once

#include "Engine/GamePlay/Components/CollisionComponent.h"

namespace CE
{
  class CEBoxComponent;
  class SphereComponent;
  class MeshCollisionComponent;

  class CECapsuleComponent : public CollisionComponent
  {
   public:
    CECapsuleComponent(CEObject* Owner = nullptr, FString NewName = "CapsuleComponent");
    virtual ~CECapsuleComponent() = default;

    void SetRadius(float Radius);
    void SetHeight(float Height);
    void SetSize(float Radius, float Height);

    float GetRadius() const
    {
      return m_Radius;
    }
    float GetHeight() const
    {
      return m_Height;
    }

    // CollisionComponent interface
    virtual ECollisionShape GetCollisionShape() const override
    {
      return ECollisionShape::Capsule;
    }
    virtual bool CheckCollisionWithBox(const CEBoxComponent* Other) const override;
    virtual bool CheckCollisionWithSphere(const SphereComponent* Other) const override;
    virtual bool CheckCollisionWithCapsule(const CECapsuleComponent* Other) const override;
    virtual bool CheckCollisionWithMesh(const MeshCollisionComponent* Other) const override;
    virtual glm::vec3 GetBoundingBoxMin() const override;
    virtual glm::vec3 GetBoundingBoxMax() const override;

    // Вспомогательные методы для коллизий
    glm::vec3 GetTopSphereCenter() const;
    glm::vec3 GetBottomSphereCenter() const;
    float GetCylinderHeight() const;

    // Методы для использования другими компонентами
    bool CheckCapsuleBoxCollision(const glm::vec3& capsuleBottom,
                                  const glm::vec3& capsuleTop,
                                  float capsuleRadius,
                                  const glm::vec3& boxMin,
                                  const glm::vec3& boxMax) const;

    glm::vec3 ClosestPointOnSegment(const glm::vec3& a,
                                    const glm::vec3& b,
                                    const glm::vec3& point) const;

    virtual void Update(float DeltaTime) override;

   private:
    // Вспомогательные методы для проверки коллизий
    bool CheckCapsuleCapsuleCollision(const CECapsuleComponent* Other) const;

    float DistanceBetweenSegments(const glm::vec3& a1,
                                  const glm::vec3& a2,
                                  const glm::vec3& b1,
                                  const glm::vec3& b2) const;

    float m_Radius = 50.0f;
    float m_Height = 200.0f;
  };
}  // namespace CE