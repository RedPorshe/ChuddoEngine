#pragma once
#include "Engine/Utils/Math/Vector3.h"
#include "Engine/Utils/Math/Ray.h"
#include <memory>
#include <vector>

#include "Engine/Core/Object.h"

class CActor;


class CLevel : public CObject
{
 public:
  CLevel(CObject* Owner = nullptr, FString LevelName = "Level");
  virtual ~CLevel();

  template <typename T, typename... Args>
  T* SpawnActor(Args&&... args);

  void DestroyActor(CActor* Actor);
  CActor* FindActorByName(const FString& Name);

  const std::vector<std::unique_ptr<CActor>>& GetActors() const
  {
    return m_Actors;
  }



  virtual void BeginPlay() override;
  virtual void Update(float DeltaTime) override;
  virtual void Tick(float DeltaTime) override;

 protected:
  std::vector<std::unique_ptr<CActor>> m_Actors;



};

#include "Engine/GamePlay/Actors/Actor.h"

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
