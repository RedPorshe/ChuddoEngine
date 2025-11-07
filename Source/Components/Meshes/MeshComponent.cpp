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
    // Вершины куба (позиция, нормаль, UV)
    std::vector<Vertex> vertices = {
        // Передняя грань
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},

        // Задняя грань
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)}};

    // Индексы куба
    std::vector<uint32_t> indices = {
        // Передняя грань
        0, 1, 2, 2, 3, 0,
        // Задняя грань
        4, 5, 6, 6, 7, 4,
        // Верхняя грань
        3, 2, 6, 6, 7, 3,
        // Нижняя грань
        0, 1, 5, 5, 4, 0,
        // Правая грань
        1, 5, 6, 6, 2, 1,
        // Левая грань
        0, 4, 7, 7, 3, 0};

    m_Mesh.vertices = vertices;
    m_Mesh.indices = indices;
    m_Mesh.color = glm::vec3(1.0f, 0.5f, 0.2f);  // Оранжевый по умолчанию

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