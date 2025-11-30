#include "Engine/GamePlay/Components/CameraComponent.h"

#include "Engine/Utils/Math/AllMath.h"

#include "Engine/GamePlay/Components/SpringArmComponent.h"


  CCameraComponent::CCameraComponent(CObject* Owner, FString NewName)
      : CSceneComponent(Owner, NewName)
  {
  }

  CEMath::Vector3f CCameraComponent::GetCameraForwardVector() const
  {
    if (GetParent())
    {
      auto forward = GetParent()->GetForwardVector();

      return forward;
    }
    auto forward = GetForwardVector();

    return forward;
  }

  CEMath::Vector3f CCameraComponent::GetCameraRightVector() const
  {
    if (GetParent())
    {
      return GetParent()->GetRightVector();
    }
    return GetRightVector();
  }

  CEMath::Vector3f CCameraComponent::GetCameraUpVector() const
  {
    if (GetParent())
    {
      return GetParent()->GetUpVector();
    }
    return GetUpVector();
  }

  CEMath::Matrix4f CCameraComponent::GetViewMatrix() const
{


    if (auto* springArm = dynamic_cast<CSpringArmComponent*>(GetParent()))
    {
        CEMath::Vector3f worldPos = springArm->GetCameraWorldLocation();
        CEMath::Vector3f targetPos = springArm->GetWorldLocation() + springArm->GetTargetOffset();
        CEMath::Vector3f up = springArm->GetUpVector();


        CEMath::Matrix4f view = CEMath::Matrix4f::LookAt(worldPos, targetPos, up);

        return view;
    }
    else
    {
        CEMath::Vector3f worldPos = GetWorldLocation();
        CEMath::Vector3f forward = GetCameraForwardVector();
        CEMath::Vector3f up = GetCameraUpVector();

        CEMath::Matrix4f view = CEMath::Matrix4f::LookAt(worldPos, worldPos + forward, up);

        return view;
    }
}


void CCameraComponent::DebugMatrix(const CEMath::Matrix4f& m, const char* name) const
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

  CEMath::Matrix4f CCameraComponent::GetProjectionMatrix() const
{
    CEMath::Matrix4f projection = CEMath::Matrix4f::Perspective(
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