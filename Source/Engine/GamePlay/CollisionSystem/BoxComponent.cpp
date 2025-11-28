#include "Engine/GamePlay/CollisionSystem/BoxComponent.h"
#include <cfloat>

namespace CE
{
// Forward declaration
bool CheckOBBOverlap(const CBoxComponent* box1, const CBoxComponent* box2);
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

  // Check if other is also a BoxComponent for OBB collision
  const CBoxComponent* otherBox = dynamic_cast<const CBoxComponent*>(other);
  if (otherBox)
  {
    // Use OBB collision detection
    if (!CheckOBBOverlap(this, otherBox)) return false;
  }
  else
  {
    // Fall back to AABB for other collision types
    Math::Vector3f thisMin = this->GetBoundingBoxMin();
    Math::Vector3f thisMax = this->GetBoundingBoxMax();
    Math::Vector3f otherMin = other->GetBoundingBoxMin();
    Math::Vector3f otherMax = other->GetBoundingBoxMax();

    bool aabbCollision = (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
    (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
    (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z) ;
    if (!aabbCollision) return false;
  }

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

std::array<Math::Vector3f, 8> CBoxComponent::GetOrientedBoundingBoxCorners() const
{
  Math::Vector3f center = GetWorldLocation();
  Math::Vector3f extents = m_Extents;
  Math::Quaternionf rotation = GetRotationQuat();

  // Define the 8 corners of the box in local space
  std::array<Math::Vector3f, 8> localCorners = {
    Math::Vector3f(-extents.x, -extents.y, -extents.z),
    Math::Vector3f( extents.x, -extents.y, -extents.z),
    Math::Vector3f( extents.x,  extents.y, -extents.z),
    Math::Vector3f(-extents.x,  extents.y, -extents.z),
    Math::Vector3f(-extents.x, -extents.y,  extents.z),
    Math::Vector3f( extents.x, -extents.y,  extents.z),
    Math::Vector3f( extents.x,  extents.y,  extents.z),
    Math::Vector3f(-extents.x,  extents.y,  extents.z)
  };

  // Transform corners to world space
  std::array<Math::Vector3f, 8> worldCorners;
  for (size_t i = 0; i < 8; ++i)
  {
    worldCorners[i] = center + rotation * localCorners[i];
  }

  return worldCorners;
}

// Check if two oriented bounding boxes overlap using Separating Axis Theorem
bool CheckOBBOverlap(const CBoxComponent* box1, const CBoxComponent* box2)
{
  // Get the 8 corners of each box
  auto corners1 = box1->GetOrientedBoundingBoxCorners();
  auto corners2 = box2->GetOrientedBoundingBoxCorners();

  // Define the axes to test (face normals of both boxes)
  std::array<Math::Vector3f, 15> axes;

  // Face normals of box1
  Math::Vector3f center1 = box1->GetWorldLocation();
  Math::Vector3f extents1 = box1->GetExtents();
  Math::Quaternionf rot1 = box1->GetRotationQuat();

  axes[0] = rot1 * Math::Vector3f(1.0f, 0.0f, 0.0f); // X axis
  axes[1] = rot1 * Math::Vector3f(0.0f, 1.0f, 0.0f); // Y axis
  axes[2] = rot1 * Math::Vector3f(0.0f, 0.0f, 1.0f); // Z axis

  // Face normals of box2
  Math::Vector3f center2 = box2->GetWorldLocation();
  Math::Vector3f extents2 = box2->GetExtents();
  Math::Quaternionf rot2 = box2->GetRotationQuat();

  axes[3] = rot2 * Math::Vector3f(1.0f, 0.0f, 0.0f); // X axis
  axes[4] = rot2 * Math::Vector3f(0.0f, 1.0f, 0.0f); // Y axis
  axes[5] = rot2 * Math::Vector3f(0.0f, 0.0f, 1.0f); // Z axis

  // Cross products of axes (edge normals)
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      axes[6 + i * 3 + j] = axes[i].Cross(axes[3 + j]);
    }
  }

  // Test each axis
  for (const auto& axis : axes)
  {
    // Skip near-zero axes
    if (axis.LengthSquared() < 1e-6f) continue;

    Math::Vector3f normalizedAxis = axis.Normalized();

    // Project all corners onto the axis
    float min1 = FLT_MAX, max1 = -FLT_MAX;
    float min2 = FLT_MAX, max2 = -FLT_MAX;

    for (const auto& corner : corners1)
    {
      float proj = corner.Dot(normalizedAxis);
      min1 = std::min(min1, proj);
      max1 = std::max(max1, proj);
    }

    for (const auto& corner : corners2)
    {
      float proj = corner.Dot(normalizedAxis);
      min2 = std::min(min2, proj);
      max2 = std::max(max2, proj);
    }

    // Check for separation
    if (max1 < min2 || max2 < min1)
    {
      return false; // Separating axis found
    }
  }

  return true; // No separating axis found, boxes overlap
}

}
