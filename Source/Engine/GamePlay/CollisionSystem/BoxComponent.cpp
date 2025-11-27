#include "Engine/GamePlay/CollisionSystem/BoxComponent.h"

namespace CE
{
    CBoxComponent::CBoxComponent(CObject* Owner, FString Name) : CCollisionComponent(Owner,Name)
{
}

bool CBoxComponent::CheckCollision(const CCollisionComponent* other) const
{
  if (this == other) return false;
  if(!other || !this->bIsCollisionEnabled || !other->IsCollisionEnabled())  return false;
  ECollisionResponse responseToOther = GetCollisionResponseToChannel(other->GetCollisionChannel());
  ECollisionResponse otherToThisResponse = other->GetCollisionResponseToChannel(this->m_CollisionChannel);

  if (responseToOther == ECollisionResponse::Ignore || otherToThisResponse == ECollisionResponse::Ignore) { return false;}

  Math::Vector3f thisMin = this->GetBoundingBoxMin();
  Math::Vector3f thisMax = this->GetBoundingBoxMax();
  Math::Vector3f otherMin = other->GetBoundingBoxMin();
  Math::Vector3f otherMax = other->GetBoundingBoxMax();

  bool aabbCollision = (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
  (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
  (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z) ;
  if (!aabbCollision) return false;

  bool bisBlocking = (responseToOther == ECollisionResponse::Block) || (otherToThisResponse == ECollisionResponse::Block);
  bool bisOverlapin = (responseToOther == ECollisionResponse::Overlap) || (otherToThisResponse == ECollisionResponse::Overlap);
  return bisBlocking || bisOverlapin;
}

Math::Vector3f CBoxComponent::GetBoundingBoxMin() const
{
  return this->GetWorldLocation() - m_Extents;
}

Math::Vector3f CBoxComponent::GetBoundingBoxMax() const
{
  return this->GetWorldLocation()+ m_Extents;
}

}