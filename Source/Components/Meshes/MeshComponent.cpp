#include "Components/Meshes/MeshComponent.h"

namespace CE
{
  MeshComponent::MeshComponent(CEObject* Owner, FString NewName)
      : SceneComponent(Owner, NewName)
  {
    // Создаем тестовый куб по умолчанию
    CreateCubeMesh();
  }

  void MeshComponent::SetMesh(const std::string& MeshPath)
  {
    m_MeshPath = MeshPath;
    // TODO: Загрузка меша из файла
    // Пока используем тестовый куб
    CreateCubeMesh();
  }

  void MeshComponent::SetMaterial(const std::string& MaterialPath)
  {
    m_MaterialPath = MaterialPath;
    // TODO: Загрузка материала
  }

  void MeshComponent::CreateCubeMesh()
  {
    // Вершины куба (позиция, нормаль, цвет, UV)
    std::vector<Vertex> vertices = {
        // Передняя грань (Z+)
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 1.0f)},

        // Задняя грань (Z-)
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 1.0f)},

        // Верхняя грань (Y+)
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 1.0f)},

        // Нижняя грань (Y-)
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 1.0f)},

        // Правая грань (X+)
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 1.0f)},

        // Левая грань (X-)
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f), glm::vec2(0.0f, 1.0f)}};

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
    m_Mesh.color = glm::vec3(.0f, 1.0f, 0.0f);  // Оранжевый по умолчанию

    CE_CORE_DEBUG("Created cube mesh for: ", GetName());
  }

  glm::mat4 MeshComponent::GetRenderTransform() const
  {
    // Возвращаем мировую трансформацию для рендеринга
    return GetWorldTransform();
  }

  void MeshComponent::UpdateMeshTransform()
  {
    // Обновляем трансформацию в меше для рендеринга
    m_Mesh.transform = GetWorldTransform();
  }

  void MeshComponent::Update(float DeltaTime)
  {
    SceneComponent::Update(DeltaTime);

    // Обновляем трансформацию меша для рендеринга
    UpdateMeshTransform();
  }
}  // namespace CE