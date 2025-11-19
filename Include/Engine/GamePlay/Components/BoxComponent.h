// BoxComponent.h
#pragma once

#include "Engine/GamePlay/Components/CollisionComponent.h"

namespace CE
{
  class SphereComponent;
  class CECapsuleComponent;
  class MeshCollisionComponent;

  class CEBoxComponent : public CollisionComponent
  {
   public:
    CEBoxComponent(CEObject* Owner = nullptr, FString NewName = "BoxComponent");
    virtual ~CEBoxComponent() = default;

    void SetExtents(const glm::vec3& Extents);
    void SetExtents(float X, float Y, float Z);
    void SetSize(const glm::vec3& Size);

    const glm::vec3& GetExtents() const
    {
      return m_Extents;
    }
    glm::vec3 GetSize() const
    {
      return m_Extents * 2.0f;
    }

    // CollisionComponent interface
    virtual ECollisionShape GetCollisionShape() const override
    {
      return ECollisionShape::Box;
    }
    virtual bool CheckCollisionWithBox(const CEBoxComponent* Other) const override;
    virtual bool CheckCollisionWithSphere(const SphereComponent* Other) const override;
    virtual bool CheckCollisionWithCapsule(const CECapsuleComponent* Other) const override;
    virtual bool CheckCollisionWithMesh(const MeshCollisionComponent* Other) const override;
    virtual glm::vec3 GetBoundingBoxMin() const override;
    virtual glm::vec3 GetBoundingBoxMax() const override;

    bool CheckPointCollision(const glm::vec3& Point) const;
    std::vector<glm::vec3> GetVertices() const;

    virtual void Update(float DeltaTime) override;

   private:
    // Вспомогательные методы для точных проверок
    bool CheckBoxBoxCollision(const CEBoxComponent* Other) const;
    bool CheckBoxSphereCollision(const SphereComponent* Other) const;
    bool CheckBoxCapsuleCollision(const CECapsuleComponent* Other) const;

    glm::vec3 m_Extents = glm::vec3(50.0f, 50.0f, 50.0f);

    friend class SphereComponent;
    friend class CECapsuleComponent;
    friend class MeshCollisionComponent;
    };
}  // namespace CE