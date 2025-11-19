#pragma once

#include "Engine/Core/Rendering/Data/Vertex.h"
#include "Engine/GamePlay/Components/CollisionComponent.h"

namespace CE
{
  class CEBoxComponent;
  class SphereComponent;
  class CECapsuleComponent;
  class MeshCollisionComponent;

  class PlaneComponent : public CollisionComponent
  {
   public:
    PlaneComponent(CEObject* Owner = nullptr, FString NewName = "PlaneComponent");
    virtual ~PlaneComponent() = default;

    // CollisionComponent interface
    virtual ECollisionShape GetCollisionShape() const override
    {
      return ECollisionShape::Mesh;
    }
    virtual bool CheckCollisionWithBox(const CEBoxComponent* Other) const override;
    virtual bool CheckCollisionWithSphere(const SphereComponent* Other) const override;
    virtual bool CheckCollisionWithCapsule(const CECapsuleComponent* Other) const override;
    virtual bool CheckCollisionWithMesh(const MeshCollisionComponent* Other) const override;
    virtual bool CheckCollision(const CollisionComponent* Other) const override;
    virtual glm::vec3 GetBoundingBoxMin() const override;
    virtual glm::vec3 GetBoundingBoxMax() const override;

    // Настройки плоскости
    void SetSize(float Width, float Height);
    void SetSubdivisions(int Subdivisions);

    // Получение данных для рендеринга
    const StaticMesh& GetMeshData() const
    {
      return m_Mesh;
    }
    glm::mat4 GetRenderTransform() const;

    virtual void Update(float DeltaTime) override;

   private:
    void CreatePlaneMesh();

    StaticMesh m_Mesh;
    float m_Width = 10.0f;
    float m_Height = 10.0f;
    int m_Subdivisions = 1;
  };
}  // namespace CE