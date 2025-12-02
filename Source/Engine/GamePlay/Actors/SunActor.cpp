// Consolidated includes
#include "Engine/GamePlay/Actors/SunActor.h"



#include "Engine/Core/Rendering/Data/RenderData.h"
#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/GamePlay/World/World.h"

#include "Engine/Utils/Math/AllMath.h"


  SunActor::SunActor(CObject* Owner, FString NewName)
      : CActor(Owner, NewName)
  {
  }

  void SunActor::BeginPlay()
  {
    CActor::BeginPlay();
    CORE_DEBUG("SunActor BeginPlay: ", GetName());

    // Create a small sphere mesh as a visual marker for the sun
    m_SunMarkerMesh = AddDefaultSubObject<CMeshComponent>("SunMarker");
    if (m_SunMarkerMesh)
    {
      // Load the sphere model from Assets
      m_SunMarkerMesh->SetMesh("Assets/Meshes/Sphere.obj");

      // Scale it down to a small marker size (0.3 units)
      m_SunMarkerMesh->SetRelativeScale(0.3f);

      // Set emissive yellow color to make it stand out
      m_SunMarkerMesh->SetColor(m_Color);
    }
  }

  void SunActor::Update(float DeltaTime)
  {
    CActor::Update(DeltaTime);
    if (m_AngularSpeed != 0.0f)
    {
      m_Angle += m_AngularSpeed * DeltaTime;

      float x = std::cos(m_Angle) * m_Radius;
      float y = std::sin(m_Angle) * (m_Radius * 0.5f) + (m_Radius * 0.2f);
      float z = std::sin(m_Angle * 0.5f) * (m_Radius * 0.3f);

      FVector sunPos = FVector(x, y, z);

      // Update sun marker position
      if (m_SunMarkerMesh)
      {
        this->SetActorLocation(sunPos);
        m_SunMarkerMesh->SetRelativePosition(0.0f);
      }

      CLevel* level = GetLevel();
      if (!level)
        return;

      CObject* levelOwner = level->GetOwner();
      if (!levelOwner)
        return;

      CWorld* world = dynamic_cast<CWorld*>(levelOwner);
      if (!world)
        return;

      LightingUBO lighting = world->GetDefaultLighting();
      lighting.lightCount = 1;
      lighting.lightPositions[0] = FVector4(sunPos, 1.0f);
      lighting.lightColors[0] = FVector4(m_Color, m_Intensity);

      world->SetDefaultLighting(lighting);
    }
    else
    {
      CLevel* level = GetLevel();
      if (!level)
        return;

      CObject* levelOwner = level->GetOwner();
      if (!levelOwner)
        return;

      CWorld* world = dynamic_cast<CWorld*>(levelOwner);
      if (!world)
        return;
      auto sunPos = GetActorLocation();
      LightingUBO lighting = world->GetDefaultLighting();
      lighting.lightCount = 1;
      lighting.lightPositions[0] = FVector4(sunPos, 1.0f);
      lighting.lightColors[0] = FVector4(m_Color, m_Intensity);

      world->SetDefaultLighting(lighting);
    }
  }