#include "Engine/GamePlay/Components/SpringArmComponent.h"

#include <glm/gtc/matrix_transform.hpp>

namespace CE
{
  CSpringArmComponent::CSpringArmComponent(CObject* Owner, FString NewName)
      : CSceneComponent(Owner, NewName)
  {
    CE_CORE_DEBUG("SpringArmComponent created: ", NewName);
  }

  glm::vec3 CSpringArmComponent::GetCameraWorldLocation() const
  {
    // Получаем мировую позицию цели (точка, куда смотрит камера)
    glm::vec3 targetPosition = GetWorldLocation() + m_TargetOffset;

    // Используем кватернион для правильного вращения
    glm::quat rotationQuat = GetRotationQuat();

    // Смещение камеры назад от цели (в локальном пространстве SpringArm)
    // Vulkan: Z+ вперед, Y+ вверх, X+ вправо
    glm::vec3 cameraOffset = glm::vec3(0.0f, 0.0f, m_ArmLength);  // Z+ для Vulkan

    // Применяем вращение к смещению камеры
    glm::vec3 rotatedOffset = rotationQuat * cameraOffset;

    // Позиция камеры = позиция цели + повернутое смещение
    return targetPosition + rotatedOffset;
  }

  void CSpringArmComponent::Update(float DeltaTime)
  {
    CSceneComponent::Update(DeltaTime);

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