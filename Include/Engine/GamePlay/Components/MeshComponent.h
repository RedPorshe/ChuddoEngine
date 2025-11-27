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
  class CMeshComponent : public CSceneComponent
  {
   public:
    CMeshComponent(CObject* Owner = nullptr, FString NewName = "MeshComponent");
    virtual ~CMeshComponent() = default;

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
    Math::Matrix4f GetRenderTransform() const;

    // Цвет материала (временно)
    void SetColor(const Math::Vector3f& color)
    {
      m_Mesh.color = color;
    }
    void SetColor(const Math::Vector4f& color)
    {
      m_Mesh.color = color.XYZ();
    }
    const Math::Vector3f& GetColor() const
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