#include "Core/CEObject.h"

#include "GamePlay/Components/Base/Component.h"

namespace CE
{
  CEObject::CEObject(CEObject* Owner, FString NewName)
      : m_Owner(Owner), m_Name(NewName)
  {
  }

  CEObject::~CEObject()
  {
  }

  CEObject* CEObject::GetOwner() const
  {
    return m_Owner;
  }

  void CEObject::SetOwner(CEObject* Owner)
  {
    m_Owner = Owner;
  }

  void CEObject::BeginPlay()
  {
  }

  void CEObject::Update(float DeltaTime)
  {
    for (auto& [name, component] : m_Components)
    {
      component->Update(DeltaTime);
    }
  }

  void CEObject::Tick(float DeltaTime)
  {
    Update(DeltaTime);
  }
}  // namespace CE