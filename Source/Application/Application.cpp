#include "Application/Application.h"

#include "test.h"  // Твой тестовый мир

namespace CE
{

  float CEGetCurrentTime()
  {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float>(currentTime - startTime).count();
  }

  Application::Application()
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

    // Инициализация рендерера (пока заглушка)
    // m_Renderer = std::make_unique<VulkanRenderer>();
    // m_Renderer->Initialize();

    // Создание мира
    m_World = std::make_unique<GameWorld>();

    // Настройка данных рендеринга
    m_RenderData.SetupDefaultLighting();

    // Запуск игрового процесса
    m_World->BeginPlay();

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
      static int FRAMES = 0;
      // FPS счетчик
      m_FrameCount++;
      m_FPSTimer += m_DeltaTime;
      if (m_FPSTimer >= 1.0f)
      {
        CE_CORE_DEBUG("FPS: ", m_FrameCount, " DeltaTime: ", m_DeltaTime);
        m_FrameCount = 0;
        m_FPSTimer = 0.0f;
      }
      ++FRAMES;
      if (FRAMES == 5)
      {
        CE_CORE_DEBUG("Frames count reach 5 request exit");
        m_IsRunning = false;
      }
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
    // Обработка ввода (позже)
    // if (ShouldClose()) m_IsRunning = false;
  }

  void Application::Update()
  {
    // Обновление игровой логики
    m_World->Tick(m_DeltaTime);
  }

  void Application::Render()
  {
    // Сбор данных для рендеринга
    m_RenderData.Clear();
    m_World->CollectRenderData(m_RenderData);

    // Рендеринг кадра
    // if (m_Renderer) {
    //     m_Renderer->DrawFrame(m_RenderData);
    // }

    // Временный вывод для отладки
    if (m_FrameCount % 60 == 0)  // Каждую секунду при 60 FPS
    {
      CE_CORE_DEBUG("Rendering frame - Objects: ", m_RenderData.renderObjects.size());
    }
  }

  void Application::Shutdown()
  {
    CE_CORE_DISPLAY("=== Shutting Down Application ===");

    m_IsRunning = false;

    // if (m_Renderer) {
    //     m_Renderer->Shutdown();
    // }

    CE_CORE_DISPLAY("=== Application Shutdown Complete ===");
  }
}  // namespace CE