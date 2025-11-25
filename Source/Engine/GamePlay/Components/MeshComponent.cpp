#include "Engine/GamePlay/Components/MeshComponent.h"

#include "Engine/Core/Utilities/ObjLoader.h"

namespace CE
{
  MeshComponent::MeshComponent(CObject* Owner, FString NewName)
      : CSceneComponent(Owner, NewName)
  {
    m_Mesh.vertices.clear();
    m_Mesh.indices.clear();
  }

  void MeshComponent::SetMesh(const std::string& MeshPath)
  {
    m_MeshPath = MeshPath;
    // По умолчанию не создаем куб, только если путь не пустой
    if (!MeshPath.empty())
    {
      // Пытаемся загрузить OBJ файл
      StaticMesh loadedMesh = ObjLoader::LoadOBJ(MeshPath);

      // Если загрузка успешна (есть вершины), используем загруженный меш
      if (!loadedMesh.vertices.empty() && !loadedMesh.indices.empty())
      {
        m_Mesh = loadedMesh;
      }
      else
      {
        // Если загрузка не удалась, используем куб как fallback
        CE_WARN("Failed to load mesh from ", MeshPath, ", using default cube");
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

  void MeshComponent::SetMaterial(const std::string& MaterialPath)
  {
    m_MaterialPath = MaterialPath;
  }

  void MeshComponent::CreateCubeMesh()
  {
    // Вершины куба (позиция, нормаль, цвет (белый), UV)
    std::vector<Vertex> vertices = {
        // Передняя грань (Z+)
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(0.0f, 1.0f)},

        // Задняя грань (Z-)
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f)},

        // Верхняя грань (Y+)
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f)},

        // Нижняя грань (Y-)
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(0.0f, 1.0f)},

        // Правая грань (X+)
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f)},

        // Левая грань (X-)
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.f, 0.f, 1.0f), glm::vec2(0.0f, 1.0f)}};

    // Индексы куба (против часовой стрелки для лицевой стороны)
    std::vector<uint32_t> indices = {
        // Передняя грань
        0, 1, 2, 2, 3, 0,
        // Задняя грань
        4, 5, 6, 6, 7, 4,
        // Верхняя грань
        8, 9, 10, 10, 11, 8,
        // Нижняя грань
        12, 13, 14, 14, 15, 12,
        // Правая грань
        16, 17, 18, 18, 19, 16,
        // Левая грань
        20, 21, 22, 22, 23, 20};

    m_Mesh.vertices = vertices;
    m_Mesh.indices = indices;
    m_Mesh.color = glm::vec3(1.0f);
  }

  glm::mat4 MeshComponent::GetRenderTransform() const
  {
    return GetWorldTransform();
  }

  void MeshComponent::UpdateMeshTransform()
  {
    m_Mesh.transform = GetWorldTransform();
  }

  void MeshComponent::Update(float DeltaTime)
  {
    CSceneComponent::Update(DeltaTime);

    UpdateMeshTransform();
  }
}  // namespace CE