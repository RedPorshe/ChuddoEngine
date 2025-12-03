#pragma once

#include "AppInfo.h"
#include "Engine/Core/Rendering/Vulkan/Integration/RenderSystem.h"
#include "Engine/GamePlay/Input/InputSystem.h"
#include <memory>

class Application
{
public:
    Application(AppInfo* info);
    virtual ~Application() = default;

    virtual void Initialize();
    virtual void Run();
    virtual void Shutdown();

protected:
    AppInfo* m_AppInfo;
    std::unique_ptr<RenderSystem> m_RenderSystem;
    std::unique_ptr<CInputSystem> m_InputSystem;
    bool m_IsRunning = false;
};
