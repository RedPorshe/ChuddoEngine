#pragma once
#include "Core/Rendering/Data/Vertex.h"
#include "GamePlay/Components/Scene/SceneComponent.h"

namespace CE
{
  class MeshComponent : public SceneComponent
  {
   public:
    MeshComponent(CEObject* Owner = nullptr, FString NewName = "MeshComponent");
    virtual ~MeshComponent() = default;

    void SetMesh(const std::string& MeshPath);
    void SetMaterial(const std::string& MaterialPath);

    // Для ручного создания меша (пока заглушка для куба)
    void CreateCubeMesh();

    // Получение данных для рендеринга
    const StaticMesh& GetMeshData() const
    {
      return m_Mesh;
    }
    glm::mat4 GetRenderTransform() const;

    // Цвет материала (временно)
    void SetColor(const glm::vec3& color)
    {
      m_Mesh.color = color;
    }
    const glm::vec3& GetColor() const
    {
      return m_Mesh.color;
    }

    virtual void Update(float DeltaTime) override;

   private:
    std::string m_MeshPath;
    std::string m_MaterialPath;
    StaticMesh m_Mesh;

    void UpdateMeshTransform();
  };
}  // namespace CE