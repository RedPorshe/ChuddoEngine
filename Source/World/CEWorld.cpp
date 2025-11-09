#include "World/CEWorld.h"

#include "Actors/Actor.h"
#include "Components/Camera/CameraComponent.h"
#include "Components/Meshes/MeshComponent.h"
#include "Rendering/Data/RenderData.h"
#include "World/Levels/CELevel.h"
#include "glm/glm.hpp"

namespace CE
{
  CEWorld::CEWorld(CEObject* Owner, FString WorldName)
      : CEObject(Owner, WorldName)
  {
    CE_CORE_DEBUG("World created: ", WorldName);
  }

  void CEWorld::AddLevel(std::unique_ptr<CELevel> Level)
  {
    if (!Level)
      return;

    // Проверяем, нет ли уже уровня с таким именем
    for (const auto& existingLevel : m_Levels)
    {
      if (existingLevel->GetName() == Level->GetName())
      {
        CE_CORE_WARN("Level with name '", Level->GetName(), "' already exists in world");
        return;
      }
    }

    m_Levels.push_back(std::move(Level));
    CE_CORE_DEBUG("Added level to world: ", m_Levels.back()->GetName());
  }

  void CEWorld::RemoveLevel(const FString& LevelName)
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

      CE_CORE_DEBUG("Removed level from world: ", LevelName);
      m_Levels.erase(it);
    }
  }

  CELevel* CEWorld::FindLevel(const FString& LevelName)
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

  void CEWorld::SetCurrentLevel(const FString& LevelName)
  {
    CELevel* level = FindLevel(LevelName);
    if (level)
    {
      SetCurrentLevel(level);
    }
    else
    {
      CE_CORE_ERROR("Cannot set current level: Level '", LevelName, "' not found in world");
    }
  }

  void CEWorld::SetCurrentLevel(CELevel* Level)
  {
    if (Level && std::find_if(m_Levels.begin(), m_Levels.end(),
                              [Level](const auto& ptr)
                              { return ptr.get() == Level; }) != m_Levels.end())
    {
      // Вызываем EndPlay для старого уровня (если был)
      if (m_CurrentLevel)
      {
        // Здесь можно добавить логику очистки старого уровня
        CE_CORE_DEBUG("Unloading previous level: ", m_CurrentLevel->GetName());
      }

      m_CurrentLevel = Level;
      CE_CORE_DEBUG("Current level set to: ", m_CurrentLevel->GetName());

      // Вызываем BeginPlay для нового уровня
      m_CurrentLevel->BeginPlay();
    }
    else
    {
      CE_CORE_ERROR("Cannot set current level: Level not found in world");
    }
  }

  void CEWorld::LoadLevel(const FString& LevelName)
  {
    CELevel* level = FindLevel(LevelName);
    if (level)
    {
      // Асинхронная загрузка уровня (базовая реализация)
      CE_CORE_DEBUG("Loading level: ", LevelName);
      SetCurrentLevel(level);
    }
    else
    {
      CE_CORE_ERROR("Cannot load level: '", LevelName, "' not found");
    }
  }

  void CEWorld::UnloadCurrentLevel()
  {
    if (m_CurrentLevel)
    {
      CE_CORE_DEBUG("Unloading current level: ", m_CurrentLevel->GetName());
      m_CurrentLevel = nullptr;
    }
  }

  void CEWorld::BeginPlay()
  {
    CEObject::BeginPlay();
    CE_CORE_DEBUG("World BeginPlay: ", GetName());

    // Если есть уровни, устанавливаем первый как текущий
    if (!m_Levels.empty() && !m_CurrentLevel)
    {
      SetCurrentLevel(m_Levels[0].get());
    }
  }

  void CEWorld::Update(float DeltaTime)
  {
    CEObject::Update(DeltaTime);

    // Обновляем только текущий уровень
    if (m_CurrentLevel)
    {
      m_CurrentLevel->Update(DeltaTime);
    }
  }

  void CEWorld::Tick(float DeltaTime)
  {
    Update(DeltaTime);
  }

  CameraComponent* CEWorld::FindActiveCamera()
  {
    if (!m_CurrentLevel)
      return nullptr;

    // Ищем первую камеру в уровне используя GetComponents<T>()
    for (const auto& actor : m_CurrentLevel->GetActors())
    {
      auto cameraComponents = actor->GetComponents<CameraComponent>();
      if (!cameraComponents.empty())
      {
        return cameraComponents[0];
      }
    }

    return nullptr;
  }

  void CEWorld::CollectRenderData(FrameRenderData& renderData)
  {
    if (!m_CurrentLevel)
      return;

    // Собираем данные камеры
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
      // Камера по умолчанию, если не найдена
      CameraData defaultCam;
      defaultCam.viewMatrix = glm::lookAt(
          glm::vec3(0.0f, 0.0f, 10.0f),  // Позиция камеры
          glm::vec3(0.0f, 0.0f, 0.0f),   // Смотрит в центр
          glm::vec3(0.0f, 1.0f, 0.0f)    // Вверх
      );
      defaultCam.projectionMatrix = glm::perspective(
          glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
      defaultCam.position = glm::vec3(0.0f, 0.0f, 10.0f);

      renderData.SetCameraData(defaultCam);
      CE_CORE_DEBUG("Using default camera");
    }

    // Собираем рендер-объекты используя GetComponents<T>()
    for (const auto& actor : m_CurrentLevel->GetActors())
    {
      auto meshComponents = actor->GetComponents<MeshComponent>();

      for (auto* meshComp : meshComponents)
      {
        RenderObject renderObj;
        renderObj.mesh = &meshComp->GetMeshData();
        renderObj.transform = meshComp->GetRenderTransform();
        renderObj.color = meshComp->GetColor();

        renderData.AddRenderObject(renderObj);
      }
    }

    renderData.lighting.ambientColor = glm::vec4(1.f, 1.f, 1.f, 1.0f);
    renderData.lighting.ambientIntensity = 1.f;

    // Добавляем источник света
    renderData.lighting.lightCount = 1;
    renderData.lighting.lightPositions[0] = glm::vec4(-15.0f, -5.0f, -5.0f, 1.0f);  // Позиция света
    renderData.lighting.lightColors[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);         // Белый свет
    renderData.lighting.lightIntensities[0] = 5.0f;                                 // Интенсивность
  }

}  // namespace CE