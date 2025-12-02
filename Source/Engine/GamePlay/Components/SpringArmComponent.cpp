#include "Engine/GamePlay/Components/SpringArmComponent.h"

#include "Engine/Core/CoreTypes.h"
#include "Engine/GamePlay/Actors/Pawn.h"
#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/Utils/Math/AllMath.h"


  CSpringArmComponent::CSpringArmComponent(CObject* Owner, FString NewName)
      : CSceneComponent(Owner, NewName)
  {
    CORE_DEBUG("SpringArmComponent created: ", NewName);
  }

  FVector CSpringArmComponent::GetCameraWorldLocation() const
  {

    FVector targetPosition = GetWorldLocation() + m_TargetOffset;


FQuat rotationQuat = GetRotationQuat();


    

    

    FVector cameraOffset = FVector(0.0f, 0.0f, m_ArmLength);  // Z+ для Vulkan


    FVector rotatedOffset = rotationQuat * cameraOffset;


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
        FVector controlRot = pawn->GetControlRotation();

        SetRotation(FVector(controlRot.x, controlRot.y, 0.0f));
      }
    }

    

    FVector targetCameraPos = GetCameraWorldLocation();


    FQuat rotationQuat = GetRotationQuat();
    FVector cameraOffset = FVector(0.0f, 0.0f, m_ArmLength);
    FVector rotatedOffset = rotationQuat * cameraOffset;

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
      m_CurrentCameraPosition = m_CurrentCameraPosition + (targetCameraPos - m_CurrentCameraPosition) * alpha;
    }
    else
    {
      m_CurrentCameraPosition = targetCameraPos;
    }
  }