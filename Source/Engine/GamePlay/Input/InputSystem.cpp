#include "Engine/GamePlay/Input/InputSystem.h"

#include "Engine/Core/CoreTypes.h"
#include <SDL3/SDL.h>

#include "Engine/GamePlay/Components/InputComponent.h"


  CInputSystem* CInputSystem::s_Instance = nullptr;

  CInputSystem& CInputSystem::Get()
  {
    static CInputSystem instance;
    s_Instance = &instance;
    return instance;
  }

  void CInputSystem::Initialize(SDL_Window* Window)
  {
    m_Window = Window;

    if (m_Window)
    {
      if (bIsFPS)
      {
        SDL_SetWindowRelativeMouseMode(m_Window, true);
      }
      else
      {
        SDL_SetWindowRelativeMouseMode(m_Window, false);
      }

      float x, y;
      SDL_GetMouseState(&x, &y);
      m_LastMousePosition = FVector2D(x, y);
      m_MousePosition = FVector2D(x, y);

      CORE_DEBUG("InputSystem initialized with SDL window");
    }
    else
    {
      CORE_ERROR("InputSystem: No SDL window provided");
    }
  }

  void CInputSystem::Shutdown()
  {
    m_InputComponents.clear();
    m_Window = nullptr;
    CORE_DEBUG("InputSystem shutdown");
  }

  void CInputSystem::ProcessMouseButton(int button, bool down, int mods)
  {
    (void)mods;

    m_KeyStates[button] = down;
  }

  void CInputSystem::RegisterInputComponent(CInputComponent* Component)
  {
    if (Component && std::find(m_InputComponents.begin(), m_InputComponents.end(), Component) == m_InputComponents.end())
    {
      m_InputComponents.push_back(Component);
      CORE_DEBUG("InputComponent registered to InputSystem");
    }
  }

  void CInputSystem::UnregisterInputComponent(CInputComponent* Component)
  {
    auto it = std::find(m_InputComponents.begin(), m_InputComponents.end(), Component);
    if (it != m_InputComponents.end())
    {
      m_InputComponents.erase(it);
      CORE_DEBUG(  "InputComponent unregistered from InputSystem");
    }
  }

  void CInputSystem::setFPSInputMode(bool mode)
  {
    if (mode)
    {
      SDL_SetWindowRelativeMouseMode(m_Window, true);
    }
    else
    {
      SDL_SetWindowRelativeMouseMode(m_Window, false);
    }
  }
  void CInputSystem::Update(float DeltaTime)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
          ProcessKeyInput(event.key.key, 0, event.key.down, 0);
          break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
          ProcessMouseButton(event.button.button, event.button.down, 0);
          break;
        case SDL_EVENT_MOUSE_MOTION:
          ProcessMouseMovement(event.motion.x, event.motion.y);
          break;
        case SDL_EVENT_MOUSE_WHEEL:
          ProcessMouseScroll(event.wheel.x, event.wheel.y);
          break;
        case SDL_EVENT_QUIT:
          // Handle quit event if needed
          break;
      }
    }

    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->Update(DeltaTime);
      }
    }

    m_MouseDelta = FVector2D(0.0f, 0.0f);
  }

  void CInputSystem::ProcessKeyInput(int key, int scancode, bool down, int mods)
  {
    (void)scancode;
    (void)mods;
    m_KeyStates[key] = down;

    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->ProcessKey(key, down ? 1 : 0, 0.016f);
      }
    }
  }

  void CInputSystem::ProcessMouseMovement(double xpos, double ypos)
  {
    float x = static_cast<float>(xpos);
    float y = static_cast<float>(ypos);

    if (m_FirstMouse)
    {
      m_LastMousePosition = FVector2D(x, y);
      m_FirstMouse = false;
      return;
    }

    float xOffset = x - m_LastMousePosition.x;
    float yOffset = m_LastMousePosition.y - y;

    m_LastMousePosition = FVector2D(x, y);
    m_MousePosition = FVector2D(x, y);
    m_MouseDelta = FVector2D(xOffset, yOffset);

    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->ProcessMouseMovement(xOffset, yOffset, 0.016f);
      }
    }
  }

  void CInputSystem::ProcessMouseScroll(double xoffset, double yoffset)
  {
    (void)xoffset;
    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->ProcessMouseScroll(static_cast<float>(yoffset));
      }
    }
  }

  bool CInputSystem::IsKeyPressed(int key) const
  {
    auto it = m_KeyStates.find(key);
    bool pressed = it != m_KeyStates.end() && it->second;
    return pressed;
  }