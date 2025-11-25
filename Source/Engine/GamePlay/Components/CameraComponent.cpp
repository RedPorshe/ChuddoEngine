#include "Engine/GamePlay/Components/CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Engine/GamePlay/Components/SpringArmComponent.h"

namespace CE
{
  CameraComponent::CameraComponent(CObject* Owner, FString NewName)
      : CSceneComponent(Owner, NewName)
  {
  }

  glm::vec3 CameraComponent::GetCameraForwardVector() const
  {
    if (GetParent())
    {
      auto forward = GetParent()->GetForwardVector();

      return forward;
    }
    auto forward = GetForwardVector();

    return forward;
  }

  glm::vec3 CameraComponent::GetCameraRightVector() const
  {
    if (GetParent())
    {
      return GetParent()->GetRightVector();
    }
    return GetRightVector();
  }

  glm::vec3 CameraComponent::GetCameraUpVector() const
  {
    if (GetParent())
    {
      return GetParent()->GetUpVector();
    }
    return GetUpVector();
  }

  glm::mat4 CameraComponent::GetViewMatrix() const
  {
    if (auto* springArm = dynamic_cast<SpringArmComponent*>(GetParent()))
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
      // worldPos.y *= -1.0f;  // Invert Y for view matrix
      glm::vec3 forward = GetCameraForwardVector();
      // forward.y *= -1.0f;  // Invert Y for view matrix
      glm::vec3 up = GetCameraUpVector();
      // up.y *= -1.0f;  // Invert Y for view matrix

      return glm::lookAt(worldPos, worldPos + forward, up);
    }
  }

  glm::mat4 CameraComponent::GetProjectionMatrix() const
  {
    if (auto* springArm = dynamic_cast<SpringArmComponent*>(GetParent()))
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
      CE_DISPLAY("without spring arm");
      projection = glm::scale(projection, glm::vec3(1.0f, -1.0f, 1.0f));

      return projection;
    }
  }

  void CameraComponent::Update(float DeltaTime)
  {
    CSceneComponent::Update(DeltaTime);
  }
}  // namespace CE