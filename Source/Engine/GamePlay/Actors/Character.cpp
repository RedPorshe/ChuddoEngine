#include "Engine/GamePlay/Actors/Character.h"
#include "Engine/GamePlay/Components/StaticMeshComponent.h"
#include "Engine/GamePlay/World/Levels/Level.h"

namespace CE
{
  CCharacter::CCharacter(CObject* Owner, FString NewName)
      : CPawn(Owner, NewName), m_VerticalVelocity(0.0f), m_IsOnGround(false)
  {
    m_MeshComponent = AddDefaultSubObject<CStaticMeshComponent>("mesh", this);
    m_MeshComponent->SetMesh("Assets/meshes/Icosahedron.obj");
    m_MeshComponent->SetPosition(0.f, 0.f, 0.f);
    m_MeshComponent->SetScale(1.f);

    m_CapsuleComponent = AddDefaultSubObject<CCapsuleComponent>("capsule", this);
    m_CapsuleComponent->SetRadius(0.5f);
    m_CapsuleComponent->SetHalfHeight(1.0f);

    m_IsOnGround = false;
  }

  void CCharacter::BeginPlay()
  {
    CPawn::BeginPlay();
  }

  void CCharacter::Tick(float DeltaTime)
  {
    CPawn::Tick(DeltaTime);

    // Apply gravity
    if (!m_IsOnGround)
    {
      m_VerticalVelocity += GetLevel()->GetGravity().z * DeltaTime;
    }

    // Ground check using raycast
    Math::Vector3f currentLocation = GetActorLocation();
    Math::Ray groundRay(currentLocation, Math::Vector3f(0.0f, 0.0f, -1.0f));
    FRaycastHit hit;
    float groundCheckDistance = 1.1f; // Slightly more than capsule half height

    if (GetLevel()->Raycast(groundRay, hit, groundCheckDistance))
    {
      // On ground
      m_IsOnGround = true;
      m_VerticalVelocity = 0.0f;
      // Snap to ground
      float groundZ = hit.Location.z;
      float newZ = groundZ + m_CapsuleComponent->GetHalfHeight();
      SetActorLocation(Math::Vector3f(currentLocation.x, currentLocation.y, newZ));
    }
    else
    {
      // In air
      m_IsOnGround = false;
      // Apply vertical movement
      Math::Vector3f newLocation = currentLocation;
      newLocation.z += m_VerticalVelocity * DeltaTime;
      SetActorLocation(newLocation);
    }
  }

  void CCharacter::SetupPlayerInputComponent()
  {
    CPawn::SetupPlayerInputComponent();

    if (GetInputComponent())
    {
    }
  }
}  // namespace CE
