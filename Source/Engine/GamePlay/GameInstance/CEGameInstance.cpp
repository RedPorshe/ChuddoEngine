#include "Engine/GamePlay/GameInstance/CEGameInstance.h"

namespace CE
{
  CEGameInstance::CEGameInstance()
      : CEObject(nullptr, "GameInstance")
  {
    CE_CORE_DEBUG("GameInstance created");
  }

  void CEGameInstance::Initialize()
  {
    CE_CORE_DISPLAY("=== Initializing GameInstance ===");
    SetupWorlds();  // Настраиваем миры при инициализации
  }

  void CEGameInstance::Shutdown()
  {
    CE_CORE_DISPLAY("=== Shutting Down GameInstance ===");
    UnloadCurrentWorld();
    m_Worlds.clear();
  }

  CEWorld* CEGameInstance::CreateWorld(const FString& WorldName)
  {
    auto world = std::make_unique<CEWorld>(nullptr, WorldName);
    auto* worldPtr = world.get();
    m_Worlds[WorldName] = std::move(world);
    CE_CORE_DEBUG("Created world: ", WorldName);
    return worldPtr;
  }

  void CEGameInstance::LoadWorld(const FString& WorldName)
  {
    auto it = m_Worlds.find(WorldName);
    if (it != m_Worlds.end() && it->second)
    {
      UnloadCurrentWorld();
      m_CurrentWorld = it->second.get();
      m_CurrentWorld->BeginPlay();
      CE_CORE_DEBUG("Loaded world: ", WorldName);
    }
    else
    {
      CE_CORE_ERROR("World not found: ", WorldName);
    }
  }

  void CEGameInstance::UnloadCurrentWorld()
  {
    if (m_CurrentWorld)
    {
      m_CurrentWorld->UnloadCurrentLevel();
      m_CurrentWorld = nullptr;
      CE_CORE_DEBUG("Unloaded current world");
    }
  }

  CEWorld* CEGameInstance::GetWorld(const FString& WorldName) const
  {
    auto it = m_Worlds.find(WorldName);
    return it != m_Worlds.end() ? it->second.get() : nullptr;
  }

  void CEGameInstance::SetupWorlds()
  {
    // Базовая реализация - создаем пустой мир
    // Производные классы переопределят этот метод
    CreateWorld("DefaultWorld");
  }

  void CEGameInstance::BeginPlay()
  {
    CEObject::BeginPlay();

    // Автоматически загружаем первый созданный мир
    if (!m_Worlds.empty())
    {
      LoadWorld(m_Worlds.begin()->first);
    }
  }

  void CEGameInstance::Update(float DeltaTime)
  {
    CEObject::Update(DeltaTime);

    if (m_CurrentWorld)
    {
      m_CurrentWorld->Update(DeltaTime);
    }
  }

  void CEGameInstance::Tick(float DeltaTime)
  {
    Update(DeltaTime);
    m_CurrentWorld->Tick(DeltaTime);
  }
}  // namespace CE