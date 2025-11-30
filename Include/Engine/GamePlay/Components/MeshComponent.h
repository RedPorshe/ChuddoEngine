#pragma once
#include "Engine/Core/Rendering/Data/Vertex.h"
#include "Engine/GamePlay/Components/SceneComponent.h"


// Forward declaration для ObjLoader

  class ObjLoader;



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
    void SetStaticMesh(const FStaticMesh& Mesh)
    {
      m_Mesh = Mesh;
    }

    // Получение данных для рендеринга
    const FStaticMesh& GetMeshData() const
    {
      return m_Mesh;
    }
    CEMath::Matrix4f GetRenderTransform() const;

    // Цвет материала (временно)
    void SetColor(const CEMath::Vector3f& color)
    {
      m_Mesh.color = color;
    }
    void SetColor(const CEMath::Vector4f& color)
    {
      m_Mesh.color = color.XYZ();
    }
    const CEMath::Vector3f& GetColor() const
    {
      return m_Mesh.color;
    }

    virtual void Update(float DeltaTime) override;

   protected:
    std::string m_MeshPath;
    std::string m_MaterialPath;
    FStaticMesh m_Mesh;

    void UpdateMeshTransform();
  };