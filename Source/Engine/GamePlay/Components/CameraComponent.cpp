#include "Engine/GamePlay/Components/CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Engine/GamePlay/Components/SpringArmComponent.h"

namespace CE
{
  CameraComponent::CameraComponent(CEObject* Owner, FString NewName)
      : SceneComponent(Owner, NewName)
  {
  }

  glm::vec3 CameraComponent::GetCameraForwardVector() const
  {
    return -GetForwardVector();
  }

  glm::vec3 CameraComponent::GetCameraRightVector() const
  {
    return -GetRightVector();
  }

  glm::vec3 CameraComponent::GetCameraUpVector() const
  {
    return GetUpVector();
  }

  glm::mat4 CameraComponent::GetViewMatrix() const
  {
    if (auto* springArm = dynamic_cast<SpringArmComponent*>(GetParent()))
    {
      glm::vec3 worldPos = springArm->GetCameraWorldLocation();
      glm::vec3 targetPos = springArm->GetWorldLocation() + springArm->GetTargetOffset();

      glm::vec3 up = springArm->GetUpVector();
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

  glm::mat4 CameraComponent::GetProjectionMatrix() const
  {
    return glm::perspective(
        glm::radians(m_FieldOfView),
        m_AspectRatio,
        m_NearPlane,
        m_FarPlane);
  }

  void CameraComponent::Update(float DeltaTime)
  {
    SceneComponent::Update(DeltaTime);
  }
}  // namespace CE