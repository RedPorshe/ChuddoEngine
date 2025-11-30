#include "Engine/GamePlay/World/Levels/Level.h"

namespace CE
{
  CLevel::CLevel(CObject* Owner, FString LevelName)
      : CObject(Owner, LevelName)
  {
    CE_CORE_DEBUG("Level created: ", LevelName);
  }

  CLevel::~CLevel()
  {
    CE_CORE_DEBUG("Level destroyed: ", GetName());
  }

  void CLevel::DestroyActor(CActor* Actor)
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

  CActor* CLevel::FindActorByName(const FString& Name)
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

  

  void CLevel::BeginPlay()
  {
    CObject::BeginPlay();

    for (auto& actor : m_Actors)
    {
      actor->BeginPlay();
    }
  }

  void CLevel::Update(float DeltaTime)
  {
    CObject::Update(DeltaTime);

    for (auto& actor : m_Actors)
    {
      actor->Update(DeltaTime);
    }
  }

  void CLevel::Tick(float DeltaTime)
  {
   
    Update(DeltaTime);
    

   
    for (auto& actor : m_Actors)
    {
     
      actor->Tick(DeltaTime);
      
    }
  }


}  // namespace CE
