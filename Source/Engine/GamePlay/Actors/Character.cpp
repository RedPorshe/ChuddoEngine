#include "Engine/GamePlay/Actors/Character.h"


#include "Engine/GamePlay/Components/StaticMeshComponent.h"


namespace CE
{
  CECharacter::CECharacter(CObject* Owner, FString NewName)
      : CEPawn(Owner, NewName), m_VerticalVelocity(0.0f), m_IsOnGround(false)
  {
    
   

    m_MeshComponent = AddDefaultSubObject<CEStaticMeshComponent>("mesh", this);
    m_MeshComponent->SetMesh("Assets/meshes/Icosahedron.obj");
   
    m_MeshComponent->SetPosition(0.f, 0.f, 0.f);
    m_MeshComponent->SetScale(1.f);

    m_IsOnGround = false;
  }

  void CECharacter::BeginPlay()
  {
    CEPawn::BeginPlay();
  }

  void CECharacter::Tick(float DeltaTime)
  {
    CEPawn::Tick(DeltaTime);
  }

  void CECharacter::SetupPlayerInputComponent()
  {
    CEPawn::SetupPlayerInputComponent();

    if (GetInputComponent())
    {
    }
  }
}  // namespace CE