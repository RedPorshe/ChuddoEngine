#include "Engine/GamePlay/Components/CameraComponent.h"

#include "Engine/Utils/Math/AllMath.h"

#include "Engine/GamePlay/Components/SpringArmComponent.h"

namespace CE
{
  CCameraComponent::CCameraComponent(CObject* Owner, FString NewName)
      : CSceneComponent(Owner, NewName)
  {
  }

  Math::Vector3f CCameraComponent::GetCameraForwardVector() const
  {
    if (GetParent())
    {
      auto forward = GetParent()->GetForwardVector();

      return forward;
    }
    auto forward = GetForwardVector();

    return forward;
  }

  Math::Vector3f CCameraComponent::GetCameraRightVector() const
  {
    if (GetParent())
    {
      return GetParent()->GetRightVector();
    }
    return GetRightVector();
  }

  Math::Vector3f CCameraComponent::GetCameraUpVector() const
  {
    if (GetParent())
    {
      return GetParent()->GetUpVector();
    }
    return GetUpVector();
  }

  Math::Matrix4f CCameraComponent::GetViewMatrix() const
{
   
    
    if (auto* springArm = dynamic_cast<CSpringArmComponent*>(GetParent()))
    {
        Math::Vector3f worldPos = springArm->GetCameraWorldLocation();
        Math::Vector3f targetPos = springArm->GetWorldLocation() + springArm->GetTargetOffset();
        Math::Vector3f up = springArm->GetUpVector();
      
        Math::Matrix4f view = Math::Matrix4f::LookAt(worldPos, targetPos, up);
       
        return view;
    }
    else
    {
        Math::Vector3f worldPos = GetWorldLocation();
        Math::Vector3f forward = GetCameraForwardVector();
        Math::Vector3f up = GetCameraUpVector();

        Math::Matrix4f view = Math::Matrix4f::LookAt(worldPos, worldPos + forward, up);
        
        return view;
    }
}


void CCameraComponent::DebugMatrix(const Math::Matrix4f& m, const char* name) const
{
    CE_CORE_DEBUG(name," Matrix:");
    for (int i = 0; i < 4; i++) {
        CE_CORE_DEBUG("  [",m(i,0), " , ", m(i,1), ",",m(i,2), ",",m(i,3), "]");
    }
    CE_CORE_DEBUG("  Determinant: ", m.Determinant());
}

void CCameraComponent::DebugMatrix(const char* message) const
{
    CE_CORE_DEBUG( message);
}

  Math::Matrix4f CCameraComponent::GetProjectionMatrix() const
{
    Math::Matrix4f projection = Math::Matrix4f::Perspective(
        Math::ToRadians(m_FieldOfView),
        m_AspectRatio,
        m_NearPlane,
        m_FarPlane);

  
    
    return projection;
}

  void CCameraComponent::Update(float DeltaTime)
  {
    CSceneComponent::Update(DeltaTime);
  }
}  // namespace CE