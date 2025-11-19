// CollisionComponent.cpp
#include "Engine/GamePlay/Components/CollisionComponent.h"

#include "Engine/Core/CollisionSystem.h"
#include "Engine/GamePlay/Components/BoxComponent.h"
#include "Engine/GamePlay/Components/CapsuleComponent.h"
#include "Engine/GamePlay/Components/MeshCollisionComponent.h"
#include "Engine/GamePlay/Components/SphereComponent.h"

namespace CE
{
  CollisionComponent::CollisionComponent(CEObject* Owner, FString NewName)
      : SceneComponent(Owner, NewName)
  {
    CE_CORE_DEBUG("CollisionComponent created: ", NewName);
    CollisionSystem::Get().RegisterCollisionComponent(this);
  }

  bool CollisionComponent::CheckCollision(const CollisionComponent* Other) const
  {
    if (!Other || !m_CollisionEnabled || !Other->IsCollisionEnabled())
      return false;

    // Быстрая проверка AABB для оптимизации
    glm::vec3 thisMin = GetBoundingBoxMin();
    glm::vec3 thisMax = GetBoundingBoxMax();
    glm::vec3 otherMin = Other->GetBoundingBoxMin();
    glm::vec3 otherMax = Other->GetBoundingBoxMax();

    bool aabbCollision = (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
                         (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
                         (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z);

    if (!aabbCollision)
      return false;

    // Точная проверка в зависимости от типов коллизий
    switch (Other->GetCollisionShape())
    {
      case ECollisionShape::Box:
        return CheckCollisionWithBox(static_cast<const CEBoxComponent*>(Other));

      case ECollisionShape::Sphere:
        return CheckCollisionWithSphere(static_cast<const SphereComponent*>(Other));

      case ECollisionShape::Capsule:
        return CheckCollisionWithCapsule(static_cast<const CECapsuleComponent*>(Other));

      case ECollisionShape::Mesh:
        return CheckCollisionWithMesh(static_cast<const MeshCollisionComponent*>(Other));

      default:
        return false;
    }
  }
}  // namespace CE