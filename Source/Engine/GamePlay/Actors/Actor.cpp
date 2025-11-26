#include "Engine/GamePlay/Actors/Actor.h"



namespace CE
{
  CActor::CActor(CObject* Owner, FString NewName)
      : CObject(Owner, NewName)
  {
    // Создаем root component
    m_RootComponent = AddDefaultSubObject<CSceneComponent>("root", this, "RootComponent");
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

  glm::vec3 CActor::GetActorForwardVector() const
  {
    return m_RootComponent ? m_RootComponent->GetForwardVector() : glm::vec3(0.0f, 0.0f, 1.0f);
  }
  glm::vec3 CActor::GetActorRightVector() const
  {
    return m_RootComponent ? m_RootComponent->GetRightVector() : glm::vec3(1.0f, 0.0f, 0.0f);
  }

  glm::vec3 CActor::GetActorUpVector() const
  {
    return m_RootComponent ? m_RootComponent->GetUpVector() : glm::vec3(0.0f, 1.0f, 0.0f);
  }

  void CActor::SetActorLocation(const float x, const float y, const float z)
  {
    SetActorLocation(glm::vec3(x, y, z));
  }

  void CActor::SetActorLocation(const glm::vec3& NewLocation)
  {
    if (m_RootComponent && !m_RootComponent->GetParent())
    {
      m_RootComponent->SetPosition(NewLocation);
    }
  }

  void CActor::SetActorRotation(const glm::vec3& NewRotation)
  {
    if (m_RootComponent && !m_RootComponent->GetParent())
    {
      m_RootComponent->SetRotation(NewRotation);
    }
  }

  void CActor::SetActorScale(const float& NewScale)
  {
    SetActorScale(glm::vec3(NewScale, NewScale, NewScale));
  }

  void CActor::SetActorScale(const glm::vec3& NewScale)
  {
    if (m_RootComponent && !m_RootComponent->GetParent())
    {
      m_RootComponent->SetScale(NewScale);
    }
  }

  glm::vec3 CActor::GetActorLocation() const
  {
    return m_RootComponent ? m_RootComponent->GetWorldLocation() : glm::vec3(0.0f);
  }

  glm::vec3 CActor::GetActorRotation() const
  {
    return m_RootComponent ? m_RootComponent->GetRotation() : glm::vec3(0.0f);
  }

  glm::vec3 CActor::GetActorScale() const
  {
    return m_RootComponent ? m_RootComponent->GetScale() : glm::vec3(1.0f);
  }

  void CActor::BeginPlay()
  {
    CObject::BeginPlay();

   
  }

  void CActor::Update(float DeltaTime)
  {
    CObject::Update(DeltaTime);

    if (bIsGravityEnabled)
    {
      // Применяем гравитацию
      auto world = dynamic_cast<CLevel*>(GetOwner());
      if (world)
      {
        auto* level = static_cast<CLevel*>(world);
        const glm::vec3 gravity = level->GetGravity();
        if (m_RootComponent->GetParent() == nullptr)
        {
          m_verticalVelocity += gravity.y * DeltaTime * Weight;
          glm::vec3 currentLocation = m_RootComponent->GetWorldLocation();
          currentLocation += gravity * DeltaTime;
          m_RootComponent->SetPosition(currentLocation);
          
        }
      }
    }
   
  }

 

  void CActor::Tick(float DeltaTime)
  {
    Update(DeltaTime);
    if (GetActorLocation().y <= -1000.f)
    {
      this->SetActorLocation(GetActorLocation().x, 1000.f, GetActorLocation().z);  // this is stub for destoy actor
      // need add function to destroy actor...
      m_verticalVelocity = 0.f;
    }

        if (!bIsStatic)
    {
      if (bIsUsePhysics)
      {
        if (bIsSimulatingPhysics)
        {
        }
      }
    }
  }

}  // namespace CE