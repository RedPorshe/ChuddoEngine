#include "Engine/GamePlay/Components/PlaneComponent.h"

#include "Engine/GamePlay/Components/BoxComponent.h"
#include "Engine/GamePlay/Components/CapsuleComponent.h"
#include "Engine/GamePlay/Components/SphereComponent.h"

namespace CE
{
  PlaneComponent::PlaneComponent(CEObject* Owner, FString NewName)
      : CollisionComponent(Owner, NewName)
  {
    CreatePlaneMesh();
    CE_CORE_DEBUG("PlaneComponent created: ", NewName);
  }

  void PlaneComponent::SetSize(float Width, float Height)
  {
    m_Width = Width;
    m_Height = Height;
    CreatePlaneMesh();
  }

  void PlaneComponent::SetSubdivisions(int Subdivisions)
  {
    m_Subdivisions = glm::max(1, Subdivisions);
    CreatePlaneMesh();
  }

  void PlaneComponent::CreatePlaneMesh()
  {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float halfWidth = m_Width * 0.5f;
    float halfHeight = m_Height * 0.5f;

    // Создаем плоскость с подразделениями
    for (int z = 0; z <= m_Subdivisions; ++z)
    {
      for (int x = 0; x <= m_Subdivisions; ++x)
      {
        float u = (float)x / m_Subdivisions;
        float v = (float)z / m_Subdivisions;

        float posX = (u * m_Width) - halfWidth;
        float posZ = (v * m_Height) - halfHeight;

        vertices.push_back({glm::vec3(posX, 0.0f, posZ),
                            glm::vec3(0.0f, 1.0f, 0.0f),  // Нормаль вверх
                            glm::vec3(0.8f, 0.8f, 0.8f),  // Серый цвет
                            glm::vec2(u, v)});
      }
    }

    // Индексы для треугольников
    for (int z = 0; z < m_Subdivisions; ++z)
    {
      for (int x = 0; x < m_Subdivisions; ++x)
      {
        uint32_t topLeft = z * (m_Subdivisions + 1) + x;
        uint32_t topRight = topLeft + 1;
        uint32_t bottomLeft = (z + 1) * (m_Subdivisions + 1) + x;
        uint32_t bottomRight = bottomLeft + 1;

        // Первый треугольник
        indices.push_back(topLeft);
        indices.push_back(bottomLeft);
        indices.push_back(topRight);

        // Второй треугольник
        indices.push_back(topRight);
        indices.push_back(bottomLeft);
        indices.push_back(bottomRight);
      }
    }

    m_Mesh.vertices = vertices;
    m_Mesh.indices = indices;
    m_Mesh.color = glm::vec3(0.8f, 0.8f, 0.8f);
  }

  glm::mat4 PlaneComponent::GetRenderTransform() const
  {
    return GetWorldTransform();
  }

  bool PlaneComponent::CheckCollision(const CollisionComponent* Other) const
  {
    if (!Other || !m_CollisionEnabled || !Other->IsCollisionEnabled())
      return false;

    // УПРОЩЕННАЯ СИММЕТРИЧНАЯ ПРОВЕРКА
    glm::vec3 thisMin = GetBoundingBoxMin();
    glm::vec3 thisMax = GetBoundingBoxMax();
    glm::vec3 otherMin = Other->GetBoundingBoxMin();
    glm::vec3 otherMax = Other->GetBoundingBoxMax();

    return (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
           (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
           (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z);
  }

  glm::vec3 PlaneComponent::GetBoundingBoxMin() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    return worldPos - glm::vec3(m_Width * 0.5f, 0.0f, m_Height * 0.5f);
  }

  glm::vec3 PlaneComponent::GetBoundingBoxMax() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    return worldPos + glm::vec3(m_Width * 0.5f, 2.0f, m_Height * 0.5f);  // Увеличили высоту до 2м
  }

  void PlaneComponent::Update(float DeltaTime)
  {
    CollisionComponent::Update(DeltaTime);
  }
}  // namespace CE