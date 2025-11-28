#include "Engine/GamePlay/CollisionSystem/SphereComponent.h"

namespace CE
{
    CSphereComponent::CSphereComponent(CObject* Owner, FString Name) : CCollisionComponent(Owner, Name)
    {
    }

    bool CSphereComponent::CheckCollision(const CCollisionComponent* other) const
    {
        if (this == other) return false;
        if (!other || !this->bIsCollisionEnabled || !other->IsCollisionEnabled()) return false;
        ECollisionResponse responseToOther = GetCollisionResponseToChannel(other->GetCollisionChannel());
        ECollisionResponse otherToThisResponse = other->GetCollisionResponseToChannel(this->m_CollisionChannel);

        if (responseToOther == ECollisionResponse::Ignore || otherToThisResponse == ECollisionResponse::Ignore) { return false; }

        // For simplicity, use AABB approximation for sphere
        Math::Vector3f thisMin = this->GetBoundingBoxMin();
        Math::Vector3f thisMax = this->GetBoundingBoxMax();
        Math::Vector3f otherMin = other->GetBoundingBoxMin();
        Math::Vector3f otherMax = other->GetBoundingBoxMax();

        bool aabbCollision = (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
                             (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
                             (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z);
        if (!aabbCollision) return false;

        bool isBlocking = (responseToOther == ECollisionResponse::Block) || (otherToThisResponse == ECollisionResponse::Block);
        bool isOverlapping = (responseToOther == ECollisionResponse::Overlap) || (otherToThisResponse == ECollisionResponse::Overlap);
        return isBlocking || isOverlapping;
    }

    Math::Vector3f CSphereComponent::GetBoundingBoxMin() const
    {
        Math::Vector3f location = this->GetWorldLocation();
        return Math::Vector3f(location.x - m_Radius, location.y - m_Radius, location.z - m_Radius);
    }

    Math::Vector3f CSphereComponent::GetBoundingBoxMax() const
    {
        Math::Vector3f location = this->GetWorldLocation();
        return Math::Vector3f(location.x + m_Radius, location.y + m_Radius, location.z + m_Radius);
    }
}  // namespace CE
