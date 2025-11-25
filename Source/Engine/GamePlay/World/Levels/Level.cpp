#include "Engine/GamePlay/World/Levels/Level.h"

namespace CE
{
  CELevel::CELevel(CObject* Owner, FString LevelName)
      : CObject(Owner, LevelName)
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
    CObject::BeginPlay();

    for (auto& actor : m_Actors)
    {
      actor->BeginPlay();
    }
  }

  void CELevel::Update(float DeltaTime)
  {
    CObject::Update(DeltaTime);

    for (auto& actor : m_Actors)
    {
      actor->Update(DeltaTime);
    }
  }

  void CELevel::Tick(float DeltaTime)
  {
    Update(DeltaTime);
    for (auto& actor : m_Actors)
    {
      actor->Tick(DeltaTime);
    }
  }
}  // namespace CE