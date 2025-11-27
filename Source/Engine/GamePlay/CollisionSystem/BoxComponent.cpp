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

  glm::vec3 thisMin = this->GetBoundingBoxMin();
  glm::vec3 thisMax = this->GetBoundingBoxMax();
  glm::vec3 otherMin = other->GetBoundingBoxMin();
  glm::vec3 otherMax = other->GetBoundingBoxMax();

  bool aabbCollision = (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
  (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
  (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z) ;
  if (!aabbCollision) return false;

  bool bisBlocking = (responseToOther == ECollisionResponse::Block) || (otherToThisResponse == ECollisionResponse::Block);
  bool bisOverlapin = (responseToOther == ECollisionResponse::Overlap) || (otherToThisResponse == ECollisionResponse::Overlap);
  return bisBlocking || bisOverlapin;
}

glm::vec3 CBoxComponent::GetBoundingBoxMin() const
{
  return this->GetWorldLocation() - m_Extents;
}

glm::vec3 CBoxComponent::GetBoundingBoxMax() const
{
  return this->GetWorldLocation()+ m_Extents;
}

}