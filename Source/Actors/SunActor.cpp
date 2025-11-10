// Consolidated includes
#include "Actors/SunActor.h"

#include <cmath>

#include "Rendering/Data/RenderData.h"
#include "World/CEWorld.h"
#include "glm/gtc/constants.hpp"

namespace CE
{
  SunActor::SunActor(CEObject* Owner, FString NewName)
      : CEActor(Owner, NewName)
  {
  }

  void SunActor::BeginPlay()
  {
    CEActor::BeginPlay();
    CE_CORE_DEBUG("SunActor BeginPlay: ", GetName());
  }

  void SunActor::Update(float DeltaTime)
  {
    CEActor::Update(DeltaTime);

    // Advance orbit angle
    m_Angle += m_AngularSpeed * DeltaTime;

    // Compute sun position in world space (simple elliptical orbit)
    float x = std::cos(m_Angle) * m_Radius;
    float y = std::sin(m_Angle) * (m_Radius * 0.5f) + (m_Radius * 0.2f);  // tilt/height
    float z = std::sin(m_Angle * 0.5f) * (m_Radius * 0.3f);

    glm::vec3 sunPos = glm::vec3(x, y, z);

    // Locate world via owner chain (actor -> level -> world)
    CELevel* level = GetLevel();
    if (!level)
      return;

    CEObject* levelOwner = level->GetOwner();
    if (!levelOwner)
      return;

    CEWorld* world = dynamic_cast<CEWorld*>(levelOwner);
    if (!world)
      return;

    // Update world's default lighting to move the sun
    LightingUBO lighting = world->GetDefaultLighting();
    lighting.lightCount = 1;
    lighting.lightPositions[0] = glm::vec4(sunPos, 1.0f);
    lighting.lightColors[0] = glm::vec4(m_Color, m_Intensity);

    world->SetDefaultLighting(lighting);
  }
}  // namespace CE
