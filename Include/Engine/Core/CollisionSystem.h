#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace CE
{
  class CollisionComponent;
  struct CollisionHitResult;

  class CollisionSystem
  {
   public:
    static CollisionSystem& Get();

    void RegisterCollisionComponent(CollisionComponent* Component);
    void UnregisterCollisionComponent(CollisionComponent* Component);

    std::vector<CollisionHitResult> CheckCollisions(CollisionComponent* Component) const;
    bool LineTrace(const glm::vec3& Start, const glm::vec3& End, CollisionHitResult& OutHit);
    bool SphereCast(float Radius, const glm::vec3& Start, const glm::vec3& End, CollisionHitResult& OutHit);

    void Update(float DeltaTime);

   private:
    std::vector<CollisionComponent*> m_CollisionComponents;
  };

  struct CollisionHitResult
  {
    CollisionComponent* HitComponent = nullptr;
    glm::vec3 ImpactPoint;
    glm::vec3 ImpactNormal;
    float Distance = 0.0f;
    bool bBlockingHit = false;
  };
}  // namespace CE