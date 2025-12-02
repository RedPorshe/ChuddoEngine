#include "Engine/GamePlay/Components/CameraComponent.h"

#include "Engine/Core/CoreTypes.h"

#include "Engine/GamePlay/Components/SpringArmComponent.h"


  CCameraComponent::CCameraComponent(CObject* Owner, FString NewName)
      : CSceneComponent(Owner, NewName)
  {
  }

  FVector CCameraComponent::GetCameraForwardVector() const
  {
    if (GetParent())
    {
      auto forward = GetParent()->GetForwardVector();

      return forward;
    }
    auto forward = GetForwardVector();

    return forward;
  }

  FVector CCameraComponent::GetCameraRightVector() const
  {
    if (GetParent())
    {
      return GetParent()->GetRightVector();
    }
    return GetRightVector();
  }

  FVector CCameraComponent::GetCameraUpVector() const
  {
    if (GetParent())
    {
      return GetParent()->GetUpVector();
    }
    return GetUpVector();
  }

  FMatrix CCameraComponent::GetViewMatrix() const
{


    if (auto* springArm = dynamic_cast<CSpringArmComponent*>(GetParent()))
    {
        FVector worldPos = springArm->GetCameraWorldLocation();
        FVector targetPos = springArm->GetWorldLocation() + springArm->GetTargetOffset();
        FVector up = springArm->GetUpVector();


        FMatrix view = FMatrix::LookAt(worldPos, targetPos, up);

        return view;
    }
    else
    {
        FVector worldPos = GetWorldLocation();
        FVector forward = GetCameraForwardVector();
        FVector up = GetCameraUpVector();

        FMatrix view = FMatrix::LookAt(worldPos, worldPos + forward, up);

        return view;
    }
}


void CCameraComponent::DebugMatrix(const FMatrix& m, const char* name) const
{
    CORE_DEBUG(name," Matrix:");
    for (int i = 0; i < 4; i++) {
        CORE_DEBUG("  [",m(i,0), " , ", m(i,1), ",",m(i,2), ",",m(i,3), "]");
    }
    CORE_DEBUG("  Determinant: ", m.Determinant());
}

void CCameraComponent::DebugMatrix(const char* message) const
{
    CORE_DEBUG( message);
}

  FMatrix CCameraComponent::GetProjectionMatrix() const
{
    FMatrix projection = FMatrix::Perspective(
        CEMath::ToRadians(m_FieldOfView),
        m_AspectRatio,
        m_NearPlane,
        m_FarPlane);

  
    
    return projection;
}

  void CCameraComponent::Update(float DeltaTime)
  {
    CSceneComponent::Update(DeltaTime);
  }