#include "Engine/GamePlay/Components/SpringArmComponent.h"

#include <glm/gtc/matrix_transform.hpp>

namespace CE
{
  SpringArmComponent::SpringArmComponent(CEObject* Owner, FString NewName)
      : SceneComponent(Owner, NewName)
  {
    CE_CORE_DEBUG("SpringArmComponent created: ", NewName);
  }

  glm::vec3 SpringArmComponent::GetCameraWorldLocation() const
  {
    glm::vec3 targetPosition = GetWorldLocation() + m_TargetOffset;
    glm::vec3 cameraPosition = targetPosition - glm::vec3(0.0f, 0.0f, m_ArmLength);

    return cameraPosition;
  }

  void SpringArmComponent::Update(float DeltaTime)
  {
    SceneComponent::Update(DeltaTime);

    // Плавное движение камеры к целевой позиции
    glm::vec3 targetCameraPos = GetCameraWorldLocation();

    if (m_CameraLag > 0.0f)
    {
      float alpha = glm::min(1.0f, DeltaTime / m_CameraLag);
      m_CurrentCameraPosition = glm::mix(m_CurrentCameraPosition, targetCameraPos, alpha);
    }
    else
    {
      m_CurrentCameraPosition = targetCameraPos;
    }
  }
}  // namespace CE