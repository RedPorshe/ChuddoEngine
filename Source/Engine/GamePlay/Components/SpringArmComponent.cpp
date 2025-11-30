#include "Engine/GamePlay/Components/SpringArmComponent.h"

#include "Engine/GamePlay/Actors/Pawn.h"
#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/Utils/Math/AllMath.h"


  CSpringArmComponent::CSpringArmComponent(CObject* Owner, FString NewName)
      : CSceneComponent(Owner, NewName)
  {
    CORE_DEBUG("SpringArmComponent created: ", NewName);
  }

  CEMath::Vector3f CSpringArmComponent::GetCameraWorldLocation() const
  {
    
    CEMath::Vector3f targetPosition = GetWorldLocation() + m_TargetOffset;

    
CEMath::Quaternionf rotationQuat = GetRotationQuat();
    

    
    CEMath::Vector3f cameraOffset = CEMath::Vector3f(0.0f, 0.0f, m_ArmLength);  // Z+ для Vulkan

  
    CEMath::Vector3f rotatedOffset = rotationQuat * cameraOffset;

    
    return targetPosition + rotatedOffset;
  }

  void CSpringArmComponent::Update(float DeltaTime)
  {
    CSceneComponent::Update(DeltaTime);

    
    if (m_bUsePawnControlRotation)
    {
      CPawn* pawn = dynamic_cast<CPawn*>(GetOwner());
      if (pawn)
      {
        CEMath::Vector3f controlRot = pawn->GetControlRotation();
        
        SetRotation(CEMath::Vector3f(controlRot.x, controlRot.y, 0.0f));
      }
    }

    
    CEMath::Vector3f targetCameraPos = GetCameraWorldLocation();

    
    CEMath::Quaternionf rotationQuat = GetRotationQuat();
    CEMath::Vector3f cameraOffset = CEMath::Vector3f(0.0f, 0.0f, m_ArmLength);
    CEMath::Vector3f rotatedOffset = rotationQuat * cameraOffset;

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
      m_CurrentCameraPosition = CEMath::Lerp(m_CurrentCameraPosition, targetCameraPos, alpha);
    }
    else
    {
      m_CurrentCameraPosition = targetCameraPos;
    }
  }