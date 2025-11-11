#pragma once
#include "GamePlay/Components/Scene/SceneComponent.h"

namespace CE
{
  class CameraComponent : public SceneComponent
  {
   public:
    CameraComponent(CEObject* Owner = nullptr, FString NewName = "CameraComponent");
    virtual ~CameraComponent() = default;

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

    // Получение матриц
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    virtual void Update(float DeltaTime) override;

   private:
    float m_FieldOfView = 45.0f;
    float m_AspectRatio = 16.0f / 9.0f;
    float m_NearPlane = 0.1f;
    float m_FarPlane = 1000.0f;
  };
}  // namespace CE