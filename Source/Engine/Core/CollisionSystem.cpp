#include "Engine/Core/CollisionSystem.h"

#include <algorithm>

#include "Engine/GamePlay/Components/CollisionComponent.h"

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

      if (tmin < maxDistance && tmin < closestHit.Distance && tmin >= 0.0f)
      {
        closestHit.HitComponent = component;
        closestHit.ImpactPoint = Start + direction * tmin;
        closestHit.Distance = tmin;
        closestHit.bBlockingHit = true;

        // Упрощенный расчет нормали
        glm::vec3 hitPoint = closestHit.ImpactPoint;
        glm::vec3 boxCenter = (boxMin + boxMax) * 0.5f;
        closestHit.ImpactNormal = glm::normalize(hitPoint - boxCenter);

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

      if (tmin < maxDistance && tmin < closestHit.Distance && tmin >= 0.0f)
      {
        closestHit.HitComponent = component;
        closestHit.ImpactPoint = Start + direction * tmin;
        closestHit.Distance = tmin;
        closestHit.bBlockingHit = true;

        // Упрощенный расчет нормали
        glm::vec3 hitPoint = closestHit.ImpactPoint;
        glm::vec3 boxCenter = (boxMin + boxMax) * 0.5f;
        closestHit.ImpactNormal = glm::normalize(hitPoint - boxCenter);

        foundHit = true;
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
    (void)DeltaTime;  // Пока не используем
  }
}  // namespace CE