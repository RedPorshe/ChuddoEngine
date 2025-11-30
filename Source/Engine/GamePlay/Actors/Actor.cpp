#include "Engine/GamePlay/Actors/Actor.h"
#include "Engine/GamePlay/World/Levels/Level.h"
#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/Core/Object.h"
#include "Engine/Utils/Math/AllMath.h"
#include "Engine/Utils/Logger.h"
#include <algorithm>
#include <cfloat>



CActor::CActor(CObject* Owner, FString NewName)
    : CObject(Owner, NewName)
{
  CORE_DEBUG("CEActor created: ", NewName);
}

CLevel* CActor::GetLevel() const
{
  const CObject* current = this;
  while (current)
  {
    if (auto* level = dynamic_cast<const CLevel*>(current))
    {
      return const_cast<CLevel*>(level);
    }
    current = current->GetOwner();
  }
  return nullptr;
}

CEMath::Vector3f CActor::GetActorForwardVector() const
{
  return m_RootComponent ? m_RootComponent->GetForwardVector() : CEMath::Vector3f(0.0f, 0.0f, 1.0f);
}
CEMath::Vector3f CActor::GetActorRightVector() const
{
  return m_RootComponent ? m_RootComponent->GetRightVector() : CEMath::Vector3f(1.0f, 0.0f, 0.0f);
}

CEMath::Vector3f CActor::GetActorUpVector() const
{
  return m_RootComponent ? m_RootComponent->GetUpVector() : CEMath::Vector3f(0.0f, 1.0f, 0.0f);
}

void CActor::SetActorLocation(const float x, const float y, const float z)
{
  SetActorLocation(CEMath::Vector3f(x, y, z));
}

void CActor::SetActorLocation(const CEMath::Vector3f& NewLocation)
{
  if (m_RootComponent && !m_RootComponent->GetParent())
  {
    m_RootComponent->SetPosition(NewLocation);
  }
}

void CActor::SetActorRotation(const CEMath::Vector3f& NewRotation)
{
  if (m_RootComponent && !m_RootComponent->GetParent())
  {
    m_RootComponent->SetRotation(NewRotation);
  }
}

void CActor::SetActorScale(const float& NewScale)
{
  SetActorScale(CEMath::Vector3f(NewScale, NewScale, NewScale));
}

void CActor::SetActorScale(const CEMath::Vector3f& NewScale)
{
  if (m_RootComponent && !m_RootComponent->GetParent())
  {
    m_RootComponent->SetScale(NewScale);
  }
}

CEMath::Vector3f CActor::GetActorLocation() const
{
  return m_RootComponent ? m_RootComponent->GetWorldLocation() : CEMath::Vector3f(0.0f);
}

CEMath::Vector3f CActor::GetActorRotation() const
{
  return m_RootComponent ? m_RootComponent->GetRotation() : CEMath::Vector3f(0.0f);
}

CEMath::Vector3f CActor::GetActorScale() const
{
  return m_RootComponent ? m_RootComponent->GetScale() : CEMath::Vector3f(1.0f);
}

void CActor::BeginPlay()
{
  CObject::BeginPlay();


}

void CActor::Update(float DeltaTime)
{
  CObject::Update(DeltaTime);
}



void CActor::Tick(float DeltaTime)
{
  Update(DeltaTime);

  if (GetActorLocation().y <= -1000.f)
  {
    this->SetActorLocation(GetActorLocation().x, 100.f, GetActorLocation().z);
   // GetLevel()->DestroyActor(this);
  }
}
