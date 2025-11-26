#include "Engine/GamePlay/Actors/Character.h"


#include "Engine/GamePlay/Components/StaticMeshComponent.h"


namespace CE
{
  CCharacter::CCharacter(CObject* Owner, FString NewName)
      : CPawn(Owner, NewName), m_VerticalVelocity(0.0f), m_IsOnGround(false)
  {
    
   

    m_MeshComponent = AddDefaultSubObject<CStaticMeshComponent>("mesh", this);
    m_MeshComponent->SetMesh("Assets/meshes/Icosahedron.obj");
   
    m_MeshComponent->SetPosition(0.f, 0.f, 0.f);
    m_MeshComponent->SetScale(1.f);

    m_IsOnGround = false;
  }

  void CCharacter::BeginPlay()
  {
    CPawn::BeginPlay();
  }

  void CCharacter::Tick(float DeltaTime)
  {
    CPawn::Tick(DeltaTime);
  }

  void CCharacter::SetupPlayerInputComponent()
  {
    CPawn::SetupPlayerInputComponent();

    if (GetInputComponent())
    {
    }
  }
}  // namespace CE