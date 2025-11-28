#include "Engine/GamePlay/World/Levels/Level.h"
#include "Engine/GamePlay/CollisionSystem/CollisionComponent.h"
#include "Engine/GamePlay/CollisionSystem/BoxComponent.h"
#include "Engine/GamePlay/CollisionSystem/CapsuleComponent.h"
#include "Engine/GamePlay/CollisionSystem/TerrainCollisionComponent.h"
#include "Engine/GamePlay/Components/PhysicsComponent.h"
#include "Engine/Utils/Math/AABB.h"

namespace CE
{
  CLevel::CLevel(CObject* Owner, FString LevelName)
      : CObject(Owner, LevelName)
  {
    CE_CORE_DEBUG("Level created: ", LevelName);
  }

  CLevel::~CLevel()
  {
    CE_CORE_DEBUG("Level destroyed: ", GetName());
  }

  void CLevel::DestroyActor(CActor* Actor)
  {
    auto it = std::find_if(m_Actors.begin(), m_Actors.end(),
                           [Actor](const auto& ptr)
                           { return ptr.get() == Actor; });

    if (it != m_Actors.end())
    {
      CE_CORE_DEBUG("Destroying actor: ", Actor->GetName());
      m_Actors.erase(it);
    }
  }

  CActor* CLevel::FindActorByName(const FString& Name)
  {
    for (auto& actor : m_Actors)
    {
      if (actor->GetName() == Name)
      {
        return actor.get();
      }
    }
    return nullptr;
  }

  bool CLevel::Raycast(const Math::Ray& ray, FRaycastHit& outHit, float maxDistance, CActor* ignoreActor)
  {
    outHit = FRaycastHit();
    float closestDistance = maxDistance;
    bool bHit = false;

    for (const auto& actor : m_Actors)
    {
      if (actor.get() == ignoreActor) continue;

      auto collisionComponents = actor->GetComponents<CCollisionComponent>();
      for (auto* comp : collisionComponents)
      {
        if (!comp->IsCollisionEnabled()) continue;

        // Special handling for terrain collision
        if (auto* terrainComp = dynamic_cast<CE::CTerrainCollisionComponent*>(comp))
        {
          // For terrain, check if ray intersects the terrain surface
          Math::Vector3f terrainMin = terrainComp->GetBoundingBoxMin();
          Math::Vector3f terrainMax = terrainComp->GetBoundingBoxMax();

          // Check if ray origin is within terrain XZ bounds
          if (ray.origin.x >= terrainMin.x && ray.origin.x <= terrainMax.x &&
              ray.origin.z >= terrainMin.z && ray.origin.z <= terrainMax.z)
          {
            // For downward vertical rays (common for ground checks)
            if (ray.direction.y < 0.0f)
            {
              Math::Vector3f localPos = ray.origin - terrainComp->GetWorldLocation();
              float terrainHeight = terrainComp->GetHeightAtPosition(localPos);
              float worldTerrainHeight = terrainComp->GetWorldLocation().y + terrainHeight;

              if (ray.origin.y > worldTerrainHeight)
              {
                float distance = (ray.origin.y - worldTerrainHeight) / -ray.direction.y;
                if (distance >= 0.0f && distance < closestDistance)
                {
                  outHit.Component = comp;
                  outHit.Distance = distance;
                  outHit.Location = Math::Vector3f(ray.origin.x, worldTerrainHeight, ray.origin.z);
                  outHit.Normal = Math::Vector3f(0.0f, 1.0f, 0.0f); // Up normal for ground
                  outHit.bBlockingHit = true;
                  closestDistance = distance;
                  bHit = true;
                }
              }
            }
          }
        }
        else
        {
          // Default AABB intersection for other components
          Math::AABB aabb(comp->GetBoundingBoxMin(), comp->GetBoundingBoxMax());
          float tMin, tMax;
          if (ray.Intersects(aabb, tMin, tMax) && tMin >= 0.0f && tMin < closestDistance)
          {
            outHit.Component = comp;
            outHit.Distance = tMin;
            outHit.Location = ray.GetPoint(tMin);
            outHit.Normal = Math::Vector3f(0.0f, 0.0f, 1.0f); // Placeholder normal
            outHit.bBlockingHit = true;
            closestDistance = tMin;
            bHit = true;
          }
        }
      }
    }

    return bHit;
  }

  void CLevel::BeginPlay()
  {
    CObject::BeginPlay();

    for (auto& actor : m_Actors)
    {
      actor->BeginPlay();
    }
  }

