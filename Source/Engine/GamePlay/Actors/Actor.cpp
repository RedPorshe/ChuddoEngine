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

FVector CActor::GetActorForwardVector() const
{
  return m_RootComponent ? m_RootComponent->GetForwardVector() : FVector(0.0f, 0.0f, 1.0f);
}
FVector CActor::GetActorRightVector() const
{
  return m_RootComponent ? m_RootComponent->GetRightVector() : FVector(1.0f, 0.0f, 0.0f);
}

FVector CActor::GetActorUpVector() const
{
  return m_RootComponent ? m_RootComponent->GetUpVector() : FVector(0.0f, 1.0f, 0.0f);
}

void CActor::SetActorLocation(const float x, const float y, const float z)
{
  SetActorLocation(FVector(x, y, z));
}

void CActor::SetActorLocation(const FVector& NewLocation)
{
  if (m_RootComponent && !m_RootComponent->GetParent())
  {
    m_RootComponent->SetPosition(NewLocation);
  }
}

void CActor::SetActorRotation(const FVector& NewRotation)
{
  if (m_RootComponent && !m_RootComponent->GetParent())
  {
    m_RootComponent->SetRotation(NewRotation);
  }
}

void CActor::SetActorScale(const float& NewScale)
{
  SetActorScale(FVector(NewScale, NewScale, NewScale));
}

void CActor::SetActorScale(const FVector& NewScale)
{
  if (m_RootComponent && !m_RootComponent->GetParent())
  {
    m_RootComponent->SetScale(NewScale);
  }
}

FVector CActor::GetActorLocation() const
{
  return m_RootComponent ? m_RootComponent->GetWorldLocation() : FVector(0.0f);
}

FVector CActor::GetActorRotation() const
{
  return m_RootComponent ? m_RootComponent->GetRotation() : FVector(0.0f);
}

FVector CActor::GetActorScale() const
{
  return m_RootComponent ? m_RootComponent->GetScale() : FVector(1.0f);
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
