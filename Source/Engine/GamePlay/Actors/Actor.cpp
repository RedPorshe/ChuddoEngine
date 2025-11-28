#include "Engine/GamePlay/Actors/Actor.h"
#include "Engine/GamePlay/World/Levels/Level.h"
#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/GamePlay/CollisionSystem/CollisionComponent.h"
#include "Engine/GamePlay/CollisionSystem/TerrainCollisionComponent.h"
#include "Engine/Core/Object.h"
#include "Engine/Utils/Math/AllMath.h"
#include "Engine/GamePlay/Components/PhysicsComponent.h"
#include "Engine/Utils/Logger.h"
#include <algorithm>
#include <cfloat>



namespace CE
{
  CActor::CActor(CObject* Owner, FString NewName)
      : CObject(Owner, NewName)
  {
    // Создаем root component
   
    CE_CORE_DEBUG("CEActor created: ", NewName);
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

  Math::Vector3f CActor::GetActorForwardVector() const
  {
    return m_RootComponent ? m_RootComponent->GetForwardVector() : Math::Vector3f(0.0f, 0.0f, 1.0f);
  }
  Math::Vector3f CActor::GetActorRightVector() const
  {
    return m_RootComponent ? m_RootComponent->GetRightVector() : Math::Vector3f(1.0f, 0.0f, 0.0f);
  }

  Math::Vector3f CActor::GetActorUpVector() const
  {
    return m_RootComponent ? m_RootComponent->GetUpVector() : Math::Vector3f(0.0f, 1.0f, 0.0f);
  }

  void CActor::SetActorLocation(const float x, const float y, const float z)
  {
    SetActorLocation(Math::Vector3f(x, y, z));
  }

  void CActor::SetActorLocation(const Math::Vector3f& NewLocation)
  {
    if (m_RootComponent && !m_RootComponent->GetParent())
    {
      m_RootComponent->SetPosition(NewLocation);
    }
  }

  void CActor::SetActorRotation(const Math::Vector3f& NewRotation)
  {
    if (m_RootComponent && !m_RootComponent->GetParent())
    {
      m_RootComponent->SetRotation(NewRotation);
    }
  }

  void CActor::SetActorScale(const float& NewScale)
  {
    SetActorScale(Math::Vector3f(NewScale, NewScale, NewScale));
  }

  void CActor::SetActorScale(const Math::Vector3f& NewScale)
  {
    if (m_RootComponent && !m_RootComponent->GetParent())
    {
      m_RootComponent->SetScale(NewScale);
    }
  }

  Math::Vector3f CActor::GetActorLocation() const
  {
    return m_RootComponent ? m_RootComponent->GetWorldLocation() : Math::Vector3f(0.0f);
  }

  Math::Vector3f CActor::GetActorRotation() const
  {
    return m_RootComponent ? m_RootComponent->GetRotation() : Math::Vector3f(0.0f);
  }

  Math::Vector3f CActor::GetActorScale() const
  {
    return m_RootComponent ? m_RootComponent->GetScale() : Math::Vector3f(1.0f);
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

    // Update physics components
    auto physicsComponents = GetComponents<CPhysicsComponent>();
    for (auto* physics : physicsComponents)
    {
      physics->Update(DeltaTime);
    }

    if (GetActorLocation().y <= -1000.f)
    {
      this->SetActorLocation(GetActorLocation().x, 100.f, GetActorLocation().z);  // this is stub for destroy actor
      // need add function to destroy actor...
    }
  }





}  // namespace CE
