#include "Engine/Application/Application.h"

#include <chrono>


#include "Engine/GamePlay/Input/InputSystem.h"

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

    // 1. Инициализация рендер-системы
    m_RenderSystem = std::make_unique<RenderSystem>(m_info);
    m_RenderSystem->Initialize();

    // 2. Инициализация системы ввода
    GLFWwindow* window = m_RenderSystem->GetWindow();
    if (window)
    {
      CInputSystem::Get().Initialize(window);
      CE_CORE_DEBUG("Input system initialized with GLFW window");
    }
    else
    {
      CE_CORE_ERROR("Failed to get GLFW window for input system");
    }

    

    // 4. Настройка данных рендеринга из текущего мира
    m_RenderData.SetupDefaultLighting();  // Оставляем вызов для безопасности

    // Пока НЕ вызываем BeginPlay() - он будет вызван перед основным циклом

    m_LastFrameTime = CEGetCurrentTime();

    CE_CORE_DISPLAY("=== Application Initialized ===");
  }

  void Application::Run()
  {
    CE_CORE_DEBUG("Starting main loop");

    // ВЫЗЫВАЕМ BeginPlay ПЕРЕД ОСНОВНЫМ ЦИКЛОМ
    if (m_GameInstance)
    {
      m_GameInstance->BeginPlay();

      // Настраиваем освещение после того как мир загружен в BeginPlay
      if (auto* world = m_GameInstance->GetCurrentWorld())
      {
        m_RenderData.lighting = world->GetDefaultLighting();
        CE_CORE_DEBUG("RenderData lighting initialized from world default (lights=", m_RenderData.lighting.lightCount, ")");
      }
    }
    m_IsRunning = true;

    while (m_IsRunning)
    {
      
      CalculateDeltaTime();
      
      ProcessInput();
     
      Update();
     
      Render();    

      
      if (m_RenderSystem->ShouldClose())
      {
        m_IsRunning = false;
      }

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
    
    if (glfwGetKey(m_RenderSystem->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
      m_IsRunning = false;
    }
  }

  void Application::Update()
  {
    
    CInputSystem::Get().Update(m_DeltaTime);

   
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

    
    CInputSystem::Get().Shutdown();

    // Завершение игрового инстанса
    if (m_GameInstance)
    {
      m_GameInstance->Shutdown();
      m_GameInstance.reset();
    }

    // Завершение рендер-системы
    if (m_RenderSystem)
    {
      m_RenderSystem->Shutdown();
      m_RenderSystem.reset();
    }

    CE_CORE_DISPLAY("=== Application Shutdown Complete ===");
  }

}  // namespace CE