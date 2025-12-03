#include "Engine/Core/Application.h"
#include "Engine/Core/Rendering/Vulkan/Integration/RenderSystem.h"
#include "Engine/GamePlay/Input/InputSystem.h"
#include <chrono>
#include <thread>

Application::Application(AppInfo* info) : m_AppInfo(info)
{
}

void Application::Initialize()
{
    CORE_DISPLAY("=== Initializing Application ===");

    // Initialize RenderSystem
    m_RenderSystem = std::make_unique<RenderSystem>(m_AppInfo);
    m_RenderSystem->Initialize();

    // Initialize InputSystem with the window from RenderSystem
    // Note: We need to get the window from VulkanContext through RenderSystem
    // For now, we'll initialize InputSystem without window and set it later
    m_InputSystem = std::make_unique<CInputSystem>();
    // m_InputSystem->Initialize(window); // TODO: Get window from RenderSystem

    CORE_DISPLAY("=== Application Initialized ===");
}

void Application::Run()
{
    CORE_DISPLAY("=== Starting main loop ===");

    m_IsRunning = true;

    auto lastTime = std::chrono::high_resolution_clock::now();
    const double targetFrameTime = 1.0 / 60.0; // 60 FPS

    while (m_IsRunning)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastTime;
        float deltaTime = elapsed.count();

        // Poll events
        m_RenderSystem->PollEvents();

        // Update input
        if (m_InputSystem)
        {
            m_InputSystem->Update(deltaTime);
        }

        // Check for quit
        if (m_RenderSystem->ShouldClose() || (m_InputSystem && m_InputSystem->ShouldQuit()))
        {
            m_IsRunning = false;
            break;
        }

        // TODO: Update game logic here

        // TODO: Render frame here
        // For now, just sleep to maintain frame rate
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS

        lastTime = currentTime;
    }

    CORE_DISPLAY("=== Main loop ended ===");
}

void Application::Shutdown()
{
    CORE_DISPLAY("=== Shutting Down Application ===");

    if (m_InputSystem)
    {
        m_InputSystem->Shutdown();
        m_InputSystem.reset();
    }

    if (m_RenderSystem)
    {
        m_RenderSystem->Shutdown();
        m_RenderSystem.reset();
    }

    CORE_DISPLAY("=== Application Shutdown Complete ===");
}
