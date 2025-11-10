#include "Input/InputSystem.h"

#include "Components/Input/InputComponent.h"

// Если GLFW еще не включен, добавим базовые определения
#ifndef GLFW_RELEASE
#define GLFW_RELEASE 0
#endif
#include <GLFW/glfw3.h>

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
      // Устанавливаем колбэки
      glfwSetKeyCallback(m_Window, KeyCallback);
      glfwSetCursorPosCallback(m_Window, MouseCallback);
      glfwSetScrollCallback(m_Window, ScrollCallback);

      // Захватываем курсор для FPS-контроля
      glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      CE_CORE_DEBUG("InputSystem initialized");
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

  void InputSystem::RegisterInputComponent(InputComponent* Component)
  {
    if (Component && std::find(m_InputComponents.begin(), m_InputComponents.end(), Component) == m_InputComponents.end())
    {
      m_InputComponents.push_back(Component);
      CE_CORE_DEBUG("InputComponent registered");
    }
  }

  void InputSystem::UnregisterInputComponent(InputComponent* Component)
  {
    auto it = std::find(m_InputComponents.begin(), m_InputComponents.end(), Component);
    if (it != m_InputComponents.end())
    {
      m_InputComponents.erase(it);
      CE_CORE_DEBUG("InputComponent unregistered");
    }
  }

  void InputSystem::Update(float DeltaTime)
  {
    // Сбрасываем дельту мыши для следующего кадра
    m_MouseDelta = glm::vec2(0.0f, 0.0f);

    // Обновляем все зарегистрированные компоненты ввода
    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->Update(DeltaTime);
      }
    }
  }

  void InputSystem::ProcessKeyInput(int key, int action, float deltaTime)
  {
    // Обновляем состояние клавиши
    m_KeyStates[key] = (action != GLFW_RELEASE);

    // Передаем событие всем зарегистрированным компонентам
    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->ProcessKey(key, action, deltaTime);
      }
    }
  }

  void InputSystem::ProcessMouseMovement(float xpos, float ypos, float deltaTime)
  {
    if (m_FirstMouse)
    {
      m_LastMousePosition = glm::vec2(xpos, ypos);
      m_FirstMouse = false;
    }

    float xOffset = xpos - m_LastMousePosition.x;
    float yOffset = m_LastMousePosition.y - ypos;  // Обратный знак для инвертированного управления

    m_LastMousePosition = glm::vec2(xpos, ypos);
    m_MousePosition = glm::vec2(xpos, ypos);
    m_MouseDelta = glm::vec2(xOffset, yOffset);

    // Передаем событие всем зарегистрированным компонентам
    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->ProcessMouseMovement(xOffset, yOffset, deltaTime);
      }
    }
  }

  void InputSystem::ProcessMouseScroll(float xoffset, float yoffset)
  {
    // Передаем событие всем зарегистрированным компонентам
    for (auto* component : m_InputComponents)
    {
      if (component)
      {
        component->ProcessMouseScroll(yoffset);
      }
    }
  }

  bool InputSystem::IsKeyPressed(int key) const
  {
    auto it = m_KeyStates.find(key);
    return it != m_KeyStates.end() && it->second;
  }

  // GLFW колбэки
  void InputSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    if (s_Instance)
    {
      s_Instance->ProcessKeyInput(key, action, 0.016f);  // Примерная дельта времени
    }
  }

  void InputSystem::MouseCallback(GLFWwindow* window, double xpos, double ypos)
  {
    if (s_Instance)
    {
      s_Instance->ProcessMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos), 0.016f);
    }
  }

  void InputSystem::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
  {
    if (s_Instance)
    {
      s_Instance->ProcessMouseScroll(static_cast<float>(xoffset), static_cast<float>(yoffset));
    }
  }
}  // namespace CE