#include "Engine/GamePlay/Components/CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Engine/GamePlay/Components/SpringArmComponent.h"

namespace CE
{
  CCameraComponent::CCameraComponent(CObject* Owner, FString NewName)
      : CSceneComponent(Owner, NewName)
  {
  }

  glm::vec3 CCameraComponent::GetCameraForwardVector() const
  {
    if (GetParent())
    {
      auto forward = GetParent()->GetForwardVector();

      return forward;
    }
    auto forward = GetForwardVector();

    return forward;
  }

  glm::vec3 CCameraComponent::GetCameraRightVector() const
  {
    if (GetParent())
    {
      return GetParent()->GetRightVector();
    }
    return GetRightVector();
  }

  glm::vec3 CCameraComponent::GetCameraUpVector() const
  {
    if (GetParent())
    {
      return GetParent()->GetUpVector();
    }
    return GetUpVector();
  }

  glm::mat4 CCameraComponent::GetViewMatrix() const
  {
    if (auto* springArm = dynamic_cast<CSpringArmComponent*>(GetParent()))
    {
      glm::vec3 worldPos = springArm->GetCameraWorldLocation();
      glm::vec3 targetPos = springArm->GetWorldLocation() + springArm->GetTargetOffset();
      glm::vec3 up = springArm->GetUpVector();
      up.y *= -1.0f;  // Invert Y for view matrix

      return glm::lookAt(worldPos, targetPos, up);
    }
    else
    {
      glm::vec3 worldPos = GetWorldLocation();
     
      glm::vec3 forward = GetCameraForwardVector();
     
      glm::vec3 up = GetCameraUpVector();
     

      return glm::lookAt(worldPos, worldPos + forward, up);
    }
  }

  glm::mat4 CCameraComponent::GetProjectionMatrix() const
  {
    if (auto* springArm = dynamic_cast<CSpringArmComponent*>(GetParent()))
    {
      glm::mat4 projection = glm::perspective(
          glm::radians(m_FieldOfView),
          m_AspectRatio,
          m_NearPlane,
          m_FarPlane);
      (void)springArm;
      projection = glm::scale(projection, glm::vec3(1.0f, 1.0f, 1.0f));

      return projection;
    }
    else
    {
      glm::mat4 projection = glm::perspective(
          glm::radians(m_FieldOfView),
          m_AspectRatio,
          m_NearPlane,
          m_FarPlane);
      
      projection = glm::scale(projection, glm::vec3(1.0f, -1.0f, 1.0f));

      return projection;
    }
  }

  void CCameraComponent::Update(float DeltaTime)
  {
    CSceneComponent::Update(DeltaTime);
  }
}  // namespace CE