#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Engine/Core/Object.h"
#include "Engine/Core/Rendering/Data/RenderData.h"
#include "Engine/GamePlay/World/Levels/Level.h"

class CCameraComponent;

class CWorld : public CObject
{
 public:
  CWorld(CObject* Owner = nullptr, FString WorldName = "World");
  virtual ~CWorld() = default;

  // Allow world to provide a default lighting setup that will be used when
  // a level doesn't populate lighting in CollectRenderData.
  void SetDefaultLighting(const LightingUBO& lighting);
  const LightingUBO& GetDefaultLighting() const
  {
    return m_defaultLighting;
  }

  void CollectRenderData(class FrameRenderData& renderData);
  CCameraComponent* FindActiveCamera();

  // Управление уровнями
  void AddLevel(std::unique_ptr<CLevel> Level);
  void RemoveLevel(const FString& LevelName);
  CLevel* FindLevel(const FString& LevelName);

  // Текущий активный уровень
  void SetCurrentLevel(const FString& LevelName);
  void SetCurrentLevel(CLevel* Level);
  CLevel* GetCurrentLevel() const
  {
    return m_CurrentLevel;
  }

  // Переход между уровнями (как в UE)
  void LoadLevel(const FString& LevelName);
  void UnloadCurrentLevel();

  // Получение всех уровней
  const std::vector<std::unique_ptr<CLevel>>& GetLevels() const
  {
    return m_Levels;
  }

  // Управление игровым процессом
  virtual void BeginPlay() override;
  virtual void Update(float DeltaTime) override;
  virtual void Tick(float DeltaTime) override;

 private:
  std::vector<std::unique_ptr<CLevel>> m_Levels;
  CLevel* m_CurrentLevel = nullptr;
  CLevel* m_PendingLevel = nullptr;  
  LightingUBO m_defaultLighting;
};
