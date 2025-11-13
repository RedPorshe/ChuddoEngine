#pragma once

#include "Engine/Core/CEObject.h"
#include "Engine/GamePlay/Components/CollisionComponent.h"
#include "Engine/GamePlay/Components/SceneComponent.h"
#include "Engine/GamePlay/World/Levels/CELevel.h"

namespace CE
{

  class CEActor : public CEObject
  {
   public:
    CEActor(CEObject* Owner = nullptr, FString NewName = "Actor");
    virtual ~CEActor() = default;

    // UE-совместимый SpawnActor
    template <typename T, typename... Args>
    T* SpawnActor(Args&&... args);

    // С авто-аттачем
    template <typename T, typename... Args>
    T* AddDefaultSubObject(const std::string& Name, Args&&... args);

    void SetRootComponent(SceneComponent* Root)
    {
      m_RootComponent = Root;
    }
    SceneComponent* GetRootComponent() const
    {
      return m_RootComponent;
    }

    // World transform
    void SetActorLocation(const glm::vec3& NewLocation);
    void SetActorLocation(const float x, const float y, const float z);
    void SetActorRotation(const glm::vec3& NewRotation);
    void SetActorScale(const glm::vec3& NewScale);

    glm::vec3 GetActorLocation() const;
    glm::vec3 GetActorRotation() const;
    glm::vec3 GetActorScale() const;

    // Получение уровня
    CELevel* GetLevel() const;

    virtual void BeginPlay() override;
    virtual void Update(float DeltaTime) override;
    virtual void Tick(float DeltaTime) override;
    void SetUsePhysics(bool value)
    {
      bIsUsePhysics = value;
    }

   protected:
    SceneComponent* m_RootComponent = nullptr;

    bool bIsUsePhysics{false};
    virtual void OnComponentOverlap(class CollisionComponent* Component,
                                    const std::vector<struct CollisionHitResult>& Hits);
  };

  template <typename T, typename... Args>
  T* CEActor::SpawnActor(Args&&... args)
  {
    if (CELevel* Level = GetLevel())
    {
      return Level->template SpawnActor<T>(std::forward<Args>(args)...);
    }
    CE_CORE_ERROR("Cannot spawn actor: No level found!");
    return nullptr;
  }

  template <typename T, typename... Args>
  T* CEActor::AddDefaultSubObject(const std::string& Name, Args&&... args)
  {
    auto* component = AddSubObject<T>(Name, std::forward<Args>(args)...);

    // Авто-аттач только для SceneComponent
    if (m_RootComponent && component != m_RootComponent)
    {
      if (auto* sceneComp = dynamic_cast<SceneComponent*>(component))
      {
        sceneComp->AttachToComponent(m_RootComponent);
        CE_CORE_DEBUG("Auto-attached ", Name, " to ", m_RootComponent->GetName());
      }
    }

    return component;
  }
}  // namespace CE
