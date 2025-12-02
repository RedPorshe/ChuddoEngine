#include "Engine/GamePlay/World/World.h"

#include "Engine/Core/CoreTypes.h"
#include "Engine/Core/Rendering/Data/RenderData.h"
#include "Engine/GamePlay/Actors/Actor.h"
#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/GamePlay/World/Levels/Level.h"
#include "glm/glm.hpp"

CWorld::CWorld(CObject* Owner, FString WorldName)
    : CObject(Owner, WorldName)
{
  CORE_DEBUG("World created: ", WorldName);
}

void CWorld::AddLevel(std::unique_ptr<CLevel> Level)
{
  if (!Level)
    return;

  // Проверяем, нет ли уже уровня с таким именем
  for (const auto& existingLevel : m_Levels)
  {
    if (existingLevel->GetName() == Level->GetName())
    {
      CORE_WARN("Level with name '", Level->GetName(), "' already exists in world");
      return;
    }
  }

  m_Levels.push_back(std::move(Level));

  m_Levels.back()->SetOwner(this);
  CORE_DEBUG("Added level to world: ", m_Levels.back()->GetName());
}

void CWorld::RemoveLevel(const FString& LevelName)
{
  auto it = std::find_if(m_Levels.begin(), m_Levels.end(),
                         [&LevelName](const auto& level)
                         {
                           return level->GetName() == LevelName;
                         });

  if (it != m_Levels.end())
  {
    // Если удаляем текущий уровень, сбрасываем текущий указатель
    if (m_CurrentLevel == it->get())
    {
      m_CurrentLevel = nullptr;
    }

    CORE_DEBUG("Removed level from world: ", LevelName);
    m_Levels.erase(it);
  }
}

CLevel* CWorld::FindLevel(const FString& LevelName)
{
  for (const auto& level : m_Levels)
  {
    if (level->GetName() == LevelName)
    {
      return level.get();
    }
  }
  return nullptr;
}

void CWorld::SetCurrentLevel(const FString& LevelName)
{
  CLevel* level = FindLevel(LevelName);
  if (level)
  {
    SetCurrentLevel(level);
  }
  else
  {
    CORE_ERROR("Cannot set current level: Level '", LevelName, "' not found in world");
  }
}

void CWorld::SetCurrentLevel(CLevel* Level)
{
  if (Level && std::find_if(m_Levels.begin(), m_Levels.end(),
                            [Level](const auto& ptr)
                            { return ptr.get() == Level; }) != m_Levels.end())
  {
    if (m_CurrentLevel)
    {
      // Здесь можно добавить логику очистки старого уровня
      CORE_DEBUG("Unloading previous level: ", m_CurrentLevel->GetName());
    }

    m_CurrentLevel = Level;
    CORE_DEBUG("Current level set to: ", m_CurrentLevel->GetName());

    // Вызываем BeginPlay для нового уровня
    m_CurrentLevel->BeginPlay();
  }
  else
  {
    CORE_ERROR("Cannot set current level: Level not found in world");
  }
}

void CWorld::LoadLevel(const FString& LevelName)
{
  CLevel* level = FindLevel(LevelName);
  if (level)
  {
    CORE_DEBUG("Loading level: ", LevelName);
    SetCurrentLevel(level);
  }
  else
  {
    CORE_ERROR("Cannot load level: '", LevelName, "' not found");
  }
}

void CWorld::UnloadCurrentLevel()
{
  if (m_CurrentLevel)
  {
    CORE_DEBUG("Unloading current level: ", m_CurrentLevel->GetName());
    m_CurrentLevel = nullptr;
  }
}

void CWorld::BeginPlay()
{
  CObject::BeginPlay();

  if (!m_Levels.empty() && !m_CurrentLevel)
  {
    SetCurrentLevel(m_Levels[0].get());
  }
}

void CWorld::Update(float DeltaTime)
{
  CObject::Update(DeltaTime);

  if (m_CurrentLevel)
  {
    m_CurrentLevel->Update(DeltaTime);
  }
}

void CWorld::Tick(float DeltaTime)
{
  Update(DeltaTime);
  m_CurrentLevel->Tick(DeltaTime);
}

CCameraComponent* CWorld::FindActiveCamera()
{
  if (!m_CurrentLevel)
    return nullptr;

  for (const auto& actor : m_CurrentLevel->GetActors())
  {
    auto cameraComponents = actor->GetComponents<CCameraComponent>();
    if (!cameraComponents.empty())
    {
      return cameraComponents[0];
    }
  }

  return nullptr;
}

void CWorld::CollectRenderData(FrameRenderData& renderData)
{
  if (!m_CurrentLevel)
    return;

  if (auto* camera = FindActiveCamera())
  {
    CameraData camData;
    camData.viewMatrix = camera->GetViewMatrix();
    camData.projectionMatrix = camera->GetProjectionMatrix();
    camData.position = camera->GetWorldLocation();

    renderData.SetCameraData(camData);
  }
  else
  {
    CameraData defaultCam;
    FVector defaultCamPosition = FVector(0.0f, 2.0f, 10.0f);
    FVector defaultCameraUp = FVector(0.0f, 1.0f, 0.0f);
    FVector defaultCameraTarget = FVector(0.0f, 0.0f, 0.0f);

    defaultCam.viewMatrix = FMatrix::LookAt(
        defaultCamPosition,
        defaultCameraTarget,
        defaultCameraUp);
    defaultCam.projectionMatrix = FMatrix::Perspective (
        CEMath::DEG_TO_RAD *(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
    defaultCam.position = defaultCamPosition;

    renderData.SetCameraData(defaultCam);
  }

  for (const auto& actor : m_CurrentLevel->GetActors())
  {
    auto meshComponents = actor->GetComponents<CMeshComponent>();

    for (auto* meshComp : meshComponents)
    {
      RenderObject renderObj;
      renderObj.mesh = &meshComp->GetMeshData();
      renderObj.transform = meshComp->GetRenderTransform();
      renderObj.color = meshComp->GetColor();

      renderData.AddRenderObject(renderObj);
    }
  }

  auto& lighting = renderData.lighting;

  if (lighting.lightCount == 0 && m_defaultLighting.lightCount > 0)
  {
    lighting = m_defaultLighting;
  }

  if (lighting.lightCount == 0)
  {

    lighting.lightCount = 1;
    lighting.lightPositions[0] = FVector4(5.0f, 5.0f, 5.0f, 1.0f);
    lighting.lightColors[0] = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
    lighting.lightColors[0].w = 2.0f;
    lighting.ambientColor = FVector4(0.2f, 0.2f, 0.2f, .2f);
  }
}

void CWorld::SetDefaultLighting(const LightingUBO& lighting)
{
  m_defaultLighting = lighting;
}
