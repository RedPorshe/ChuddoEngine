#include "Engine/Editor/EditorApplication.h"

#include "Engine/Editor/UI/PropertyEditor.h"


  EditorApplication::EditorApplication(AppInfo* info) : Application(info)
  {
    CORE_DEBUG("EditorApplication created");
  }

  EditorApplication::~EditorApplication()
  {
    Shutdown();
  }

  void EditorApplication::Initialize()
  {
    // Call parent initialization first
    Application::Initialize();

    // TODO: Initialize Slate UI framework

    CORE_DISPLAY("=== EditorApplication Initialized ===");
  }

  void EditorApplication::Run()
  {
    CORE_DEBUG("Starting editor main loop");

    // Initialize game instance for editor
    if (m_GameInstance)
    {
      m_GameInstance->BeginPlay();
      if (auto* world = m_GameInstance->GetCurrentWorld())
      {
        m_RenderData.lighting = world->GetDefaultLighting();
        CORE_DEBUG("RenderData lighting initialized from world default (lights=", m_RenderData.lighting.lightCount, ")");
      }
    }

    m_IsRunning = true;

    while (m_IsRunning)
    {
      CalculateDeltaTime();

      // Handle editor input
      ProcessEditorInput();

      // Update game logic if in play mode
      if (m_EditorMode == EditorMode::Play)
      {
        if (m_GameInstance)
        {
          m_GameInstance->Tick(m_DeltaTime);
        }
      }

      // Render
      if (m_EditorMode == EditorMode::Play)
      {
        RenderPlayMode();
      }
      else
      {
        RenderEditMode();
      }

      // Check for quit
      if (m_RenderSystem->ShouldClose())
      {
        m_IsRunning = false;
      }

      m_RenderSystem->PollEvents();
    }
  }

  void EditorApplication::Shutdown()
  {
    m_PropertyEditor.Shutdown();
    // TODO: Shutdown Slate UI framework
    Application::Shutdown();
    CORE_DISPLAY("=== EditorApplication Shutdown Complete ===");
  }

  void EditorApplication::ToggleEditorMode()
  {
    m_EditorMode = (m_EditorMode == EditorMode::Play) ? EditorMode::Edit : EditorMode::Play;
    CORE_DEBUG("Switched to ", (m_EditorMode == EditorMode::Play ? "Play" : "Edit"), " mode");
  }



  void EditorApplication::RenderEditorUI()
  {
    // TODO: Implement Slate-based rendering
    // RenderMainMenuBar();
    // RenderViewport();
    // RenderOutliner();
    // RenderPropertyPanel();
  }

  void EditorApplication::ProcessEditorInput()
  {
    // Handle editor-specific input (F5 for play/pause, etc.)
    const bool* state = SDL_GetKeyboardState(NULL);
    if (state && state[SDL_SCANCODE_F5])
    {
      ToggleEditorMode();
    }
  }

  void EditorApplication::RenderPlayMode()
  {
    // Clear render data
    m_RenderData.Clear();

    // Collect render data from world
    if (m_GameInstance && m_GameInstance->GetCurrentWorld())
    {
      m_GameInstance->GetCurrentWorld()->CollectRenderData(m_RenderData);
    }

    // Render the frame
    if (m_RenderSystem)
    {
      m_RenderSystem->DrawFrame(m_RenderData);
    }
  }

  void EditorApplication::RenderEditMode()
  {
    // TODO: Start Slate frame

    // Render editor UI
    RenderEditorUI();

    // Clear render data
    m_RenderData.Clear();

    // Collect render data from world
    if (m_GameInstance && m_GameInstance->GetCurrentWorld())
    {
      m_GameInstance->GetCurrentWorld()->CollectRenderData(m_RenderData);
    }

    // Render the frame (this will include Slate UI)
    if (m_RenderSystem)
    {
      m_RenderSystem->DrawFrame(m_RenderData);
    }

    // TODO: End Slate frame
  }


