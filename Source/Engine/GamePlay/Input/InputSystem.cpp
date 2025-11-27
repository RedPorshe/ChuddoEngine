#include "Engine/GamePlay/Input/InputSystem.h"

#include <GLFW/glfw3.h>

#include "Engine/GamePlay/Components/InputComponent.h"

namespace CE
{
  CInputSystem* CInputSystem::s_Instance = nullptr;

  CInputSystem& CInputSystem::Get()
  {
    static CInputSystem instance;
    s_Instance = &instance;
    return instance;
  }

  void CInputSystem::Initialize(GLFWwindow* Window)
  {
    m_Window = Window;

    if (m_Window)
    {
      glfwSetWindowUserPointer(m_Window, this);

      glfwSetKeyCallback(m_Window, KeyCallback);
      glfwSetCursorPosCallback(m_Window, MouseCallback);
      glfwSetScrollCallback(m_Window, ScrollCallback);
      glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
      if (bIsFPS)
      {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }
      else
      {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      }
      double xpos, ypos;
      glfwGetCursorPos(m_Window, &xpos, &ypos);
      m_LastMousePosition = Math::Vector2f(xpos, ypos);
      m_MousePosition = Math::Vector2f(xpos, ypos);

      CE_CORE_DEBUG("InputSystem initialized with GLFW window");
    }
    else
    {
      CE_CORE_ERROR("InputSystem: No GLFW window provided");
    }
  }

  void CInputSystem::Shutdown()
  {
    m_InputComponents.clear();
    m_Window = nullptr;
    CE_CORE_DEBUG("InputSystem shutdown");
  }

  void CInputSystem::ProcessMouseButton(int button, int action, int mods)
  {
    (void)mods;

    m_KeyStates[button] = (action != GLFW_RELEASE);
  }

  void CInputSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
  {
    CInputSystem* inputSystem = static_cast<CInputSystem*>(glfwGetWindowUserPointer(window));
    if (inputSystem)
    {
      inputSystem->ProcessMouseButton(button, action, mods);
    }
  }

  void CInputSystem::RegisterInputComponent(CInputComponent* Component)
  {
    if (Component && std::find(m_InputComponents.begin(), m_InputComponents.end(), Component) == m_InputComponents.end())
    {
      m_InputComponents.push_back(Component);
      CE_CORE_DEBUG("InputComponent registered to InputSystem");
    }
  }

  void CInputSystem::UnregisterInputComponent(CInputComponent* Component)
  {
    auto it = std::find(m_InputComponents.begin(), m_InputComponents.end(), Component);
    if (it != m_InputComponents.end())
    {
      m_InputComponents.erase(it);
      CE_CORE_DEBUG("InputComponent unregistered from InputSystem");
    }
  }

  void CInputSystem::setFPSInputMode(bool mode)
  {
    if (mode)
    {
      glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
      glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }
  void CInputSystem::Update(float DeltaTime)
  {
    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->Update(DeltaTime);
      }
    }

    m_MouseDelta = Math::Vector2f(0.0f, 0.0f);
  }

  void CInputSystem::ProcessKeyInput(int key, int scancode, int action, int mods)
  {
    (void)scancode;
    (void)mods;
    m_KeyStates[key] = (action != GLFW_RELEASE);

    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->ProcessKey(key, action, 0.016f);
      }
    }
  }

  void CInputSystem::ProcessMouseMovement(double xpos, double ypos)
  {
    float x = static_cast<float>(xpos);
    float y = static_cast<float>(ypos);

    if (m_FirstMouse)
    {
      m_LastMousePosition = Math::Vector2f(x, y);
      m_FirstMouse = false;
      return;
    }

    float xOffset = x - m_LastMousePosition.x;
    float yOffset = m_LastMousePosition.y - y;

    m_LastMousePosition = Math::Vector2f(x, y);
    m_MousePosition = Math::Vector2f(x, y);
    m_MouseDelta = Math::Vector2f(xOffset, yOffset);

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

  void CInputSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    CInputSystem* inputSystem = static_cast<CInputSystem*>(glfwGetWindowUserPointer(window));
    if (inputSystem)
    {
      inputSystem->ProcessKeyInput(key, scancode, action, mods);
    }
  }

  void CInputSystem::MouseCallback(GLFWwindow* window, double xpos, double ypos)
  {
    CInputSystem* inputSystem = static_cast<CInputSystem*>(glfwGetWindowUserPointer(window));
    if (inputSystem)
    {
      inputSystem->ProcessMouseMovement(xpos, ypos);
    }
  }

  void CInputSystem::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
  {
    CInputSystem* inputSystem = static_cast<CInputSystem*>(glfwGetWindowUserPointer(window));
    if (inputSystem)
    {
      inputSystem->ProcessMouseScroll(xoffset, yoffset);
    }
  }
}  // namespace CE