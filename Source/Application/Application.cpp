#include "Application/Application.h"

#include <chrono>

#include "Input/InputSystem.h"
#include "test.h"  // Твой тестовый мир

namespace CE
{
  float CEGetCurrentTime()
  {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float>(currentTime - startTime).count();
  }

  Application::Application(AppInfo* info) : m_info{info}
  {
    CE_CORE_DEBUG("Application created");
  }

  Application::~Application()
  {
    Shutdown();
  }

  void Application::Initialize()
  {
    CE_CORE_DISPLAY("=== Initializing Application ===");

    m_RenderSystem = std::make_unique<RenderSystem>(m_info);
    m_RenderSystem->Initialize();

    GLFWwindow* window = m_RenderSystem->GetWindow();
    if (window)
    {
      InputSystem::Get().Initialize(window);
      CE_CORE_DEBUG("Input system initialized with GLFW window");
    }
    else
    {
      CE_CORE_ERROR("Failed to get GLFW window for input system");
    }

    m_GameInstance = std::make_unique<CEGameInstance>();
    m_GameInstance->Initialize();

    m_GameInstance->CreateWorld("MainGameWorld");

    // Настраиваем мир ДО загрузки
    if (auto* world = m_GameInstance->GetWorld("MainGameWorld"))
    {
      auto testLevel = std::make_unique<TestWorld>();
      world->AddLevel(std::move(testLevel));
      world->SetCurrentLevel("TestWorld");
    }

    if (auto* world = m_GameInstance->GetWorld("MainGameWorld"))
    {
      // Create a few lights with different positions/colors/intensities for testing
      CE::LightingUBO worldLighting{};
      worldLighting.lightCount = 3;

      // Key light (warm)
      worldLighting.lightPositions[0] = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
      worldLighting.lightColors[0] = glm::vec4(1.0f, 0.95f, 0.9f, 2.0f);

      // Fill light (cool)
      worldLighting.lightPositions[1] = glm::vec4(-4.0f, 3.0f, 2.0f, 1.0f);
      worldLighting.lightColors[1] = glm::vec4(0.4f, 0.6f, 1.0f, 1.2f);

      // Rim/back light (magenta-ish)
      worldLighting.lightPositions[2] = glm::vec4(0.0f, 4.0f, -6.0f, 1.0f);
      worldLighting.lightColors[2] = glm::vec4(1.0f, 0.4f, 0.6f, 1.0f);

      worldLighting.ambientColor = glm::vec4(0.05f, 0.05f, 0.06f, 0.25f);

      world->SetDefaultLighting(worldLighting);
      CE_CORE_DEBUG("Application set world default lighting with ", worldLighting.lightCount, " lights");
    }

    // ЗАГРУЖАЕМ мир после настройки
    m_GameInstance->LoadWorld("MainGameWorld");

    // Настройка данных рендеринга
    m_RenderData.SetupDefaultLighting();

    if (auto* world = m_GameInstance->GetWorld("MainGameWorld"))
    {
      m_RenderData.lighting = world->GetDefaultLighting();
      CE_CORE_DEBUG("RenderData lighting initialized from world default (lights=", m_RenderData.lighting.lightCount, ")");
    }

    // Запуск игрового процесса
    m_GameInstance->BeginPlay();

    m_IsRunning = true;
    m_LastFrameTime = CEGetCurrentTime();

    CE_CORE_DISPLAY("=== Application Initialized ===");
  }

  void Application::Run()
  {
    CE_CORE_DEBUG("Starting main loop");

    while (m_IsRunning)
    {
      CalculateDeltaTime();
      ProcessInput();
      Update();
      Render();

      // FPS счетчик
      m_FrameCount++;
      m_FPSTimer += m_DeltaTime;
      if (m_FPSTimer >= 1.0f)
      {
        CE_CORE_DISPLAY("FPS: ", m_FrameCount, " DeltaTime: ", m_DeltaTime);
        m_FrameCount = 0;
        m_FPSTimer = 0.0f;
      }

      // static int FRAMES = 0;
      // ++FRAMES;
      // if (FRAMES == 3)
      // {
      //   CE_CORE_DEBUG("Frames count reach 3 request exit");
      //   m_IsRunning = false;
      // }
      // m_IsRunning = !m_RenderSystem->ShouldClose();  // delete FRAMES and this after stop debug rendering
      m_RenderSystem->PollEvents();
    }
  }

  void Application::CalculateDeltaTime()
  {
    float currentTime = CEGetCurrentTime();
    m_DeltaTime = currentTime - m_LastFrameTime;
    m_LastFrameTime = currentTime;
  }

  void Application::ProcessInput()
  {
    glfwPollEvents();

    if (m_RenderSystem->ShouldClose())
    {
      m_IsRunning = false;
    }
    if (glfwGetKey(m_RenderSystem->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
      m_IsRunning = false;
    }
  }

  void Application::Update()
  {
    InputSystem::Get().Update(m_DeltaTime);

    if (m_GameInstance)
    {
      m_GameInstance->Tick(m_DeltaTime);
    }
  }

  void Application::Render()
  {
    m_RenderData.Clear();

    if (m_GameInstance && m_GameInstance->GetCurrentWorld())
    {
      m_GameInstance->GetCurrentWorld()->CollectRenderData(m_RenderData);
    }

    if (m_RenderSystem)
    {
      m_RenderSystem->DrawFrame(m_RenderData);
    }
  }

  void Application::Shutdown()
  {
    CE_CORE_DISPLAY("=== Shutting Down Application ===");

    m_IsRunning = false;

    InputSystem::Get().Shutdown();

    if (m_GameInstance)
    {
      m_GameInstance->Shutdown();
    }

    if (m_RenderSystem)
    {
      m_RenderSystem->Shutdown();
    }

    CE_CORE_DISPLAY("=== Application Shutdown Complete ===");
  }
}  // namespace CE