#pragma once
#include <memory>
#include <vector>

#include "Engine/Core/CEObject.h"

namespace CE
{

  class CEActor;

  class CELevel : public CEObject
  {
   public:
    CELevel(CEObject* Owner = nullptr, FString LevelName = "Level");
    virtual ~CELevel();

    template <typename T, typename... Args>
    T* SpawnActor(Args&&... args);

    void DestroyActor(CEActor* Actor);
    CEActor* FindActorByName(const FString& Name);

    const std::vector<std::unique_ptr<CEActor>>& GetActors() const
    {
      return m_Actors;
    }

    virtual void BeginPlay() override;
    virtual void Update(float DeltaTime) override;
    virtual void Tick(float DeltaTime) override;

   protected:
    std::vector<std::unique_ptr<CEActor>> m_Actors;
  };

}  // namespace CE

#include "Engine/GamePlay/Actors/Actor.h"

namespace CE
{
  template <typename T, typename... Args>
  T* CELevel::SpawnActor(Args&&... args)
  {
    static_assert(std::is_base_of_v<CEActor, T>, "T must be derived from CEActor");

    auto actor = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = actor.get();
    m_Actors.push_back(std::move(actor));

    ptr->BeginPlay();
    CE_CORE_DEBUG("Spawned actor: ", ptr->GetName(), " in level: ", GetName());

    return ptr;
  }
}  // namespace CE