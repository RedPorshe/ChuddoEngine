#pragma once

#include "Engine/GamePlay/Actors/Actor.h"
#include "Engine/GamePlay/World/World.h"
#include "glm/glm.hpp"

namespace CE
{
  class CMeshComponent;

  class SunActor : public CActor
  {
   public:
    SunActor(CObject* Owner = nullptr, FString NewName = "Sun");
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
    float m_AngularSpeed = 0.00f;  // radians per second
    float m_Angle = 0.0f;
    float m_Radius = 10.0f;  // orbit radius (large = more directional)

    // Visual marker mesh
    CMeshComponent* m_SunMarkerMesh = nullptr;
  };
}  // namespace CE
