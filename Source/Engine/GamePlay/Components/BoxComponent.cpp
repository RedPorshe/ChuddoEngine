// BoxComponent.cpp
#include "Engine/GamePlay/Components/BoxComponent.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Engine/GamePlay/Components/CapsuleComponent.h"
#include "Engine/GamePlay/Components/SphereComponent.h"

namespace CE
{
  CEBoxComponent::CEBoxComponent(CEObject* Owner, FString NewName)
      : CollisionComponent(Owner, NewName)  // Изменили на CollisionComponent
  {
    CE_CORE_DEBUG("BoxComponent created: ", NewName);
  }

  void CEBoxComponent::SetExtents(const glm::vec3& Extents)
  {
    m_Extents = glm::max(Extents, glm::vec3(0.0f));
  }

  void CEBoxComponent::SetExtents(float X, float Y, float Z)
  {
    SetExtents(glm::vec3(X, Y, Z));
  }

  void CEBoxComponent::SetSize(const glm::vec3& Size)
  {
    SetExtents(Size * 0.5f);
  }

  bool CEBoxComponent::CheckCollision(const CollisionComponent* Other) const
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

  bool CEBoxComponent::CheckPointCollision(const glm::vec3& Point) const
  {
    glm::vec3 min = GetBoundingBoxMin();
    glm::vec3 max = GetBoundingBoxMax();

    return (Point.x >= min.x && Point.x <= max.x) &&
           (Point.y >= min.y && Point.y <= max.y) &&
           (Point.z >= min.z && Point.z <= max.z);
  }

  glm::vec3 CEBoxComponent::GetBoundingBoxMin() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    return worldPos - m_Extents;
  }

  glm::vec3 CEBoxComponent::GetBoundingBoxMax() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    return worldPos + m_Extents;
  }

  std::vector<glm::vec3> CEBoxComponent::GetVertices() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    std::vector<glm::vec3> vertices;

    // Все 8 вершин бокса
    for (int x = -1; x <= 1; x += 2)
    {
      for (int y = -1; y <= 1; y += 2)
      {
        for (int z = -1; z <= 1; z += 2)
        {
          vertices.push_back(worldPos + m_Extents * glm::vec3(x, y, z));
        }
      }
    }

    return vertices;
  }

  void CEBoxComponent::Update(float DeltaTime)
  {
    CollisionComponent::Update(DeltaTime);
  }
}  // namespace CE