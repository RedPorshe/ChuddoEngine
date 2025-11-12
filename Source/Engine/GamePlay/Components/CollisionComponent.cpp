// CollisionComponent.cpp
#include "Engine/GamePlay/Components/CollisionComponent.h"

#include "Engine/Core/CollisionSystem.h"

namespace CE
{
  CollisionComponent::CollisionComponent(CEObject* Owner, FString NewName)
      : SceneComponent(Owner, NewName)
  {
    CE_CORE_DEBUG("CollisionComponent created: ", NewName);
    CollisionSystem::Get().RegisterCollisionComponent(this);
  }
}  // namespace CE