#pragma once
#include "Engine/Core/Rendering/Data/Vertex.h"
#include "Engine/GamePlay/Components/SceneComponent.h"

// Forward declaration для ObjLoader
namespace CE
{
  class ObjLoader;
}

namespace CE
{
  class MeshComponent : public SceneComponent
  {
   public:
    MeshComponent(CEObject* Owner = nullptr, FString NewName = "MeshComponent");
    virtual ~MeshComponent() = default;

    virtual void SetMesh(const std::string& MeshPath);
    virtual void SetMaterial(const std::string& MaterialPath);

    // Для ручного создания меша (пока заглушка для куба)
    void CreateCubeMesh();

    // Установка статического меша напрямую
    void SetStaticMesh(const StaticMesh& Mesh)
    {
      m_Mesh = Mesh;
    }

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
    void SetColor(const glm::vec4& color)
    {
      m_Mesh.color = glm::vec3(color);
    }
    const glm::vec3& GetColor() const
    {
      return m_Mesh.color;
    }

    virtual void Update(float DeltaTime) override;

   protected:
    std::string m_MeshPath;
    std::string m_MaterialPath;
    StaticMesh m_Mesh;

    void UpdateMeshTransform();
  };
}  // namespace CE