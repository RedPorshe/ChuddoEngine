#include "Engine/GamePlay/Components/SpringArmComponent.h"

#include "Engine/GamePlay/Actors/Pawn.h"
#include "Engine/GamePlay/Components/CameraComponent.h"
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

    // Если используем вращение от Pawn, устанавливаем вращение SpringArm
    if (m_bUsePawnControlRotation)
    {
      CPawn* pawn = dynamic_cast<CPawn*>(GetOwner());
      if (pawn)
      {
        Math::Vector3f controlRot = pawn->GetControlRotation();
        // SpringArm использует pitch и yaw для камеры
        SetRotation(Math::Vector3f(controlRot.x, controlRot.y, 0.0f));
      }
    }

    // Плавное движение камеры к целевой позиции
    Math::Vector3f targetCameraPos = GetCameraWorldLocation();

    // Обновляем позицию камеры относительно SpringArm
    Math::Quaternionf rotationQuat = GetRotationQuat();
    Math::Vector3f cameraOffset = Math::Vector3f(0.0f, 0.0f, m_ArmLength);
    Math::Vector3f rotatedOffset = rotationQuat * cameraOffset;

    CCameraComponent* camera = nullptr;
    ForEachComponent<CCameraComponent>([&camera](CCameraComponent* comp) {
      if (!camera) {
        camera = comp;
      }
    });
    if (camera) {
      camera->SetRelativePosition(rotatedOffset);
    }

    if (m_CameraLag > 0.0f)
    {
      float alpha = std::min(1.0f, DeltaTime / m_CameraLag);
      m_CurrentCameraPosition = Math::Lerp(m_CurrentCameraPosition, targetCameraPos, alpha);
    }
    else
    {
      m_CurrentCameraPosition = targetCameraPos;
    }
  }
}  // namespace CE