#include "GamePlay/Components/Camera/CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>

namespace CE
{
  CameraComponent::CameraComponent(CEObject* Owner, FString NewName)
      : SceneComponent(Owner, NewName)
  {
  }

  glm::mat4 CameraComponent::GetViewMatrix() const
  {
    glm::vec3 worldPos = GetWorldLocation();
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);  // Forward on -Z
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);        // Up on Y

    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));

    forward = glm::vec3(rotationMatrix * glm::vec4(forward, 0.0f));
    up = glm::vec3(rotationMatrix * glm::vec4(up, 0.0f));

    return glm::lookAt(worldPos, worldPos + forward, up);
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