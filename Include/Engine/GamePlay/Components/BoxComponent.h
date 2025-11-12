// BoxComponent.h
#pragma once

#include "Engine/GamePlay/Components/CollisionComponent.h"

namespace CE
{
  class CECapsuleComponent;
  class SphereComponent;

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
    virtual bool CheckCollision(const CollisionComponent* Other) const override;
    virtual glm::vec3 GetBoundingBoxMin() const override;
    virtual glm::vec3 GetBoundingBoxMax() const override;

    bool CheckPointCollision(const glm::vec3& Point) const;
    std::vector<glm::vec3> GetVertices() const;

    virtual void Update(float DeltaTime) override;

   private:
    glm::vec3 m_Extents = glm::vec3(50.0f, 50.0f, 50.0f);
  };
}  // namespace CE