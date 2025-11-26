#include "Engine/GamePlay/GameInstance/GameInstance.h"

namespace CE
{
  CGameInstance::CGameInstance()
      : CObject(nullptr, "GameInstance")
  {
    CE_CORE_DEBUG("GameInstance created");
  }

  void CGameInstance::Initialize()
  {
    CE_CORE_DISPLAY("=== Initializing GameInstance ===");
    SetupWorlds();  // Настраиваем миры при инициализации
  }

  void CGameInstance::Shutdown()
  {
    CE_CORE_DISPLAY("=== Shutting Down GameInstance ===");
    UnloadCurrentWorld();
    m_Worlds.clear();
  }

  CWorld* CGameInstance::CreateWorld(const FString& WorldName)
  {
    auto world = std::make_unique<CWorld>(nullptr, WorldName);
    auto* worldPtr = world.get();
    m_Worlds[WorldName] = std::move(world);
    CE_CORE_DEBUG("Created world: ", WorldName);
    return worldPtr;
  }

  void CGameInstance::LoadWorld(const FString& WorldName)
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

  void CGameInstance::UnloadCurrentWorld()
  {
    if (m_CurrentWorld)
    {
      m_CurrentWorld->UnloadCurrentLevel();
      m_CurrentWorld = nullptr;
      CE_CORE_DEBUG("Unloaded current world");
    }
  }

  CWorld* CGameInstance::GetWorld(const FString& WorldName) const
  {
    auto it = m_Worlds.find(WorldName);
    return it != m_Worlds.end() ? it->second.get() : nullptr;
  }

  void CGameInstance::SetupWorlds()
  {
    // Базовая реализация - создаем пустой мир
    // Производные классы переопределят этот метод
    CreateWorld("DefaultWorld");
  }

  void CGameInstance::BeginPlay()
  {
    CObject::BeginPlay();

    // Автоматически загружаем первый созданный мир
    if (!m_Worlds.empty())
    {
      LoadWorld(m_Worlds.begin()->first);
    }
  }

  void CGameInstance::Update(float DeltaTime)
  {
    CObject::Update(DeltaTime);

    if (m_CurrentWorld)
    {
      m_CurrentWorld->Update(DeltaTime);
    }
  }

  void CGameInstance::Tick(float DeltaTime)
  {
    Update(DeltaTime);
    m_CurrentWorld->Tick(DeltaTime);
  }
}  // namespace CE