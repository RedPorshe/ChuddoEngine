#pragma once
#include "Engine/Utils/Math/Vector3.h"
#include "Engine/Utils/Math/Ray.h"
#include <memory>
#include <vector>

#include "Engine/Core/Object.h"

namespace CE
{

  class CActor;
  class CCollisionComponent;

  struct FRaycastHit
  {
      CCollisionComponent* Component = nullptr;
      Math::Vector3f Location;
      Math::Vector3f Normal;
      float Distance = 0.0f;
      bool bBlockingHit = false;
  };

  class CLevel : public CObject
  {
   public:
    CLevel(CObject* Owner = nullptr, FString LevelName = "Level");
    virtual ~CLevel();

    template <typename T, typename... Args>
    T* SpawnActor(Args&&... args);

    void DestroyActor(CActor* Actor);
    CActor* FindActorByName(const FString& Name);
    Math::Vector3f GetGravity() const
    {
      return m_gravity;
    }
    void SetGravity(const Math::Vector3f& gravity)
    {
      m_gravity = gravity;
    }
    const std::vector<std::unique_ptr<CActor>>& GetActors() const
    {
      return m_Actors;
    }

    // Raycast method
    bool Raycast(const Math::Ray& ray, FRaycastHit& outHit, float maxDistance = 1000.0f);

    virtual void BeginPlay() override;
    virtual void Update(float DeltaTime) override;
    virtual void Tick(float DeltaTime) override;

   protected:
    std::vector<std::unique_ptr<CActor>> m_Actors;
    Math::Vector3f m_gravity{0.0f, -9.81f, 0.0f};
  };

}  // namespace CE

#include "Engine/GamePlay/Actors/Actor.h"

namespace CE
{
  template <typename T, typename... Args>
  T* CLevel::SpawnActor(Args&&... args)
  {
    static_assert(std::is_base_of_v<CActor, T>, "T must be derived from CEActor");

    auto actor = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = actor.get();
    m_Actors.push_back(std::move(actor));

    ptr->BeginPlay();
    CE_CORE_DEBUG("Spawned actor: ", ptr->GetName(), " in level: ", GetName());

    return ptr;
  }
}  // namespace CE
