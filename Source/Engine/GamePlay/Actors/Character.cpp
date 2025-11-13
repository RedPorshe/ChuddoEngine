#include "Engine/GamePlay/Actors/Character.h"

#include "Engine/Core/CollisionSystem.h"
#include "Engine/GamePlay/Components/CEStaticMeshComponent.h"
#include "Engine/GamePlay/Components/PlaneComponent.h"

namespace CE
{
  CECharacter::CECharacter(CEObject* Owner, FString NewName)
      : CEPawn(Owner, NewName), m_VerticalVelocity(0.0f), m_IsOnGround(false)
  {
    m_CapsuleComponent = AddDefaultSubObject<CECapsuleComponent>("Collision", this, "CapsuleComponent");
    m_CapsuleComponent->SetCollisionEnabled(true);
    m_CapsuleComponent->SetGenerateOverlapEvents(true);
    SetRootComponent(m_CapsuleComponent);
    m_CapsuleComponent->SetSize(0.35f, 1.8f);  // radius 35sm height 1.8m

    m_MeshComponent = AddDefaultSubObject<CEStaticMeshComponent>("mesh", this);
    m_MeshComponent->SetMesh("Assets/meshes/Icosahedron.obj");
    m_MeshComponent->AttachToComponent(m_CapsuleComponent);
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

    const float gravity = -9.8f;

    if (m_CapsuleComponent)
    {
      auto collisions = CollisionSystem::Get().CheckCollisions(m_CapsuleComponent);

      m_IsOnGround = false;

      for (const auto& hit : collisions)
      {
        if (hit.HitComponent && hit.bBlockingHit)
        {
          m_IsOnGround = true;

          break;
        }
      }

      if (!m_IsOnGround)
      {
        m_VerticalVelocity += gravity * DeltaTime;
      }
      else
      {
        if (m_VerticalVelocity < 0.0f)
        {
          m_VerticalVelocity = 0.0f;
        }
      }

      if (abs(m_VerticalVelocity) > 0.01f)
      {
        glm::vec3 movement(0.0f, m_VerticalVelocity * DeltaTime, 0.0f);
        m_CapsuleComponent->Move(movement);
      }

      if (GetActorLocation().y < -10.0f)
      {
        SetActorLocation(glm::vec3(0.0f, 5.0f, 5.0f));
        m_VerticalVelocity = 0.0f;
      }
    }
  }

  void CECharacter::SetupPlayerInputComponent()
  {
    CEPawn::SetupPlayerInputComponent();

    if (GetInputComponent())
    {
      // Биндинги управления для персонажа
      GetInputComponent()->BindAxis("MoveForward", [this](float Value)
                                    {
        if (m_CapsuleComponent && abs(Value) > 0.1f)
        {
          glm::vec3 movement = glm::vec3(0.0f, 0.0f, Value * .020f); // Движение вперед/назад
          m_CapsuleComponent->Move(movement);
        } });

      GetInputComponent()->BindAxis("MoveRight", [this](float Value)
                                    {
        if (m_CapsuleComponent && abs(Value) > 0.1f)
        {
          glm::vec3 movement = glm::vec3(Value * .020f, 0.0f, 0.0f); // Движение вправо/влево
          m_CapsuleComponent->Move(movement);
        } });

      GetInputComponent()->BindAction("Jump", EInputEvent::Pressed, [this]()
                                      {
        if (m_IsOnGround)
        {
          // Прыжок вверх (положительное значение Y)
          m_VerticalVelocity = 4.0f; // Импульс вверх
          CE_CORE_DEBUG("JUMP! Vertical velocity: ", m_VerticalVelocity);
        } });

      CE_CORE_DEBUG("Character input bindings setup complete");
    }
  }
}  // namespace CE