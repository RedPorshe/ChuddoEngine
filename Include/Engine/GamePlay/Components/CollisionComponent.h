// CollisionComponent.h
#pragma once

#include "Engine/GamePlay/Components/SceneComponent.h"

namespace CE
{
  enum class ECollisionShape
  {
    Box,
    Capsule,
    Sphere,
    Mesh
  };

  class CollisionComponent : public SceneComponent
  {
   public:
    CollisionComponent(CEObject* Owner = nullptr, FString NewName = "CollisionComponent");
    virtual ~CollisionComponent() = default;

    virtual ECollisionShape GetCollisionShape() const = 0;
    virtual bool CheckCollision(const CollisionComponent* Other) const = 0;
    virtual glm::vec3 GetBoundingBoxMin() const = 0;
    virtual glm::vec3 GetBoundingBoxMax() const = 0;

    // Настройки коллизии
    void SetCollisionEnabled(bool Enabled)
    {
      m_CollisionEnabled = Enabled;
    }
    bool IsCollisionEnabled() const
    {
      return m_CollisionEnabled;
    }

    void SetGenerateOverlapEvents(bool Generate)
    {
      m_GenerateOverlapEvents = Generate;
    }
    bool GetGenerateOverlapEvents() const
    {
      return m_GenerateOverlapEvents;
    }

   protected:
    bool m_CollisionEnabled = true;
    bool m_GenerateOverlapEvents = true;
  };
}  // namespace CE