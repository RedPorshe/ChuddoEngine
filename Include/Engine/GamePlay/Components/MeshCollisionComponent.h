// MeshCollisionComponent.h
#pragma once

#include "Engine/Core/Rendering/Data/Vertex.h"
#include "Engine/GamePlay/Components/CollisionComponent.h"

namespace CE
{
  class MeshCollisionComponent : public CollisionComponent
  {
   public:
    MeshCollisionComponent(CEObject* Owner = nullptr, FString NewName = "MeshCollisionComponent");
    virtual ~MeshCollisionComponent() = default;

    void SetCollisionMesh(const std::vector<Vertex>& Vertices, const std::vector<uint32_t>& Indices);
    void SetUseSimpleCollision(bool UseSimple)
    {
      m_UseSimpleCollision = UseSimple;
    }

    // CollisionComponent interface
    virtual ECollisionShape GetCollisionShape() const override
    {
      return ECollisionShape::Mesh;
    }
    virtual bool CheckCollision(const CollisionComponent* Other) const override;
    virtual glm::vec3 GetBoundingBoxMin() const override;
    virtual glm::vec3 GetBoundingBoxMax() const override;

    virtual void Update(float DeltaTime) override;

   private:
    std::vector<Vertex> m_CollisionVertices;
    std::vector<uint32_t> m_CollisionIndices;
    bool m_UseSimpleCollision = true;  // Использовать упрощенную коллизию (bounding box)

    glm::vec3 m_BoundingBoxMin;
    glm::vec3 m_BoundingBoxMax;

    void UpdateBoundingBox();
  };
}  // namespace CE