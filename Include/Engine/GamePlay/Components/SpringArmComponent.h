#pragma once

#include "Engine/GamePlay/Components/SceneComponent.h"


  class CSpringArmComponent : public CSceneComponent
  {
   public:
    CSpringArmComponent(CObject* Owner = nullptr, FString NewName = "SpringArmComponent");
    virtual ~CSpringArmComponent() = default;

    // Настройки spring arm
    void SetTargetOffset(const FVector& Offset)
    {
      m_TargetOffset = Offset;
    }
    void SetArmLength(float Length)
    {
      m_ArmLength = Length;
    }
    void SetCameraLag(float Lag)
    {
      m_CameraLag = Lag;
    }
    void SetUsePawnControlRotation(bool bUse)
    {
      m_bUsePawnControlRotation = bUse;
    }

    const FVector& GetTargetOffset() const
    {
      return m_TargetOffset;
    }
    float GetArmLength() const
    {
      return m_ArmLength;
    }
    float GetCameraLag() const
    {
      return m_CameraLag;
    }
    bool GetUsePawnControlRotation() const
    {
      return m_bUsePawnControlRotation;
    }

    // Получение конечной позиции камеры
    FVector GetCameraWorldLocation() const;

    virtual void Update(float DeltaTime) override;

   private:
    FVector m_TargetOffset{0.0f, -0.9f, 1.0f};  // Смещение от цели (Vulkan: Y+ вверх)
    float m_ArmLength = 5.0f;                    // Длина "руки" камеры
    float m_CameraLag = 0.05f;                   // Задержка движения камеры
    bool m_bUsePawnControlRotation = true;       // Использовать вращение от Pawn

    FVector m_CurrentCameraPosition{0.0f, 0.0f, 0.0f};
  };