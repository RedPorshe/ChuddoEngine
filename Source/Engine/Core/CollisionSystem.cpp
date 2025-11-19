#include "Engine/Core/CollisionSystem.h"

#include <algorithm>
#include <limits>

#include "Engine/GamePlay/Components/BoxComponent.h"
#include "Engine/GamePlay/Components/CapsuleComponent.h"
#include "Engine/GamePlay/Components/CollisionComponent.h"
#include "Engine/GamePlay/Components/MeshCollisionComponent.h"
#include "Engine/GamePlay/Components/SphereComponent.h"

namespace CE
{
  CollisionSystem& CollisionSystem::Get()
  {
    static CollisionSystem instance;
    return instance;
  }

  void CollisionSystem::RegisterCollisionComponent(CollisionComponent* Component)
  {
    if (Component && std::find(m_CollisionComponents.begin(), m_CollisionComponents.end(), Component) == m_CollisionComponents.end())
    {
      m_CollisionComponents.push_back(Component);
      CE_CORE_DEBUG("CollisionComponent registered: ", Component->GetName());
    }
  }

  void CollisionSystem::UnregisterCollisionComponent(CollisionComponent* Component)
  {
    auto it = std::find(m_CollisionComponents.begin(), m_CollisionComponents.end(), Component);
    if (it != m_CollisionComponents.end())
    {
      m_CollisionComponents.erase(it);
      CE_CORE_DEBUG("CollisionComponent unregistered: ", Component->GetName());
    }
  }

