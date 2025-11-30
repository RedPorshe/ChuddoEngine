#pragma once
#include "Engine/GamePlay/Components/SceneComponent.h"


  class CCameraComponent : public CSceneComponent
  {
   public:
    CCameraComponent(CObject* Owner = nullptr, FString NewName = "CameraComponent");
    virtual ~CCameraComponent() = default;

    // Настройки камеры
    void SetFieldOfView(float FOV)
    {
      m_FieldOfView = FOV;
    }
    void SetAspectRatio(float Aspect)
    {
      m_AspectRatio = Aspect;
    }
    void SetNearPlane(float Near)
    {
      m_NearPlane = Near;
    }
    void SetFarPlane(float Far)
    {
      m_FarPlane = Far;
    }

    void DebugMatrix(const CEMath::Matrix4f& m, const char* name) const;
    void DebugMatrix(const char* message) const;
    // Получение матриц
    CEMath::Matrix4f GetViewMatrix() const;
    CEMath::Matrix4f GetProjectionMatrix() const;

    CEMath::Vector3f GetCameraForwardVector() const;
    CEMath::Vector3f GetCameraRightVector() const;
    CEMath::Vector3f GetCameraUpVector() const;

    virtual void Update(float DeltaTime) override;

   private:
    float m_FieldOfView = 45.0f;
    float m_AspectRatio = 16.0f / 9.0f;
    float m_NearPlane = 0.1f;
    float m_FarPlane = 1000.0f;
  };