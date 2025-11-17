#include "Engine/GamePlay/Actors/Actor.h"

#include "Engine/Core/CollisionSystem.h"
#include "Engine/GamePlay/Components/CollisionComponent.h"
// #include "World/Levels/CELevel.h"

namespace CE
{
  CEActor::CEActor(CEObject* Owner, FString NewName)
      : CEObject(Owner, NewName)
  {
    // Создаем root component
    m_RootComponent = AddDefaultSubObject<SceneComponent>("root", this, "RootComponent");
    CE_CORE_DEBUG("CEActor created: ", NewName);
  }

  CELevel* CEActor::GetLevel() const
  {
    const CEObject* current = this;
    while (current)
    {
      if (auto* level = dynamic_cast<const CELevel*>(current))
      {
        return const_cast<CELevel*>(level);
      }
      current = current->GetOwner();
    }
    return nullptr;
  }

  glm::vec3 CEActor::GetActorForwardVector() const
  {
    return m_RootComponent ? m_RootComponent->GetForwardVector() : glm::vec3(0.0f, 0.0f, 1.0f);
  }
  glm::vec3 CEActor::GetActorRightVector() const
  {
    return m_RootComponent ? m_RootComponent->GetRightVector() : glm::vec3(1.0f, 0.0f, 0.0f);
  }

  glm::vec3 CEActor::GetActorUpVector() const
  {
    return m_RootComponent ? m_RootComponent->GetUpVector() : glm::vec3(0.0f, 1.0f, 0.0f);
  }

  void CEActor::SetActorLocation(const float x, const float y, const float z)
  {
    SetActorLocation(glm::vec3(x, y, z));
  }

  void CEActor::SetActorLocation(const glm::vec3& NewLocation)
  {
    if (m_RootComponent && !m_RootComponent->GetParent())
    {
      m_RootComponent->SetPosition(NewLocation);
    }
  }

  void CEActor::SetActorRotation(const glm::vec3& NewRotation)
  {
    if (m_RootComponent && !m_RootComponent->GetParent())
    {
      m_RootComponent->SetRotation(NewRotation);
    }
  }

  void CEActor::SetActorScale(const float& NewScale)
  {
    SetActorScale(glm::vec3(NewScale, NewScale, NewScale));
  }

  void CEActor::SetActorScale(const glm::vec3& NewScale)
  {
    if (m_RootComponent && !m_RootComponent->GetParent())
    {
      m_RootComponent->SetScale(NewScale);
    }
  }

  glm::vec3 CEActor::GetActorLocation() const
  {
    return m_RootComponent ? m_RootComponent->GetWorldLocation() : glm::vec3(0.0f);
  }

  glm::vec3 CEActor::GetActorRotation() const
  {
    return m_RootComponent ? m_RootComponent->GetRotation() : glm::vec3(0.0f);
  }

  glm::vec3 CEActor::GetActorScale() const
  {
    return m_RootComponent ? m_RootComponent->GetScale() : glm::vec3(1.0f);
  }

  void CEActor::BeginPlay()
  {
    CEObject::BeginPlay();

    ForEachComponent<CollisionComponent>([this](CollisionComponent* comp)
                                         {
        if (comp->IsCollisionEnabled())
        {
            CollisionSystem::Get().RegisterCollisionComponent(comp);
        } });
  }

  void CEActor::Update(float DeltaTime)
  {
    CEObject::Update(DeltaTime);

    // Автоматическая проверка коллизий для всех компонентов актора
    ForEachComponent<CollisionComponent>([this](CollisionComponent* comp)
                                         {
        if (comp->GetGenerateOverlapEvents())
        {
            auto collisions = CollisionSystem::Get().CheckCollisions(comp);
            if (!collisions.empty())
            {
                // Вызываем событие столкновения
                OnComponentOverlap(comp, collisions);
            }
        } });
  }

  // Нужно добавить метод для обработки overlap событий
  void CEActor::OnComponentOverlap(CollisionComponent* Component, const std::vector<CollisionHitResult>& Hits)
  {
    for (const auto& hit : Hits)
    {
      CE_CORE_TRACE("Actor ", GetName(), " component ", Component->GetName(),
                    " overlapped with ", hit.HitComponent->GetName());

      // Здесь можно добавить пользовательскую логику обработки столкновения
    }
  }

  void CEActor::Tick(float DeltaTime)
  {
    Update(DeltaTime);
    if (GetActorLocation().y <= -1000.f)
    {
      this->SetActorLocation(GetActorLocation().x, 1000.f, GetActorLocation().z);  // this is stub for destoy actor
      // need add function to destroy actor...
      m_verticalVelocity = 0.f;
    }

    if (bIsGravityEnabled)
    {
      // Применяем гравитацию
      auto world = dynamic_cast<CELevel*>(GetOwner());
      if (world)
      {
        auto* level = static_cast<CELevel*>(world);
        const glm::vec3 gravity = level->GetGravity();
        if (m_RootComponent->GetParent() == nullptr)
        {
          m_verticalVelocity += gravity.y * DeltaTime * Weight;
          glm::vec3 currentLocation = m_RootComponent->GetWorldLocation();
          currentLocation += gravity * DeltaTime;
          m_RootComponent->SetPosition(currentLocation);
          // if have collision, stop falling
          ForEachComponent<CollisionComponent>([this](CollisionComponent* comp)
                                               {
                if (comp->IsCollisionEnabled())
                {
                    auto collisions = CollisionSystem::Get().CheckCollisions(comp);
                    if (!collisions.empty())
                    {
                        m_verticalVelocity = 0.0f;
                    }
                } });
        }
      }
    }

    if (!bIsStatic)
    {
      if (bIsUsePhysics)
      {
        if (bIsSimulatingPhysics)
        {
        }
      }
    }
  }

}  // namespace CE