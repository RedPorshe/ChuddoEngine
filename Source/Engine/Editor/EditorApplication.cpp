#include "Engine/Editor/EditorApplication.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include "Engine/Core/Rendering/Vulkan/Core/VulkanContext.h"
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

    // Initialize ImGui
    InitializeImGui();

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
    ShutdownImGui();
    Application::Shutdown();
    CORE_DISPLAY("=== EditorApplication Shutdown Complete ===");
  }

  void EditorApplication::ToggleEditorMode()
  {
    m_EditorMode = (m_EditorMode == EditorMode::Play) ? EditorMode::Edit : EditorMode::Play;
    CORE_DEBUG("Switched to ", (m_EditorMode == EditorMode::Play ? "Play" : "Edit"), " mode");
  }

  void EditorApplication::InitializeImGui()
  {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Note: Docking not available in this ImGui version

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Get Vulkan context for ImGui initialization
    VulkanContext* vulkanContext = m_RenderSystem->GetVulkanContext();
    if (!vulkanContext)
    {
      CORE_ERROR("Failed to get Vulkan context for ImGui initialization");
      return;
    }

    // Initialize ImGui for SDL2
    if (!ImGui_ImplSDL2_InitForVulkan(m_RenderSystem->GetWindow()))
    {
      CORE_ERROR("Failed to initialize ImGui SDL2 backend");
      return;
    }

    // Initialize ImGui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vulkanContext->GetInstance();
    init_info.PhysicalDevice = vulkanContext->GetPhysicalDevice();
    init_info.Device = vulkanContext->GetDevice();
    init_info.QueueFamily = vulkanContext->GetGraphicsQueueFamily();
    init_info.Queue = vulkanContext->GetGraphicsQueue();
    init_info.DescriptorPool = vulkanContext->GetDescriptorPool();
    init_info.MinImageCount = 2;
    init_info.ImageCount = vulkanContext->GetSwapchainImageCount();

    // Pipeline info for main viewport
    init_info.PipelineInfoMain.RenderPass = vulkanContext->GetRenderPass();
    init_info.PipelineInfoMain.Subpass = 0;
    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    if (!ImGui_ImplVulkan_Init(&init_info))
    {
      CORE_ERROR("Failed to initialize ImGui Vulkan backend");
      return;
    }

    // Font texture will be created automatically when first drawing

    CORE_DEBUG("ImGui initialized successfully");
  }

  void EditorApplication::ShutdownImGui()
  {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    CORE_DEBUG("ImGui shutdown");
  }

  void EditorApplication::BeginImGuiFrame()
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
  }

  void EditorApplication::EndImGuiFrame()
  {
    ImGui::Render();
    VulkanContext* vulkanContext = m_RenderSystem->GetVulkanContext();
    if (vulkanContext)
    {
      ImDrawData* drawData = ImGui::GetDrawData();
      ImGui_ImplVulkan_RenderDrawData(drawData, vulkanContext->GetCurrentCommandBuffer());
    }
  }

  void EditorApplication::RenderEditorUI()
  {
    RenderMainMenuBar();
    RenderViewport();
    RenderOutliner();
    RenderPropertyPanel();
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
    // Start ImGui frame
    BeginImGuiFrame();

    // Render editor UI
    RenderEditorUI();

    // Clear render data
    m_RenderData.Clear();

    // Collect render data from world
    if (m_GameInstance && m_GameInstance->GetCurrentWorld())
    {
      m_GameInstance->GetCurrentWorld()->CollectRenderData(m_RenderData);
    }

    // Render the frame (this will include ImGui)
    if (m_RenderSystem)
    {
      m_RenderSystem->DrawFrame(m_RenderData);
    }

    // End ImGui frame
    EndImGuiFrame();
  }

  void EditorApplication::RenderMainMenuBar()
  {
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("New Scene")) {}
        if (ImGui::MenuItem("Open Scene")) {}
        if (ImGui::MenuItem("Save Scene")) {}
        ImGui::Separator();
        if (ImGui::MenuItem("Exit")) { m_IsRunning = false; }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Edit"))
      {
        if (ImGui::MenuItem("Undo")) {}
        if (ImGui::MenuItem("Redo")) {}
        ImGui::Separator();
        if (ImGui::MenuItem("Preferences")) {}
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("View"))
      {
        ImGui::MenuItem("Outliner", nullptr, &m_ShowOutliner);
        ImGui::MenuItem("Property Panel", nullptr, &m_ShowPropertyPanel);
        ImGui::MenuItem("Viewport", nullptr, &m_ShowViewport);
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Play"))
      {
        if (ImGui::MenuItem("Play", "F5")) { SetEditorMode(EditorMode::Play); }
        if (ImGui::MenuItem("Pause")) { SetEditorMode(EditorMode::Edit); }
        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }
  }

  void EditorApplication::RenderViewport()
  {
    if (!m_ShowViewport) return;

    ImGui::Begin("Viewport", &m_ShowViewport);

    // Get the available content region
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();

    // For now, just show a placeholder
    ImGui::Text("3D Viewport");
    ImGui::Text("Size: %.0f x %.0f", viewportSize.x, viewportSize.y);

    // TODO: Integrate actual 3D rendering into ImGui window

    ImGui::End();
  }

  void EditorApplication::RenderOutliner()
  {
    if (!m_ShowOutliner) return;

    ImGui::Begin("Outliner", &m_ShowOutliner);

    if (m_GameInstance && m_GameInstance->GetCurrentWorld())
    {
      ImGui::Text("World Actors:");

      // TODO: List all actors in the world
      // For now, just show a placeholder
      ImGui::Text("No actors to display");
    }
    else
    {
      ImGui::Text("No world loaded");
    }

    ImGui::End();
  }

  void EditorApplication::RenderPropertyPanel()
  {
    if (!m_ShowPropertyPanel) return;

    ImGui::Begin("Properties", &m_ShowPropertyPanel);

    if (m_SelectedObject)
    {
      m_PropertyEditor.SetSelectedObject(m_SelectedObject);
      m_PropertyEditor.Render(m_SelectedObject);
    }
    else
    {
      ImGui::Text("No object selected");
    }

    ImGui::End();
  }