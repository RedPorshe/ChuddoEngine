// CollisionSystem.h
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

    // Проверка коллизий для конкретного компонента
    std::vector<CollisionHitResult> CheckCollisions(CollisionComponent* Component) const;

    // Трассировка луча
    bool LineTrace(const glm::vec3& Start, const glm::vec3& End, CollisionHitResult& OutHit);

    // Трассировка сферы
    bool SphereCast(float Radius, const glm::vec3& Start, const glm::vec3& End, CollisionHitResult& OutHit);

    // Обновление системы коллизий (вызывается каждый кадр)
    void Update(float DeltaTime);

    // Получение всех зарегистрированных компонентов (для отладки)
    const std::vector<CollisionComponent*>& GetCollisionComponents() const
    {
      return m_CollisionComponents;
    }

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

    // Дополнительная информация
    glm::vec3 StartLocation;
    glm::vec3 EndLocation;

    bool IsValid() const
    {
      return HitComponent != nullptr;
    }
  };
}  // namespace CE