#pragma once
#include "Components/Scene/SceneComponent.h"
#include "Core/CEObject.h"
#include "World/Levels/CELevel.h"

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

   protected:
    SceneComponent* m_RootComponent = nullptr;
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

    // Авто-аттач
    if (m_RootComponent && component != m_RootComponent)
    {
      if (auto* sceneComp = dynamic_cast<SceneComponent*>(component))
      {
        sceneComp->AttachToComponent(m_RootComponent);
        CE_CORE_DEBUG("Auto-attached ", Name, " to root component");
      }
    }

    return component;
  }
}  // namespace CE