  std::vector<CollisionHitResult> CollisionSystem::CheckCollisions(CollisionComponent* Component) const
  {
    std::vector<CollisionHitResult> hits;

    if (!Component || !Component->IsCollisionEnabled())
    {
      CE_CORE_DEBUG("CheckCollisions: Component null or disabled");
      return hits;
    }

    // Статическая переменная для отслеживания глубины рекурсии
    static thread_local int recursionDepth = 0;

    // Ограничиваем глубину рекурсии
    if (recursionDepth > 3)
    {
      CE_CORE_WARN("Collision recursion depth exceeded for: ", Component->GetName());
      return hits;
    }

    recursionDepth++;

    try
    {
      int aabbCount = 0;
      int preciseCount = 0;

      for (auto* other : m_CollisionComponents)
      {
        if (other == Component || !other->IsCollisionEnabled())
          continue;

        // Проверяем сначала bounding boxes для оптимизации
        glm::vec3 thisMin = Component->GetBoundingBoxMin();
        glm::vec3 thisMax = Component->GetBoundingBoxMax();
        glm::vec3 otherMin = other->GetBoundingBoxMin();
        glm::vec3 otherMax = other->GetBoundingBoxMax();

        // Быстрая проверка AABB
        bool aabbCollision = (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
                             (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
                             (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z);

        if (aabbCollision)
        {
          aabbCount++;

          // Только если AABB пересекаются, делаем точную проверку
          bool hasCollision = Component->CheckCollision(other);

          if (hasCollision)
          {
            preciseCount++;

            CollisionHitResult hit;
            hit.HitComponent = other;
            hit.bBlockingHit = true;

            // Упрощенный расчет точки столкновения
            hit.ImpactPoint = (Component->GetWorldLocation() + other->GetWorldLocation()) * 0.5f;
            hit.ImpactNormal = glm::normalize(Component->GetWorldLocation() - other->GetWorldLocation());
            hit.Distance = glm::distance(Component->GetWorldLocation(), other->GetWorldLocation());

            hits.push_back(hit);
          }
          else
          {
            CE_CORE_DEBUG("NO precise collision between: ",
                          Component->GetName(), " and ", other->GetName());
          }
        }
      }

      CE_CORE_DEBUG("Collision check: ", Component->GetName(),
                    " AABB checks: ", aabbCount,
                    " Precise collisions: ", preciseCount);
    }
    catch (const std::exception& e)
    {
      CE_CORE_ERROR("Exception in CheckCollisions: ", e.what());
    }
    catch (...)
    {
      CE_CORE_ERROR("Unknown exception in CheckCollisions");
    }

    recursionDepth--;

    return hits;
  }

  bool CollisionSystem::LineTrace(const glm::vec3& Start, const glm::vec3& End, CollisionHitResult& OutHit)
  {
    glm::vec3 direction = End - Start;
    float maxDistance = glm::length(direction);
    if (maxDistance < 0.001f)
      return false;

    direction = glm::normalize(direction);

    CollisionHitResult closestHit;
    closestHit.Distance = std::numeric_limits<float>::max();
    bool foundHit = false;

    for (auto* component : m_CollisionComponents)
    {
      if (!component->IsCollisionEnabled())
        continue;

      glm::vec3 boxMin = component->GetBoundingBoxMin();
      glm::vec3 boxMax = component->GetBoundingBoxMax();

      // Алгоритм пересечения луча с AABB (Slab Method)
      float tmin = (boxMin.x - Start.x) / direction.x;
      float tmax = (boxMax.x - Start.x) / direction.x;

      if (tmin > tmax)
        std::swap(tmin, tmax);

      float tymin = (boxMin.y - Start.y) / direction.y;
      float tymax = (boxMax.y - Start.y) / direction.y;

      if (tymin > tymax)
        std::swap(tymin, tymax);

      if ((tmin > tymax) || (tymin > tmax))
        continue;

      if (tymin > tmin)
        tmin = tymin;
      if (tymax < tmax)
        tmax = tymax;

      float tzmin = (boxMin.z - Start.z) / direction.z;
      float tzmax = (boxMax.z - Start.z) / direction.z;

      if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

      if ((tmin > tzmax) || (tzmin > tmax))
        continue;

      if (tzmin > tmin)
        tmin = tzmin;
      if (tzmax < tmax)
        tmax = tzmax;

      // Находим ближайшее пересечение
      float hitDistance = tmin;
      if (tmin < 0.0f)
      {
        hitDistance = tmax;
        if (tmax < 0.0f)
          continue;
      }

      if (hitDistance < maxDistance && hitDistance < closestHit.Distance && hitDistance >= 0.0f)
      {
        closestHit.HitComponent = component;
        closestHit.ImpactPoint = Start + direction * hitDistance;
        closestHit.Distance = hitDistance;
        closestHit.bBlockingHit = true;

        // Расчет нормали пересечения
        glm::vec3 hitPoint = closestHit.ImpactPoint;
        glm::vec3 boxCenter = (boxMin + boxMax) * 0.5f;

        // Находим ближайшую грань бокса для определения нормали
        glm::vec3 localHit = hitPoint - boxCenter;
        glm::vec3 halfExtents = (boxMax - boxMin) * 0.5f;

        // Находим минимальное расстояние до грани
        glm::vec3 absLocalHit = glm::abs(localHit);
        glm::vec3 distanceToFace = halfExtents - absLocalHit;

        float minDistance = std::min({distanceToFace.x, distanceToFace.y, distanceToFace.z});

        if (minDistance == distanceToFace.x)
        {
          closestHit.ImpactNormal = glm::vec3((localHit.x > 0) ? 1.0f : -1.0f, 0.0f, 0.0f);
        }
        else if (minDistance == distanceToFace.y)
        {
          closestHit.ImpactNormal = glm::vec3(0.0f, (localHit.y > 0) ? 1.0f : -1.0f, 0.0f);
        }
        else
        {
          closestHit.ImpactNormal = glm::vec3(0.0f, 0.0f, (localHit.z > 0) ? 1.0f : -1.0f);
        }

        foundHit = true;
      }
    }

    if (foundHit)
    {
      OutHit = closestHit;
    }

    return foundHit;
  }

  bool CollisionSystem::SphereCast(float Radius, const glm::vec3& Start, const glm::vec3& End, CollisionHitResult& OutHit)
  {
    glm::vec3 direction = End - Start;
    float maxDistance = glm::length(direction);
    if (maxDistance < 0.001f)
      return false;

    direction = glm::normalize(direction);

    CollisionHitResult closestHit;
    closestHit.Distance = std::numeric_limits<float>::max();
    bool foundHit = false;

    for (auto* component : m_CollisionComponents)
    {
      if (!component->IsCollisionEnabled())
        continue;

      // Упрощенная проверка сферы с bounding box
      glm::vec3 boxMin = component->GetBoundingBoxMin();
      glm::vec3 boxMax = component->GetBoundingBoxMax();

      // Расширяем bounding box на радиус сферы
      glm::vec3 expandedMin = boxMin - glm::vec3(Radius);
      glm::vec3 expandedMax = boxMax + glm::vec3(Radius);

      // Проверяем пересечение луча с расширенным bounding box
      float tmin = (expandedMin.x - Start.x) / direction.x;
      float tmax = (expandedMax.x - Start.x) / direction.x;

      if (tmin > tmax)
        std::swap(tmin, tmax);

      float tymin = (expandedMin.y - Start.y) / direction.y;
      float tymax = (expandedMax.y - Start.y) / direction.y;

      if (tymin > tymax)
        std::swap(tymin, tymax);

      if ((tmin > tymax) || (tymin > tmax))
        continue;

      if (tymin > tmin)
        tmin = tymin;
      if (tymax < tmax)
        tmax = tymax;

      float tzmin = (expandedMin.z - Start.z) / direction.z;
      float tzmax = (expandedMax.z - Start.z) / direction.z;

      if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

      if ((tmin > tzmax) || (tzmin > tmax))
        continue;

      if (tzmin > tmin)
        tmin = tzmin;
      if (tzmax < tmax)
        tmax = tzmax;

      // Находим ближайшее пересечение
      float hitDistance = tmin;
      if (tmin < 0.0f)
      {
        hitDistance = tmax;
        if (tmax < 0.0f)
          continue;
      }

      if (hitDistance < maxDistance && hitDistance < closestHit.Distance && hitDistance >= 0.0f)
      {
        // Проверяем точное пересечение сферы с компонентом
        glm::vec3 spherePosAtHit = Start + direction * hitDistance;

        // Для разных типов коллизий используем разные проверки
        bool preciseHit = false;

        switch (component->GetCollisionShape())
        {
          case ECollisionShape::Box:
          {
            auto* boxComponent = static_cast<CEBoxComponent*>(component);
            preciseHit = boxComponent->CheckPointCollision(spherePosAtHit);
            break;
          }
          case ECollisionShape::Sphere:
          {
            auto* sphereComponent = static_cast<SphereComponent*>(component);
            float distance = glm::distance(spherePosAtHit, sphereComponent->GetWorldLocation());
            preciseHit = distance <= (Radius + sphereComponent->GetRadius());
            break;
          }
          case ECollisionShape::Capsule:
          {
            // Упрощенная проверка для капсулы
            auto* capsuleComponent = static_cast<CECapsuleComponent*>(component);
            glm::vec3 closestPoint = capsuleComponent->ClosestPointOnSegment(
                capsuleComponent->GetBottomSphereCenter(),
                capsuleComponent->GetTopSphereCenter(),
                spherePosAtHit);
            float distance = glm::distance(spherePosAtHit, closestPoint);
            preciseHit = distance <= (Radius + capsuleComponent->GetRadius());
            break;
          }
          case ECollisionShape::Mesh:
          {
            // Для меша используем упрощенную проверку с bounding box
            glm::vec3 closestPoint;
            closestPoint.x = glm::clamp(spherePosAtHit.x, boxMin.x, boxMax.x);
            closestPoint.y = glm::clamp(spherePosAtHit.y, boxMin.y, boxMax.y);
            closestPoint.z = glm::clamp(spherePosAtHit.z, boxMin.z, boxMax.z);
            float distance = glm::distance(spherePosAtHit, closestPoint);
            preciseHit = distance <= Radius;
            break;
          }
          default:
            preciseHit = false;
        }

        if (preciseHit)
        {
          closestHit.HitComponent = component;
          closestHit.ImpactPoint = spherePosAtHit;
          closestHit.Distance = hitDistance;
          closestHit.bBlockingHit = true;

          // Расчет нормали
          glm::vec3 hitPoint = closestHit.ImpactPoint;
          glm::vec3 boxCenter = (boxMin + boxMax) * 0.5f;
          closestHit.ImpactNormal = glm::normalize(hitPoint - boxCenter);

          foundHit = true;
        }
      }
    }

    if (foundHit)
    {
      OutHit = closestHit;
    }

    return foundHit;
  }

  void CollisionSystem::Update(float DeltaTime)
  {
    // Используем параметр, чтобы убрать предупреждение
    (void)DeltaTime;

    // Проверяем коллизии для всех компонентов
    for (auto* component : m_CollisionComponents)
    {
      if (!component->IsCollisionEnabled() || !component->GetGenerateOverlapEvents())
        continue;

      auto collisions = CheckCollisions(component);

      // Здесь можно обработать события коллизий
      if (!collisions.empty())
      {
        CE_CORE_DEBUG("Component ", component->GetName(),
                      " has ", collisions.size(), " collisions");

        // TODO: Вызвать события overlap/коллизий
        // Убираем неиспользуемую переменную
        for ([[maybe_unused]] const auto& collision : collisions)
        {
          // Можно вызвать OnOverlapBegin, OnCollision и т.д.
        }
      }
    }
  }
}  // namespace CE