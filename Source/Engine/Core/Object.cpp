#include "Engine/Core/Object.h"

#include "Engine/GamePlay/Components/Base/Component.h"

CObject::CObject(CObject* Owner, FString NewName)
    : m_Owner(Owner), m_Name(NewName)
{
}

CObject::~CObject()
{
}

CObject* CObject::GetOwner() const
{
  return m_Owner;
}

void CObject::SetOwner(CObject* Owner)
{
  m_Owner = Owner;
}

void CObject::BeginPlay()
{
}

void CObject::Update(float DeltaTime)
{
  for (auto& [name, component] : m_Components)
  {
    component->Update(DeltaTime);
  }
}

void CObject::Tick(float DeltaTime)
{
  Update(DeltaTime);
}
