#include "Engine/GamePlay/Components/MeshComponent.h"

#include "Engine/Core/Utilities/ObjLoader.h"


  CMeshComponent::CMeshComponent(CObject* Owner, FString NewName)
      : CSceneComponent(Owner, NewName)
  {
    m_Mesh.vertices.clear();
    m_Mesh.indices.clear();
  }

  void CMeshComponent::SetMesh(const std::string& MeshPath)
  {
    m_MeshPath = MeshPath;
    // По умолчанию не создаем куб, только если путь не пустой
    if (!MeshPath.empty())
    {
      // Пытаемся загрузить OBJ файл
      FStaticMesh loadedMesh = ObjLoader::LoadOBJ(MeshPath);

      // Если загрузка успешна (есть вершины), используем загруженный меш
      if (!loadedMesh.vertices.empty() && !loadedMesh.indices.empty())
      {
        m_Mesh = loadedMesh;
      }
      else
      {
        // Если загрузка не удалась, используем куб как fallback
        CORE_WARN("Failed to load mesh from ", MeshPath, ", using default cube");
        CreateCubeMesh();
      }
    }
    else
    {
      // Если путь пустой - очищаем меш
      m_Mesh.vertices.clear();
      m_Mesh.indices.clear();
    }
  }

  void CMeshComponent::SetMaterial(const std::string& MaterialPath)
  {
    m_MaterialPath = MaterialPath;
  }

  void CMeshComponent::CreateCubeMesh()
  {
    // Вершины куба (позиция, нормаль, цвет (белый), UV)
    std::vector<Vertex> vertices = {
        // Передняя грань (Z+)
        {CEMath::Vector3f(-0.5f, -0.5f, 0.5f), CEMath::Vector3f(0.0f, 0.0f, 1.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(0.0f, 0.0f)},
        {CEMath::Vector3f(0.5f, -0.5f, 0.5f), CEMath::Vector3f(0.0f, 0.0f, 1.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(1.0f, 0.0f)},
        {CEMath::Vector3f(0.5f, 0.5f, 0.5f), CEMath::Vector3f(0.0f, 0.0f, 1.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(1.0f, 1.0f)},
        {CEMath::Vector3f(-0.5f, 0.5f, 0.5f), CEMath::Vector3f(0.0f, 0.0f, 1.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(0.0f, 1.0f)},

        // Задняя грань (Z-)
        {CEMath::Vector3f(0.5f, -0.5f, -0.5f), CEMath::Vector3f(0.0f, 0.0f, -1.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(0.0f, 0.0f)},
        {CEMath::Vector3f(-0.5f, -0.5f, -0.5f), CEMath::Vector3f(0.0f, 0.0f, -1.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(1.0f, 0.0f)},
        {CEMath::Vector3f(-0.5f, 0.5f, -0.5f), CEMath::Vector3f(0.0f, 0.0f, -1.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(1.0f, 1.0f)},
        {CEMath::Vector3f(0.5f, 0.5f, -0.5f), CEMath::Vector3f(0.0f, 0.0f, -1.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(0.0f, 1.0f)},

        // Верхняя грань (Y+)
        {CEMath::Vector3f(-0.5f, 0.5f, 0.5f), CEMath::Vector3f(0.0f, 1.0f, 0.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(0.0f, 0.0f)},
        {CEMath::Vector3f(0.5f, 0.5f, 0.5f), CEMath::Vector3f(0.0f, 1.0f, 0.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(1.0f, 0.0f)},
        {CEMath::Vector3f(0.5f, 0.5f, -0.5f), CEMath::Vector3f(0.0f, 1.0f, 0.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(1.0f, 1.0f)},
        {CEMath::Vector3f(-0.5f, 0.5f, -0.5f), CEMath::Vector3f(0.0f, 1.0f, 0.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(0.0f, 1.0f)},

        // Нижняя грань (Y-)
        {CEMath::Vector3f(-0.5f, -0.5f, -0.5f), CEMath::Vector3f(0.0f, -1.0f, 0.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(0.0f, 0.0f)},
        {CEMath::Vector3f(0.5f, -0.5f, -0.5f), CEMath::Vector3f(0.0f, -1.0f, 0.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(1.0f, 0.0f)},
        {CEMath::Vector3f(0.5f, -0.5f, 0.5f), CEMath::Vector3f(0.0f, -1.0f, 0.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(1.0f, 1.0f)},
        {CEMath::Vector3f(-0.5f, -0.5f, 0.5f), CEMath::Vector3f(0.0f, -1.0f, 0.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(0.0f, 1.0f)},

        // Правая грань (X+)
        {CEMath::Vector3f(0.5f, -0.5f, 0.5f), CEMath::Vector3f(1.0f, 0.0f, 0.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(0.0f, 0.0f)},
        {CEMath::Vector3f(0.5f, -0.5f, -0.5f), CEMath::Vector3f(1.0f, 0.0f, 0.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(1.0f, 0.0f)},
        {CEMath::Vector3f(0.5f, 0.5f, -0.5f), CEMath::Vector3f(1.0f, 0.0f, 0.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(1.0f, 1.0f)},
        {CEMath::Vector3f(0.5f, 0.5f, 0.5f), CEMath::Vector3f(1.0f, 0.0f, 0.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(0.0f, 1.0f)},

        // Левая грань (X-)
        {CEMath::Vector3f(-0.5f, -0.5f, -0.5f), CEMath::Vector3f(-1.0f, 0.0f, 0.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(0.0f, 0.0f)},
        {CEMath::Vector3f(-0.5f, -0.5f, 0.5f), CEMath::Vector3f(-1.0f, 0.0f, 0.0f), CEMath::Vector3f(1.0f), CEMath::Vector2f(1.0f, 0.0f)},
        {CEMath::Vector3f(-0.5f, 0.5f, 0.5f), CEMath::Vector3f(-1.0f, 0.0f, 0.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(1.0f, 1.0f)},
        {CEMath::Vector3f(-0.5f, 0.5f, -0.5f), CEMath::Vector3f(-1.0f, 0.0f, 0.0f), CEMath::Vector3f(0.f, 0.f, 1.0f), CEMath::Vector2f(0.0f, 1.0f)}};

    // Индексы куба (против часовой стрелки для лицевой стороны)
    std::vector<uint32_t> indices = {
        // Передняя грань
        0, 2, 1, 2, 0, 3,
        // Задняя грань
        4, 6, 5, 6, 4, 7,
        // Верхняя грань
        8, 10, 9, 10, 8, 11,
        // Нижняя грань
        12, 14, 13, 14, 12, 15,
        // Правая грань
        16, 18, 17, 18, 16, 19,
        // Левая грань
        20, 22, 21, 22, 20, 23};

    m_Mesh.vertices = vertices;
    m_Mesh.indices = indices;
    m_Mesh.color = CEMath::Vector3f(1.0f);
  }

  CEMath::Matrix4f CMeshComponent::GetRenderTransform() const
  {
    return GetWorldTransform();
  }

  void CMeshComponent::UpdateMeshTransform()
  {
    m_Mesh.transform = GetWorldTransform();
  }

  void CMeshComponent::Update(float DeltaTime)
  {
    CSceneComponent::Update(DeltaTime);

    UpdateMeshTransform();
  }