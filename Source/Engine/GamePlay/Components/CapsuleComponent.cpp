// CapsuleComponent.cpp
#include "Engine/GamePlay/Components/CapsuleComponent.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine/GamePlay/Components/BoxComponent.h"
#include "Engine/GamePlay/Components/MeshCollisionComponent.h"
#include "Engine/GamePlay/Components/SphereComponent.h"

namespace CE
{
  CECapsuleComponent::CECapsuleComponent(CEObject* Owner, FString NewName)
      : CollisionComponent(Owner, NewName)
  {
    CE_CORE_DEBUG("CapsuleComponent created: ", NewName);
  }

  void CECapsuleComponent::SetRadius(float Radius)
  {
    m_Radius = glm::max(Radius, 0.0f);
  }

  void CECapsuleComponent::SetHeight(float Height)
  {
    m_Height = glm::max(Height, 2.0f * m_Radius);
  }

  void CECapsuleComponent::SetSize(float Radius, float Height)
  {
    SetRadius(Radius);
    SetHeight(Height);
  }

  bool CECapsuleComponent::CheckCollisionWithBox(const CEBoxComponent* Other) const
  {
    return CheckCapsuleBoxCollision(GetBottomSphereCenter(), GetTopSphereCenter(),
                                    GetRadius(), Other->GetBoundingBoxMin(), Other->GetBoundingBoxMax());
  }

  bool CECapsuleComponent::CheckCollisionWithSphere(const SphereComponent* Other) const
  {
    return Other->CheckSphereCapsuleCollision(this);
  }

  bool CECapsuleComponent::CheckCollisionWithCapsule(const CECapsuleComponent* Other) const
  {
    return CheckCapsuleCapsuleCollision(Other);
  }

  bool CECapsuleComponent::CheckCollisionWithMesh(const MeshCollisionComponent* Other) const
  {
    // Упрощенная проверка капсулы с bounding box меша
    glm::vec3 capsulePos = GetWorldLocation();
    float capsuleRadius = GetRadius();
    glm::vec3 boxMin = Other->GetBoundingBoxMin();
    glm::vec3 boxMax = Other->GetBoundingBoxMax();

    glm::vec3 closestPoint;
    closestPoint.x = glm::clamp(capsulePos.x, boxMin.x, boxMax.x);
    closestPoint.y = glm::clamp(capsulePos.y, boxMin.y, boxMax.y);
    closestPoint.z = glm::clamp(capsulePos.z, boxMin.z, boxMax.z);

    float distance = glm::distance(capsulePos, closestPoint);
    return distance <= capsuleRadius;
  }

  bool CECapsuleComponent::CheckCapsuleBoxCollision(const glm::vec3& capsuleBottom,
                                                    const glm::vec3& capsuleTop,
                                                    float capsuleRadius,
                                                    const glm::vec3& boxMin,
                                                    const glm::vec3& boxMax) const
  {
    // Находим ближайшую точку на сегменте капсулы к центру бокса
    glm::vec3 boxCenter = (boxMin + boxMax) * 0.5f;
    glm::vec3 closestPointOnSegment = ClosestPointOnSegment(capsuleBottom, capsuleTop, boxCenter);

    // Находим ближайшую точку на боксе к этой точке
    glm::vec3 closestPointOnBox;
    closestPointOnBox.x = glm::clamp(closestPointOnSegment.x, boxMin.x, boxMax.x);
    closestPointOnBox.y = glm::clamp(closestPointOnSegment.y, boxMin.y, boxMax.y);
    closestPointOnBox.z = glm::clamp(closestPointOnSegment.z, boxMin.z, boxMax.z);

    float distance = glm::distance(closestPointOnSegment, closestPointOnBox);
    return distance <= capsuleRadius;
  }

