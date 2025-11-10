#pragma once

#include "Actors/Actor.h"
#include "World/CEWorld.h"
#include "glm/glm.hpp"

namespace CE
{
  class SunActor : public CEActor
  {
   public:
    SunActor(CEObject* Owner = nullptr, FString NewName = "Sun");
    virtual ~SunActor() = default;

    virtual void BeginPlay() override;
    virtual void Update(float DeltaTime) override;

    void SetColor(const glm::vec3& color)
    {
      m_Color = color;
    }
    void SetIntensity(float intensity)
    {
      m_Intensity = intensity;
    }
    void SetRadius(float radius)
    {
      m_Radius = radius;
    }
    void SetAngularSpeed(float speed)
    {
      m_AngularSpeed = speed;
    }

   private:
    glm::vec3 m_Color = glm::vec3(1.0f);
    float m_Intensity = 1.0f;
    float m_AngularSpeed = 0.5f;  // radians per second
    float m_Angle = 0.0f;
    float m_Radius = 10.0f;  // orbit radius (large = more directional)
  };
}  // namespace CE
