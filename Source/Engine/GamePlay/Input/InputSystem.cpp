#include "Engine/GamePlay/Input/InputSystem.h"

#include <GLFW/glfw3.h>

#include "Engine/GamePlay/Components/InputComponent.h"

namespace CE
{
  InputSystem* InputSystem::s_Instance = nullptr;

  InputSystem& InputSystem::Get()
  {
    static InputSystem instance;
    s_Instance = &instance;
    return instance;
  }

  void InputSystem::Initialize(GLFWwindow* Window)
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
      m_LastMousePosition = glm::vec2(xpos, ypos);
      m_MousePosition = glm::vec2(xpos, ypos);

      CE_CORE_DEBUG("InputSystem initialized with GLFW window");
    }
    else
    {
      CE_CORE_ERROR("InputSystem: No GLFW window provided");
    }
  }

  void InputSystem::Shutdown()
  {
    m_InputComponents.clear();
    m_Window = nullptr;
    CE_CORE_DEBUG("InputSystem shutdown");
  }

  void InputSystem::ProcessMouseButton(int button, int action, int mods)
  {
    (void)mods;

    m_KeyStates[button] = (action != GLFW_RELEASE);
  }

  void InputSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
  {
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (inputSystem)
    {
      inputSystem->ProcessMouseButton(button, action, mods);
    }
  }

  void InputSystem::RegisterInputComponent(InputComponent* Component)
  {
    if (Component && std::find(m_InputComponents.begin(), m_InputComponents.end(), Component) == m_InputComponents.end())
    {
      m_InputComponents.push_back(Component);
      CE_CORE_DEBUG("InputComponent registered to InputSystem");
    }
  }

  void InputSystem::UnregisterInputComponent(InputComponent* Component)
  {
    auto it = std::find(m_InputComponents.begin(), m_InputComponents.end(), Component);
    if (it != m_InputComponents.end())
    {
      m_InputComponents.erase(it);
      CE_CORE_DEBUG("InputComponent unregistered from InputSystem");
    }
  }

  void InputSystem::setFPSInputMode(bool mode)
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
  void InputSystem::Update(float DeltaTime)
  {
    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->Update(DeltaTime);
      }
    }

    m_MouseDelta = glm::vec2(0.0f, 0.0f);
  }

  void InputSystem::ProcessKeyInput(int key, int scancode, int action, int mods)
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

  void InputSystem::ProcessMouseMovement(double xpos, double ypos)
  {
    float x = static_cast<float>(xpos);
    float y = static_cast<float>(ypos);

    if (m_FirstMouse)
    {
      m_LastMousePosition = glm::vec2(x, y);
      m_FirstMouse = false;
      return;
    }

    float xOffset = x - m_LastMousePosition.x;
    float yOffset = m_LastMousePosition.y - y;

    m_LastMousePosition = glm::vec2(x, y);
    m_MousePosition = glm::vec2(x, y);
    m_MouseDelta = glm::vec2(xOffset, yOffset);

    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->ProcessMouseMovement(xOffset, yOffset, 0.016f);
      }
    }
  }

  void InputSystem::ProcessMouseScroll(double xoffset, double yoffset)
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

  bool InputSystem::IsKeyPressed(int key) const
  {
    auto it = m_KeyStates.find(key);
    bool pressed = it != m_KeyStates.end() && it->second;
    return pressed;
  }

  void InputSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (inputSystem)
    {
      inputSystem->ProcessKeyInput(key, scancode, action, mods);
    }
  }

  void InputSystem::MouseCallback(GLFWwindow* window, double xpos, double ypos)
  {
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (inputSystem)
    {
      inputSystem->ProcessMouseMovement(xpos, ypos);
    }
  }

  void InputSystem::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
  {
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (inputSystem)
    {
      inputSystem->ProcessMouseScroll(xoffset, yoffset);
    }
  }
}  // namespace CE