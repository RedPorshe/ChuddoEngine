// BoxComponent.cpp
#include "Engine/GamePlay/Components/BoxComponent.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine/GamePlay/Components/CapsuleComponent.h"
#include "Engine/GamePlay/Components/MeshCollisionComponent.h"
#include "Engine/GamePlay/Components/SphereComponent.h"

namespace CE
{
  CEBoxComponent::CEBoxComponent(CEObject* Owner, FString NewName)
      : CollisionComponent(Owner, NewName)
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

  bool CEBoxComponent::CheckCollisionWithBox(const CEBoxComponent* Other) const
  {
    return CheckBoxBoxCollision(Other);
  }

  bool CEBoxComponent::CheckCollisionWithSphere(const SphereComponent* Other) const
  {
    return CheckBoxSphereCollision(Other);
  }

  bool CEBoxComponent::CheckCollisionWithCapsule(const CECapsuleComponent* Other) const
  {
    return CheckBoxCapsuleCollision(Other);
  }

  bool CEBoxComponent::CheckCollisionWithMesh(const MeshCollisionComponent* Other) const
  {
    // Для mesh используем AABB проверку
    glm::vec3 thisMin = GetBoundingBoxMin();
    glm::vec3 thisMax = GetBoundingBoxMax();
    glm::vec3 otherMin = Other->GetBoundingBoxMin();
    glm::vec3 otherMax = Other->GetBoundingBoxMax();

    return (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
           (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
           (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z);
  }

  bool CEBoxComponent::CheckBoxBoxCollision(const CEBoxComponent* Other) const
  {
    glm::vec3 thisCenter = GetWorldLocation();
    glm::vec3 otherCenter = Other->GetWorldLocation();

    glm::vec3 thisExtents = GetExtents();
    glm::vec3 otherExtents = Other->GetExtents();

    // Separating Axis Theorem для OBB
    glm::vec3 delta = otherCenter - thisCenter;

    // Получаем оси этого бокса
    glm::vec3 thisRight = GetRightVector();
    glm::vec3 thisUp = GetUpVector();
    glm::vec3 thisForward = GetForwardVector();

    // Получаем оси другого бокса
    glm::vec3 otherRight = Other->GetRightVector();
    glm::vec3 otherUp = Other->GetUpVector();
    glm::vec3 otherForward = Other->GetForwardVector();

    float rThis, rOther;

    // Проверка по 6 основным осям
    // Ось X этого бокса
    rThis = thisExtents.x;
    rOther = otherExtents.x * std::abs(glm::dot(thisRight, otherRight)) +
             otherExtents.y * std::abs(glm::dot(thisRight, otherUp)) +
             otherExtents.z * std::abs(glm::dot(thisRight, otherForward));
    if (std::abs(glm::dot(delta, thisRight)) > rThis + rOther)
      return false;

    // Ось Y этого бокса
    rThis = thisExtents.y;
    rOther = otherExtents.x * std::abs(glm::dot(thisUp, otherRight)) +
             otherExtents.y * std::abs(glm::dot(thisUp, otherUp)) +
             otherExtents.z * std::abs(glm::dot(thisUp, otherForward));
    if (std::abs(glm::dot(delta, thisUp)) > rThis + rOther)
      return false;

    // Ось Z этого бокса
    rThis = thisExtents.z;
    rOther = otherExtents.x * std::abs(glm::dot(thisForward, otherRight)) +
             otherExtents.y * std::abs(glm::dot(thisForward, otherUp)) +
             otherExtents.z * std::abs(glm::dot(thisForward, otherForward));
    if (std::abs(glm::dot(delta, thisForward)) > rThis + rOther)
      return false;

    // Ось X другого бокса
    rThis = thisExtents.x * std::abs(glm::dot(otherRight, thisRight)) +
            thisExtents.y * std::abs(glm::dot(otherRight, thisUp)) +
            thisExtents.z * std::abs(glm::dot(otherRight, thisForward));
    rOther = otherExtents.x;
    if (std::abs(glm::dot(delta, otherRight)) > rThis + rOther)
      return false;

    // Ось Y другого бокса
    rThis = thisExtents.x * std::abs(glm::dot(otherUp, thisRight)) +
            thisExtents.y * std::abs(glm::dot(otherUp, thisUp)) +
            thisExtents.z * std::abs(glm::dot(otherUp, thisForward));
    rOther = otherExtents.y;
    if (std::abs(glm::dot(delta, otherUp)) > rThis + rOther)
      return false;

    // Ось Z другого бокса
    rThis = thisExtents.x * std::abs(glm::dot(otherForward, thisRight)) +
            thisExtents.y * std::abs(glm::dot(otherForward, thisUp)) +
            thisExtents.z * std::abs(glm::dot(otherForward, thisForward));
    rOther = otherExtents.z;
    if (std::abs(glm::dot(delta, otherForward)) > rThis + rOther)
      return false;

    return true;
  }

  bool CEBoxComponent::CheckBoxSphereCollision(const SphereComponent* Other) const
  {
    glm::vec3 sphereCenter = Other->GetWorldLocation();
    float sphereRadius = Other->GetRadius();

    // Преобразуем точку в локальное пространство бокса
    glm::mat4 invTransform = glm::inverse(GetWorldTransform());
    glm::vec3 localSpherePos = invTransform * glm::vec4(sphereCenter, 1.0f);

    // Находим ближайшую точку на боксе к сфере
    glm::vec3 closestPoint;
    closestPoint.x = glm::clamp(localSpherePos.x, -m_Extents.x, m_Extents.x);
    closestPoint.y = glm::clamp(localSpherePos.y, -m_Extents.y, m_Extents.y);
    closestPoint.z = glm::clamp(localSpherePos.z, -m_Extents.z, m_Extents.z);

    // Преобразуем обратно в мировые координаты
    closestPoint = GetWorldTransform() * glm::vec4(closestPoint, 1.0f);

    // Проверяем расстояние
    float distance = glm::distance(sphereCenter, closestPoint);
    return distance <= sphereRadius;
  }

  bool CEBoxComponent::CheckBoxCapsuleCollision(const CECapsuleComponent* Other) const
  {
    glm::vec3 capsuleBottom = Other->GetBottomSphereCenter();
    glm::vec3 capsuleTop = Other->GetTopSphereCenter();
    float capsuleRadius = Other->GetRadius();

    return Other->CheckCapsuleBoxCollision(capsuleBottom, capsuleTop, capsuleRadius,
                                           GetBoundingBoxMin(), GetBoundingBoxMax());
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