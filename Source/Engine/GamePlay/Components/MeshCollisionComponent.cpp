// MeshCollisionComponent.cpp
#include "Engine/GamePlay/Components/MeshCollisionComponent.h"

#include <algorithm>

#include "Engine/GamePlay/Components/BoxComponent.h"
#include "Engine/GamePlay/Components/CapsuleComponent.h"
#include "Engine/GamePlay/Components/SphereComponent.h"

namespace CE
{
  MeshCollisionComponent::MeshCollisionComponent(CEObject* Owner, FString NewName)
      : CollisionComponent(Owner, NewName)
  {
    CE_CORE_DEBUG("MeshCollisionComponent created: ", NewName);
  }

  void MeshCollisionComponent::SetCollisionMesh(const std::vector<Vertex>& Vertices, const std::vector<uint32_t>& Indices)
  {
    m_CollisionVertices = Vertices;
    m_CollisionIndices = Indices;
    UpdateBoundingBox();
  }

  bool MeshCollisionComponent::CheckCollision(const CollisionComponent* Other) const
  {
    if (!Other || !m_CollisionEnabled || !Other->IsCollisionEnabled())
      return false;

    // Если используем упрощенную коллизию - проверяем только bounding boxes
    if (m_UseSimpleCollision)
    {
      glm::vec3 thisMin = GetBoundingBoxMin();
      glm::vec3 thisMax = GetBoundingBoxMax();
      glm::vec3 otherMin = Other->GetBoundingBoxMin();
      glm::vec3 otherMax = Other->GetBoundingBoxMax();

      return (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
             (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
             (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z);
    }

    // TODO: Реализовать точную проверку mesh-mesh коллизий
    // Пока используем bounding box для всех типов
    switch (Other->GetCollisionShape())
    {
      case ECollisionShape::Box:
      {
        auto* otherBox = static_cast<const CEBoxComponent*>(Other);
        glm::vec3 thisMin = GetBoundingBoxMin();
        glm::vec3 thisMax = GetBoundingBoxMax();
        glm::vec3 otherMin = otherBox->GetBoundingBoxMin();
        glm::vec3 otherMax = otherBox->GetBoundingBoxMax();

        return (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
               (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
               (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z);
      }

      case ECollisionShape::Sphere:
      {
        auto* otherSphere = static_cast<const SphereComponent*>(Other);
        // Упрощенная проверка сферы с bounding box меша
        glm::vec3 spherePos = otherSphere->GetWorldLocation();
        float sphereRadius = otherSphere->GetRadius();
        glm::vec3 boxMin = GetBoundingBoxMin();
        glm::vec3 boxMax = GetBoundingBoxMax();

        // Находим ближайшую точку на bounding box к сфере
        glm::vec3 closestPoint;
        closestPoint.x = glm::clamp(spherePos.x, boxMin.x, boxMax.x);
        closestPoint.y = glm::clamp(spherePos.y, boxMin.y, boxMax.y);
        closestPoint.z = glm::clamp(spherePos.z, boxMin.z, boxMax.z);

        float distance = glm::distance(spherePos, closestPoint);
        return distance <= sphereRadius;
      }

      case ECollisionShape::Capsule:
      {
        auto* otherCapsule = static_cast<const CECapsuleComponent*>(Other);
        // Упрощенная проверка капсулы с bounding box меша
        glm::vec3 capsulePos = otherCapsule->GetWorldLocation();
        float capsuleRadius = otherCapsule->GetRadius();
        glm::vec3 boxMin = GetBoundingBoxMin();
        glm::vec3 boxMax = GetBoundingBoxMax();

        glm::vec3 closestPoint;
        closestPoint.x = glm::clamp(capsulePos.x, boxMin.x, boxMax.x);
        closestPoint.y = glm::clamp(capsulePos.y, boxMin.y, boxMax.y);
        closestPoint.z = glm::clamp(capsulePos.z, boxMin.z, boxMax.z);

        float distance = glm::distance(capsulePos, closestPoint);
        return distance <= capsuleRadius;
      }

      default:
        return false;
    }
  }

  glm::vec3 MeshCollisionComponent::GetBoundingBoxMin() const
  {
    if (m_UseSimpleCollision && m_CollisionVertices.empty())
    {
      // Если нет вершин, возвращаем bounding box вокруг позиции компонента
      glm::vec3 worldPos = GetWorldLocation();
      return worldPos - glm::vec3(1.0f);
    }
    return m_BoundingBoxMin;
  }

  glm::vec3 MeshCollisionComponent::GetBoundingBoxMax() const
  {
    if (m_UseSimpleCollision && m_CollisionVertices.empty())
    {
      glm::vec3 worldPos = GetWorldLocation();
      return worldPos + glm::vec3(1.0f);
    }
    return m_BoundingBoxMax;
  }

  void MeshCollisionComponent::UpdateBoundingBox()
  {
    if (m_CollisionVertices.empty())
    {
      m_BoundingBoxMin = glm::vec3(0.0f);
      m_BoundingBoxMax = glm::vec3(0.0f);
      return;
    }

    m_BoundingBoxMin = glm::vec3(std::numeric_limits<float>::max());
    m_BoundingBoxMax = glm::vec3(std::numeric_limits<float>::lowest());

    // Применяем мировую трансформацию к bounding box
    glm::mat4 worldTransform = GetWorldTransform();

    for (const auto& vertex : m_CollisionVertices)
    {
      glm::vec3 worldPos = worldTransform * glm::vec4(vertex.position, 1.0f);

      m_BoundingBoxMin.x = std::min(m_BoundingBoxMin.x, worldPos.x);
      m_BoundingBoxMin.y = std::min(m_BoundingBoxMin.y, worldPos.y);
      m_BoundingBoxMin.z = std::min(m_BoundingBoxMin.z, worldPos.z);

      m_BoundingBoxMax.x = std::max(m_BoundingBoxMax.x, worldPos.x);
      m_BoundingBoxMax.y = std::max(m_BoundingBoxMax.y, worldPos.y);
      m_BoundingBoxMax.z = std::max(m_BoundingBoxMax.z, worldPos.z);
    }
  }

  void MeshCollisionComponent::Update(float DeltaTime)
  {
    CollisionComponent::Update(DeltaTime);

    // Обновляем bounding box если трансформация изменилась
    static glm::mat4 lastTransform = GetWorldTransform();
    glm::mat4 currentTransform = GetWorldTransform();

    if (lastTransform != currentTransform)
    {
      UpdateBoundingBox();
      lastTransform = currentTransform;
    }
  }
}  // namespace CE