#include "Engine/GamePlay/World/Levels/Level.h"
#include "Engine/GamePlay/CollisionSystem/CollisionComponent.h"
#include "Engine/GamePlay/CollisionSystem/BoxComponent.h"
#include "Engine/GamePlay/CollisionSystem/CapsuleComponent.h"
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

  bool CLevel::Raycast(const Math::Ray& ray, FRaycastHit& outHit, float maxDistance)
  {
    outHit = FRaycastHit();
    float closestDistance = maxDistance;
    bool bHit = false;

    for (const auto& actor : m_Actors)
    {
      auto collisionComponents = actor->GetComponents<CCollisionComponent>();
      for (auto* comp : collisionComponents)
      {
        if (!comp->IsCollisionEnabled()) continue;

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
    for (auto& actor : m_Actors)
    {
      actor->Tick(DeltaTime);
    }
  }
}  // namespace CE
