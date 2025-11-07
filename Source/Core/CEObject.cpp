#include "Core/CEObject.h"

#include "Components/Base/Component.h"

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

  void CEObject::BeginPlay()
  {
  }

  void CEObject::Update(float DeltaTime)
  {
    for (auto& [name, component] : m_Components)
    {
      component->Update(DeltaTime);
      FString ownerName = m_Owner ? m_Owner->GetName() : "None";
      CE_CORE_DEBUG("Updating: ", component->GetName(), " for ", ownerName, " DeltaTime: ", DeltaTime);
    }
  }

  void CEObject::Tick(float DeltaTime)
  {
    Update(DeltaTime);
  }
}  // namespace CE