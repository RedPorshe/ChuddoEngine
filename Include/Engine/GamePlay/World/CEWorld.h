#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Engine/Core/CEObject.h"
#include "Engine/Core/Rendering/Data/RenderData.h"
#include "Engine/GamePlay/World/Levels/CELevel.h"

namespace CE
{
  class CameraComponent;

  class CEWorld : public CEObject
  {
   public:
    CEWorld(CEObject* Owner = nullptr, FString WorldName = "World");
    virtual ~CEWorld() = default;

    // Allow world to provide a default lighting setup that will be used when
    // a level doesn't populate lighting in CollectRenderData.
    void SetDefaultLighting(const LightingUBO& lighting);
    const LightingUBO& GetDefaultLighting() const
    {
      return m_defaultLighting;
    }

    void CollectRenderData(class FrameRenderData& renderData);
    CameraComponent* FindActiveCamera();

    // Управление уровнями
    void AddLevel(std::unique_ptr<CELevel> Level);
    void RemoveLevel(const FString& LevelName);
    CELevel* FindLevel(const FString& LevelName);

    // Текущий активный уровень
    void SetCurrentLevel(const FString& LevelName);
    void SetCurrentLevel(CELevel* Level);
    CELevel* GetCurrentLevel() const
    {
      return m_CurrentLevel;
    }

    // Переход между уровнями (как в UE)
    void LoadLevel(const FString& LevelName);
    void UnloadCurrentLevel();

    // Получение всех уровней
    const std::vector<std::unique_ptr<CELevel>>& GetLevels() const
    {
      return m_Levels;
    }

    // Управление игровым процессом
    virtual void BeginPlay() override;
    virtual void Update(float DeltaTime) override;
    virtual void Tick(float DeltaTime) override;

   private:
    std::vector<std::unique_ptr<CELevel>> m_Levels;
    CELevel* m_CurrentLevel = nullptr;
    CELevel* m_PendingLevel = nullptr;  // Для плавных переходов
    // Optional world-level lighting that can be applied into FrameRenderData
    // when a level doesn't explicitly set lighting.
    LightingUBO m_defaultLighting;
  };
}  // namespace CE