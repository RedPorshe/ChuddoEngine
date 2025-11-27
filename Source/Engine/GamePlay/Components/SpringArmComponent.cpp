#include "Engine/GamePlay/Components/SpringArmComponent.h"

#include "Engine/Utils/Math/AllMath.h"

namespace CE
{
  CSpringArmComponent::CSpringArmComponent(CObject* Owner, FString NewName)
      : CSceneComponent(Owner, NewName)
  {
    CE_CORE_DEBUG("SpringArmComponent created: ", NewName);
  }

  Math::Vector3f CSpringArmComponent::GetCameraWorldLocation() const
  {
    // Получаем мировую позицию цели (точка, куда смотрит камера)
    Math::Vector3f targetPosition = GetWorldLocation() + m_TargetOffset;

    // Используем кватернион для правильного вращения
Math::Quaternionf rotationQuat = GetRotationQuat();
    

    // Смещение камеры назад от цели (в локальном пространстве SpringArm)
    // Vulkan: Z+ вперед, Y+ вверх, X+ вправо
    Math::Vector3f cameraOffset = Math::Vector3f(0.0f, 0.0f, m_ArmLength);  // Z+ для Vulkan

    // Применяем вращение к смещению камеры
    Math::Vector3f rotatedOffset = rotationQuat * cameraOffset;

    // Позиция камеры = позиция цели + повернутое смещение
    return targetPosition + rotatedOffset;
  }

  void CSpringArmComponent::Update(float DeltaTime)
  {
    CSceneComponent::Update(DeltaTime);

    // Плавное движение камеры к целевой позиции
    Math::Vector3f targetCameraPos = GetCameraWorldLocation();

    if (m_CameraLag > 0.0f)
    {
      //float alpha = glm::min(1.0f, DeltaTime / m_CameraLag);
     // m_CurrentCameraPosition = glm::mix(m_CurrentCameraPosition, targetCameraPos, alpha);
    }
    else
    {
      m_CurrentCameraPosition = targetCameraPos;
    }
  }
}  // namespace CE