  bool CECapsuleComponent::CheckCapsuleCapsuleCollision(const CECapsuleComponent* Other) const
  {
    glm::vec3 thisBottom = GetBottomSphereCenter();
    glm::vec3 thisTop = GetTopSphereCenter();
    float thisRadius = GetRadius();

    glm::vec3 otherBottom = Other->GetBottomSphereCenter();
    glm::vec3 otherTop = Other->GetTopSphereCenter();
    float otherRadius = Other->GetRadius();

    // Находим минимальное расстояние между сегментами капсул
    float distance = DistanceBetweenSegments(thisBottom, thisTop, otherBottom, otherTop);
    return distance <= (thisRadius + otherRadius);
  }

  glm::vec3 CECapsuleComponent::ClosestPointOnSegment(const glm::vec3& a, const glm::vec3& b,
                                                      const glm::vec3& point) const
  {
    glm::vec3 ab = b - a;
    float t = glm::dot(point - a, ab) / glm::dot(ab, ab);
    t = glm::clamp(t, 0.0f, 1.0f);
    return a + t * ab;
  }

  float CECapsuleComponent::DistanceBetweenSegments(const glm::vec3& a1, const glm::vec3& a2,
                                                    const glm::vec3& b1, const glm::vec3& b2) const
  {
    glm::vec3 u = a2 - a1;
    glm::vec3 v = b2 - b1;
    glm::vec3 w = a1 - b1;

    float a = glm::dot(u, u);
    float b = glm::dot(u, v);
    float c = glm::dot(v, v);
    float d = glm::dot(u, w);
    float e = glm::dot(v, w);

    float D = a * c - b * b;
    float sc, sN, sD = D;
    float tc, tN, tD = D;

    if (D < 0.0001f)
    {
      sN = 0.0f;
      sD = 1.0f;
      tN = e;
      tD = c;
    }
    else
    {
      sN = (b * e - c * d);
      tN = (a * e - b * d);
      if (sN < 0.0f)
      {
        sN = 0.0f;
        tN = e;
        tD = c;
      }
      else if (sN > sD)
      {
        sN = sD;
        tN = e + b;
        tD = c;
      }
    }

    if (tN < 0.0f)
    {
      tN = 0.0f;
      if (-d < 0.0f)
        sN = 0.0f;
      else if (-d > a)
        sN = sD;
      else
      {
        sN = -d;
        sD = a;
      }
    }
    else if (tN > tD)
    {
      tN = tD;
      if ((-d + b) < 0.0f)
        sN = 0.0f;
      else if ((-d + b) > a)
        sN = sD;
      else
      {
        sN = -d + b;
        sD = a;
      }
    }

    sc = (std::abs(sN) < 0.0001f ? 0.0f : sN / sD);
    tc = (std::abs(tN) < 0.0001f ? 0.0f : tN / tD);

    glm::vec3 dP = w + (sc * u) - (tc * v);
    return glm::length(dP);
  }

  glm::vec3 CECapsuleComponent::GetBoundingBoxMin() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    float totalHeight = m_Height;
    return worldPos - glm::vec3(m_Radius, totalHeight * 0.5f, m_Radius);
  }

  glm::vec3 CECapsuleComponent::GetBoundingBoxMax() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    float totalHeight = m_Height;
    return worldPos + glm::vec3(m_Radius, totalHeight * 0.5f, m_Radius);
  }

  glm::vec3 CECapsuleComponent::GetTopSphereCenter() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    float cylinderHeight = GetCylinderHeight();
    return worldPos + glm::vec3(0.0f, cylinderHeight * 0.5f, 0.0f);
  }

  glm::vec3 CECapsuleComponent::GetBottomSphereCenter() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    float cylinderHeight = GetCylinderHeight();
    return worldPos - glm::vec3(0.0f, cylinderHeight * 0.5f, 0.0f);
  }

  float CECapsuleComponent::GetCylinderHeight() const
  {
    return glm::max(m_Height - 2.0f * m_Radius, 0.0f);
  }

  void CECapsuleComponent::Update(float DeltaTime)
  {
    CollisionComponent::Update(DeltaTime);
  }
}  // namespace CE