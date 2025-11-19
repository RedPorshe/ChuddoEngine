// SphereComponent.cpp
#include "Engine/GamePlay/Components/SphereComponent.h"

#include "Engine/GamePlay/Components/BoxComponent.h"
#include "Engine/GamePlay/Components/CapsuleComponent.h"
#include "Engine/GamePlay/Components/MeshCollisionComponent.h"

namespace CE
{
  SphereComponent::SphereComponent(CEObject* Owner, FString NewName)
      : CollisionComponent(Owner, NewName)
  {
    CE_CORE_DEBUG("SphereComponent created: ", NewName);
  }

  void SphereComponent::SetRadius(float Radius)
  {
    m_Radius = glm::max(Radius, 0.0f);
  }

  bool SphereComponent::CheckCollisionWithBox(const CEBoxComponent* Other) const
  {
    return Other->CheckBoxSphereCollision(this);
  }

  bool SphereComponent::CheckCollisionWithSphere(const SphereComponent* Other) const
  {
    return CheckSphereSphereCollision(Other);
  }

  bool SphereComponent::CheckCollisionWithCapsule(const CECapsuleComponent* Other) const
  {
    return CheckSphereCapsuleCollision(Other);
  }

  bool SphereComponent::CheckCollisionWithMesh(const MeshCollisionComponent* Other) const
  {
    // Упрощенная проверка сферы с bounding box меша
    glm::vec3 spherePos = GetWorldLocation();
    float sphereRadius = GetRadius();
    glm::vec3 boxMin = Other->GetBoundingBoxMin();
    glm::vec3 boxMax = Other->GetBoundingBoxMax();

    // Находим ближайшую точку на bounding box к сфере
    glm::vec3 closestPoint;
    closestPoint.x = glm::clamp(spherePos.x, boxMin.x, boxMax.x);
    closestPoint.y = glm::clamp(spherePos.y, boxMin.y, boxMax.y);
    closestPoint.z = glm::clamp(spherePos.z, boxMin.z, boxMax.z);

    float distance = glm::distance(spherePos, closestPoint);
    return distance <= sphereRadius;
  }

  bool SphereComponent::CheckSphereSphereCollision(const SphereComponent* Other) const
  {
    glm::vec3 thisPos = GetWorldLocation();
    glm::vec3 otherPos = Other->GetWorldLocation();
    float thisRadius = GetRadius();
    float otherRadius = Other->GetRadius();

    float distance = glm::distance(thisPos, otherPos);
    return distance <= (thisRadius + otherRadius);
  }

  bool SphereComponent::CheckSphereCapsuleCollision(const CECapsuleComponent* Other) const
  {
    glm::vec3 spherePos = GetWorldLocation();
    float sphereRadius = GetRadius();

    glm::vec3 capsuleBottom = Other->GetBottomSphereCenter();
    glm::vec3 capsuleTop = Other->GetTopSphereCenter();
    float capsuleRadius = Other->GetRadius();

    // Находим ближайшую точку на сегменте капсулы
    glm::vec3 closestPoint = Other->ClosestPointOnSegment(capsuleBottom, capsuleTop, spherePos);

    float distance = glm::distance(spherePos, closestPoint);
    return distance <= (sphereRadius + capsuleRadius);
  }

  glm::vec3 SphereComponent::GetBoundingBoxMin() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    return worldPos - glm::vec3(m_Radius);
  }

  glm::vec3 SphereComponent::GetBoundingBoxMax() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    return worldPos + glm::vec3(m_Radius);
  }

  void SphereComponent::Update(float DeltaTime)
  {
    CollisionComponent::Update(DeltaTime);
  }
}  // namespace CE