  void CLevel::Update(float DeltaTime)
  {
    CObject::Update(DeltaTime);

    for (auto& actor : m_Actors)
    {
      actor->Update(DeltaTime);
    }
  }

  void CLevel::Tick(float DeltaTime)
  {
    Update(DeltaTime);

    // Perform collision detection and response
    PerformCollisionDetection(DeltaTime);

    for (auto& actor : m_Actors)
    {
      actor->Tick(DeltaTime);
    }
  }

  void CLevel::PerformCollisionDetection(float DeltaTime)
  {
    // Collect all collision components
    std::vector<CCollisionComponent*> allCollisionComponents;
    for (const auto& actor : m_Actors)
    {
      auto collisionComponents = actor->GetComponents<CCollisionComponent>();
      for (auto* comp : collisionComponents)
      {
        if (comp->IsCollisionEnabled())
        {
          allCollisionComponents.push_back(comp);
        }
      }
    }

    CE_LOG("[CollisionDetection] Found %d collision components", allCollisionComponents.size());

    // Check collisions between all pairs
    for (size_t i = 0; i < allCollisionComponents.size(); ++i)
    {
      for (size_t j = i + 1; j < allCollisionComponents.size(); ++j)
      {
        CCollisionComponent* compA = allCollisionComponents[i];
        CCollisionComponent* compB = allCollisionComponents[j];

        CE_LOG("[CollisionDetection] Checking collision between %s and %s",
               compA->GetOwner()->GetName().c_str(), compB->GetOwner()->GetName().c_str());

        if (compA->CheckCollision(compB))
        {
          CE_LOG("[CollisionDetection] Collision detected between %s and %s",
                 compA->GetOwner()->GetName().c_str(), compB->GetOwner()->GetName().c_str());
          // Handle collision response
          HandleCollision(compA, compB, DeltaTime);
        }
      }
    }
  }

