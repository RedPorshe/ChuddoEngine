#include "Engine/Application/Application.h"

#include <chrono>
#include <SDL3/SDL.h>

#include "Engine/GamePlay/Input/InputSystem.h"

float CEGetCurrentTime()
{
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  return std::chrono::duration<float>(currentTime - startTime).count();
}

Application::Application(AppInfo* info) : m_info{info}
{
  CORE_DEBUG("Application created");
}

Application::~Application()
{
  Shutdown();
}

void Application::Initialize()
{
  CORE_DISPLAY("=== Initializing Application ===");

  // 1. Инициализация рендер-системы
  m_RenderSystem = std::make_unique<RenderSystem>(m_info);
  m_RenderSystem->Initialize();

  // 2. Инициализация системы ввода
  SDL_Window* window = m_RenderSystem->GetWindow();
  if (window)
  {
    CInputSystem::Get().Initialize(window);
    CORE_DEBUG("Input system initialized with SDL window");
  }
  else
  {
    CORE_ERROR("Failed to get SDL window for input system");
  }



  
  m_RenderData.SetupDefaultLighting();  



  m_LastFrameTime = CEGetCurrentTime();

  CORE_DISPLAY("=== Application Initialized ===");
}

void Application::Run()
{
  CORE_DEBUG("Starting main loop");

  // ВЫЗЫВАЕМ BeginPlay ПЕРЕД ОСНОВНЫМ ЦИКЛОМ
  if (m_GameInstance)
  {
    m_GameInstance->BeginPlay();

    // Настраиваем освещение после того как мир загружен в BeginPlay
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
  const bool* state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_ESCAPE])
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
  CORE_DISPLAY("=== Shutting Down Application ===");

  m_IsRunning = false;


  CInputSystem::Get().Shutdown();

  
  if (m_GameInstance)
  {
    m_GameInstance->Shutdown();
    m_GameInstance.reset();
  }

  
  if (m_RenderSystem)
  {
    m_RenderSystem->Shutdown();
    m_RenderSystem.reset();
  }

  CORE_DISPLAY("=== Application Shutdown Complete ===");
}
