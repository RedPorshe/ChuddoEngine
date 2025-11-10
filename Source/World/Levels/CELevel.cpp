#include "World/Levels/CELevel.h"

namespace CE
{
  CELevel::CELevel(CEObject* Owner, FString LevelName)
      : CEObject(Owner, LevelName)
  {
    CE_CORE_DEBUG("Level created: ", LevelName);
  }

  CELevel::~CELevel()
  {
    CE_CORE_DEBUG("Level destroyed: ", GetName());
  }

  void CELevel::DestroyActor(CEActor* Actor)
  {
    auto it = std::find_if(m_Actors.begin(), m_Actors.end(),
                           [Actor](const auto& ptr)
                           { return ptr.get() == Actor; });

    if (it != m_Actors.end())
    {
      CE_CORE_DEBUG("Destroying actor: ", Actor->GetName());
      m_Actors.erase(it);
    }
  }

  CEActor* CELevel::FindActorByName(const FString& Name)
  {
    for (auto& actor : m_Actors)
    {
      if (actor->GetName() == Name)
      {
        return actor.get();
      }
    }
    return nullptr;
  }

  void CELevel::BeginPlay()
  {
    CEObject::BeginPlay();
    CE_CORE_DEBUG("Level BeginPlay: ", GetName());

    // Вызываем BeginPlay для всех акторов
    for (auto& actor : m_Actors)
    {
      actor->BeginPlay();
    }
  }

  void CELevel::Update(float DeltaTime)
  {
    CEObject::Update(DeltaTime);

        for (auto& actor : m_Actors)
    {
      actor->Update(DeltaTime);
    }
  }

  void CELevel::Tick(float DeltaTime)
  {
    Update(DeltaTime);
  }
}  // namespace CE