#include "Engine/GamePlay/Components/CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Engine/GamePlay/Components/SpringArmComponent.h"

namespace CE
{
  CameraComponent::CameraComponent(CEObject* Owner, FString NewName)
      : SceneComponent(Owner, NewName)
  {
  }

  glm::mat4 CameraComponent::GetViewMatrix() const
  {
    // Если есть родительский SpringArm, используем его позицию камеры
    if (auto* springArm = dynamic_cast<SpringArmComponent*>(GetParent()))
    {
      glm::vec3 worldPos = springArm->GetCameraWorldLocation();
      glm::vec3 targetPos = springArm->GetWorldLocation() + springArm->GetTargetOffset();

      glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f);
      return glm::lookAt(worldPos, targetPos, up);
    }
    else
    {
      // Старая логика для камеры без SpringArm
      glm::vec3 worldPos = GetWorldLocation();
      glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
      glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f);

      glm::mat4 rotationMatrix = glm::mat4(1.0f);
      rotationMatrix = glm::rotate(rotationMatrix, glm::radians(GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
      rotationMatrix = glm::rotate(rotationMatrix, glm::radians(GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
      rotationMatrix = glm::rotate(rotationMatrix, glm::radians(GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));

      forward = glm::vec3(rotationMatrix * glm::vec4(forward, 0.0f));
      up = glm::vec3(rotationMatrix * glm::vec4(up, 0.0f));

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