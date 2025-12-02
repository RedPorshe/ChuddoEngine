#pragma once

#include "Engine/Core/Object.h"
#include "Engine/Core/Reflection.h"
#include "Engine/GamePlay/Components/Base/Component.h"
#include "Engine/GamePlay/Components/SceneComponent.h"
#include "Engine/GamePlay/World/Levels/Level.h"
#include "Engine/Core/CoreTypes.h"

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
    void SetActorLocation(const FVector& NewLocation);
    void SetActorLocation(const float x, const float y, const float z);
    void SetActorRotation(const FVector& NewRotation);
    void SetActorScale(const float& NewScale);
    void SetActorScale(const FVector& NewScale);

    FVector GetActorLocation() const;
    FVector GetActorRotation() const;
    FVector GetActorScale() const;

    FVector GetActorForwardVector() const;
    FVector GetActorRightVector() const;
    FVector GetActorUpVector() const;
    // Получение уровня
    CLevel* GetLevel() const;

    virtual void BeginPlay() override;
    virtual void Update(float DeltaTime) override;
    virtual void Tick(float DeltaTime) override;

    FVector Location = FVector(0.0f);
    FVector Rotation = FVector(0.0f);
    FVector Scale = FVector(1.0f);

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
    CORE_ERROR("Cannot spawn actor: No level found!");
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
        CORE_DEBUG("Auto-attached ", Name, " to root component");
      }
    }

    return component;
  }
