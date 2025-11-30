#include "Engine/GamePlay/GameInstance/GameInstance.h"


  CGameInstance::CGameInstance()
      : CObject(nullptr, "GameInstance")
  {
    CORE_DEBUG("GameInstance created");
  }

  void CGameInstance::Initialize()
  {
    CORE_DISPLAY("=== Initializing GameInstance ===");
    SetupWorlds();  // Настраиваем миры при инициализации
  }

  void CGameInstance::Shutdown()
  {
    CORE_DISPLAY("=== Shutting Down GameInstance ===");
    UnloadCurrentWorld();
    m_Worlds.clear();
  }

  CWorld* CGameInstance::CreateWorld(const FString& WorldName)
  {
    auto world = std::make_unique<CWorld>(nullptr, WorldName);
    auto* worldPtr = world.get();
    m_Worlds[WorldName] = std::move(world);
    CORE_DEBUG("Created world: ", WorldName);
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
      CORE_DEBUG("Loaded world: ", WorldName);
    }
    else
    {
      CORE_ERROR("World not found: ", WorldName);
    }
  }

  void CGameInstance::UnloadCurrentWorld()
  {
    if (m_CurrentWorld)
    {
      m_CurrentWorld->UnloadCurrentLevel();
      m_CurrentWorld = nullptr;
      CORE_DEBUG("Unloaded current world");
    }
  }

  CWorld* CGameInstance::GetWorld(const FString& WorldName) const
  {
    auto it = m_Worlds.find(WorldName);
    return it != m_Worlds.end() ? it->second.get() : nullptr;
  }

  void CGameInstance::SetupWorlds()
  {
    
    CreateWorld("DefaultWorld");
  }

  void CGameInstance::BeginPlay()
  {
    CObject::BeginPlay();
    
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