  void CLevel::HandleCollision(CCollisionComponent* compA, CCollisionComponent* compB, float DeltaTime)
  {
    // Get actors
    CActor* actorA = dynamic_cast<CActor*>(compA->GetOwner());
    CActor* actorB = dynamic_cast<CActor*>(compB->GetOwner());

    if (!actorA || !actorB) return;

    // Get physics components
    auto physicsA = actorA->GetComponents<CPhysicsComponent>();
    auto physicsB = actorB->GetComponents<CPhysicsComponent>();

    Math::Vector3f normal;
    float penetration;

    // Special handling for terrain collision
    CTerrainCollisionComponent* terrainA = dynamic_cast<CTerrainCollisionComponent*>(compA);
    CTerrainCollisionComponent* terrainB = dynamic_cast<CTerrainCollisionComponent*>(compB);

    if (terrainA || terrainB)
    {
      // For terrain collision, normal is always up, and penetration is calculated differently
      CTerrainCollisionComponent* terrain = terrainA ? terrainA : terrainB;
      CCollisionComponent* other = terrainA ? compB : compA;
      CActor* otherActor = terrainA ? actorB : actorA;
      auto otherPhysics = terrainA ? physicsB : physicsA;

      // Calculate penetration based on terrain height
      Math::Vector3f otherMin = other->GetBoundingBoxMin() + other->GetWorldLocation();
      Math::Vector3f localPos = otherMin - terrain->GetWorldLocation();
      float terrainHeight = terrain->GetHeightAtPosition(localPos);
      float worldTerrainHeight = terrain->GetWorldLocation().y + terrainHeight;

      penetration = worldTerrainHeight - otherMin.y;

      // Debug logging
      CE_LOG("[TerrainCollision] Actor: %s, otherMin.y: %.2f, worldTerrainHeight: %.2f, penetration: %.2f",
             otherActor->GetName().c_str(), otherMin.y, worldTerrainHeight, penetration);

      if (penetration < -0.1f) return; // No collision (with tolerance)

      normal = Math::Vector3f(0.0f, 1.0f, 0.0f); // Up normal for terrain

      // Terrain is always static
      if (!otherPhysics.empty())
      {
        CPhysicsComponent* phys = otherPhysics[0];

        // Reflect velocity if moving down
        Math::Vector3f velocity = phys->GetVelocity();
        if (velocity.y < 0)
        {
          velocity.y = -velocity.y * 0.8f; // Bounce with damping
          phys->SetVelocity(velocity);
          CE_LOG("[TerrainCollision] Bounced actor %s, new velocity: (%.2f, %.2f, %.2f)",
                 otherActor->GetName().c_str(), velocity.x, velocity.y, velocity.z);
        }

        // Separate - ensure minimum separation to prevent falling through
        float effectivePenetration = std::max(penetration, 0.01f);
        Math::Vector3f separation = normal * effectivePenetration;
        Math::Vector3f newPos = otherActor->GetActorLocation() + separation;
        otherActor->SetActorLocation(newPos);
        CE_LOG("[TerrainCollision] Moved actor %s to position: (%.2f, %.2f, %.2f)",
               otherActor->GetName().c_str(), newPos.x, newPos.y, newPos.z);
      }

      // Trigger overlap events
      compA->OnHit(compB, terrainA ? normal : -normal);
      compB->OnHit(compA, terrainB ? normal : -normal);
      return;
    }

    // Standard collision handling for non-terrain
    // Calculate collision normal and penetration using AABB overlap
    Math::Vector3f posA = compA->GetWorldLocation();
    Math::Vector3f posB = compB->GetWorldLocation();

    Math::AABB aabbA(compA->GetBoundingBoxMin() + posA, compA->GetBoundingBoxMax() + posA);
    Math::AABB aabbB(compB->GetBoundingBoxMin() + posB, compB->GetBoundingBoxMax() + posB);

    Math::Vector3f overlap = Math::Vector3f(
      std::min(aabbA.max.x, aabbB.max.x) - std::max(aabbA.min.x, aabbB.min.x),
      std::min(aabbA.max.y, aabbB.max.y) - std::max(aabbA.min.y, aabbB.min.y),
      std::min(aabbA.max.z, aabbB.max.z) - std::max(aabbA.min.z, aabbB.min.z)
    );

    if (overlap.x <= 0 || overlap.y <= 0 || overlap.z <= 0) return; // No collision

    // Find the axis with minimum penetration
    float minPenetration = std::min({overlap.x, overlap.y, overlap.z});

    if (minPenetration == overlap.x) {
      normal.x = (posA.x < posB.x) ? -1.0f : 1.0f;
    } else if (minPenetration == overlap.y) {
      normal.y = (posA.y < posB.y) ? -1.0f : 1.0f;
    } else {
      normal.z = (posA.z < posB.z) ? -1.0f : 1.0f;
    }

    penetration = minPenetration;

    // Apply collision response
    if (!physicsA.empty() && !physicsB.empty())
    {
      CPhysicsComponent* physA = physicsA[0];
      CPhysicsComponent* physB = physicsB[0];

      // Simple elastic collision response
      Math::Vector3f relativeVelocity = physA->GetVelocity() - physB->GetVelocity();
      float velocityAlongNormal = relativeVelocity.Dot(normal);

      if (velocityAlongNormal > 0) return; // Separating

      float restitution = 0.8f; // Bounciness
      float impulseScalar = -(1 + restitution) * velocityAlongNormal / (1 / physA->GetMass() + 1 / physB->GetMass());

      Math::Vector3f impulse = normal * impulseScalar;

      physA->ApplyCollisionImpulse(-impulse);
      physB->ApplyCollisionImpulse(impulse);

      // Separate objects
      Math::Vector3f separation = normal * (penetration / 2.0f);
      actorA->SetActorLocation(actorA->GetActorLocation() + separation);
      actorB->SetActorLocation(actorB->GetActorLocation() - separation);
    }
    else if (!physicsA.empty())
    {
      // Only A has physics, B is static
      CPhysicsComponent* physA = physicsA[0];

      // Reflect velocity
      Math::Vector3f velocity = physA->GetVelocity();
      velocity = velocity - normal * (2.0f * velocity.Dot(normal));
      physA->SetVelocity(velocity * 0.8f); // Dampen

      // Separate
      Math::Vector3f separation = normal * penetration;
      actorA->SetActorLocation(actorA->GetActorLocation() + separation);
    }
    else if (!physicsB.empty())
    {
      // Only B has physics, A is static
      CPhysicsComponent* physB = physicsB[0];

      // Reflect velocity
      Math::Vector3f velocity = physB->GetVelocity();
      velocity = velocity - normal * (2.0f * velocity.Dot(normal));
      physB->SetVelocity(velocity * 0.8f); // Dampen

      // Separate
      Math::Vector3f separation = normal * penetration;
      actorB->SetActorLocation(actorB->GetActorLocation() + separation);
    }

    // Trigger overlap events
    compA->OnHit(compB, normal);
    compB->OnHit(compA, -normal);
  }
}  // namespace CE
