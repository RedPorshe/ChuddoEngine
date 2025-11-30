#pragma once

#include "Engine/Core/Object.h"
#include "Engine/Core/Reflection.h"
#include "Engine/GamePlay/Components/Base/Component.h"
#include "Engine/GamePlay/Components/SceneComponent.h"
#include "Engine/GamePlay/World/Levels/Level.h"
#include "Engine/Utils/Math/AllMath.h"

class CMeshComponent;

class CActor : public CObject
  {
   public:
    CActor(CObject* Owner = nullptr, FString NewName = "Actor");
    virtual ~CActor() = default;

    // UE-совместимый SpawnActor
    template <typename T, typename... Args>
    T* SpawnActor(Args&&... args);

    // С авто-аттачем
    template <typename T, typename... Args>
    T* AddDefaultSubObject(const std::string& Name, Args&&... args);

    void SetRootComponent(CSceneComponent* Root)
    {
      m_RootComponent = Root;
    }
    CSceneComponent* GetRootComponent() const
    {
      return m_RootComponent;
    }

    // World transform
    void SetActorLocation(const Math::Vector3f& NewLocation);
    void SetActorLocation(const float x, const float y, const float z);
    void SetActorRotation(const Math::Vector3f& NewRotation);
    void SetActorScale(const float& NewScale);
    void SetActorScale(const Math::Vector3f& NewScale);

    Math::Vector3f GetActorLocation() const;
    Math::Vector3f GetActorRotation() const;
    Math::Vector3f GetActorScale() const;

    Math::Vector3f GetActorForwardVector() const;
    Math::Vector3f GetActorRightVector() const;
    Math::Vector3f GetActorUpVector() const;
    // Получение уровня
    CLevel* GetLevel() const;

    virtual void BeginPlay() override;
    virtual void Update(float DeltaTime) override;
    virtual void Tick(float DeltaTime) override;

    Math::Vector3f Location = Math::Vector3f(0.0f);
    Math::Vector3f Rotation = Math::Vector3f(0.0f);
    Math::Vector3f Scale = Math::Vector3f(1.0f);

  protected:
    CSceneComponent* m_RootComponent = nullptr;


  };

  template <typename T, typename... Args>
  T* CActor::SpawnActor(Args&&... args)
  {
    if (CLevel* Level = GetLevel())
    {
      return Level->template SpawnActor<T>(std::forward<Args>(args)...);
    }
    CE_CORE_ERROR("Cannot spawn actor: No level found!");
    return nullptr;
  }

  template <typename T, typename... Args>
  T* CActor::AddDefaultSubObject(const std::string& Name, Args&&... args)
  {
    auto* component = AddSubObject<T>(Name, std::forward<Args>(args)...);

    // Авто-аттач только для SceneComponent
    if (m_RootComponent && component != m_RootComponent)
    {
      if (auto* sceneComp = dynamic_cast<CSceneComponent*>(component))
      {
        sceneComp->AttachToComponent(m_RootComponent);
        CE_CORE_DEBUG("Auto-attached ", Name, " to root component");
      }
    }

    return component;
  }
