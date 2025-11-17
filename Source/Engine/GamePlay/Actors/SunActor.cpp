// Consolidated includes
#include "Engine/GamePlay/Actors/SunActor.h"

#include <cmath>

#include "Engine/Core/Rendering/Data/RenderData.h"
#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/GamePlay/World/CEWorld.h"
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

    // Create a small sphere mesh as a visual marker for the sun
    m_SunMarkerMesh = AddDefaultSubObject<MeshComponent>("SunMarker");
    if (m_SunMarkerMesh)
    {
      // Load the sphere model from Assets
      m_SunMarkerMesh->SetMesh("Assets/Meshes/Sphere.obj");

      // Scale it down to a small marker size (0.3 units)
      m_SunMarkerMesh->SetRelativeScale(glm::vec3(0.3f));

      // Set emissive yellow color to make it stand out
      m_SunMarkerMesh->SetColor(m_Color);
    }
  }

  void SunActor::Update(float DeltaTime)
  {
    CEActor::Update(DeltaTime);
    if (m_AngularSpeed != 0.0f)
    {
      m_Angle += m_AngularSpeed * DeltaTime;

      float x = std::cos(m_Angle) * m_Radius;
      float y = std::sin(m_Angle) * (m_Radius * 0.5f) + (m_Radius * 0.2f);
      float z = std::sin(m_Angle * 0.5f) * (m_Radius * 0.3f);

      glm::vec3 sunPos = glm::vec3(x, y, z);

      // Update sun marker position
      if (m_SunMarkerMesh)
      {
        this->SetActorLocation(sunPos);
        m_SunMarkerMesh->SetRelativePosition(glm::vec3(0.0f));
      }

      CELevel* level = GetLevel();
      if (!level)
        return;

      CEObject* levelOwner = level->GetOwner();
      if (!levelOwner)
        return;

      CEWorld* world = dynamic_cast<CEWorld*>(levelOwner);
      if (!world)
        return;

      LightingUBO lighting = world->GetDefaultLighting();
      lighting.lightCount = 1;
      lighting.lightPositions[0] = glm::vec4(sunPos, 1.0f);
      lighting.lightColors[0] = glm::vec4(m_Color, m_Intensity);

      world->SetDefaultLighting(lighting);
    }
    else
    {
      CELevel* level = GetLevel();
      if (!level)
        return;

      CEObject* levelOwner = level->GetOwner();
      if (!levelOwner)
        return;

      CEWorld* world = dynamic_cast<CEWorld*>(levelOwner);
      if (!world)
        return;
      auto sunPos = GetActorLocation();
      LightingUBO lighting = world->GetDefaultLighting();
      lighting.lightCount = 1;
      lighting.lightPositions[0] = glm::vec4(sunPos, 1.0f);
      lighting.lightColors[0] = glm::vec4(m_Color, m_Intensity);

      world->SetDefaultLighting(lighting);
    }
  }
}  // namespace CE
