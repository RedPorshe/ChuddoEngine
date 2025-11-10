#pragma once

#include <unordered_map>

#include "CoreMinimal.h"
#include "glm/glm.hpp"

// Forward declaration для GLFW
typedef struct GLFWwindow GLFWwindow;

namespace CE
{
  class InputSystem
  {
   public:
    static InputSystem& Get();

    // Инициализация
    void Initialize(GLFWwindow* Window);
    void Shutdown();

    // Регистрация компонентов ввода
    void RegisterInputComponent(class InputComponent* Component);
    void UnregisterInputComponent(class InputComponent* Component);

    // Обновление состояний
    void Update(float DeltaTime);

    // Колбэки для GLFW
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    // Состояние ввода
    bool IsKeyPressed(int key) const;
    glm::vec2 GetMousePosition() const
    {
      return m_MousePosition;
    }
    glm::vec2 GetMouseDelta() const
    {
      return m_MouseDelta;
    }

   private:
    InputSystem() = default;
    ~InputSystem() = default;

    void ProcessKeyInput(int key, int action, float deltaTime);
    void ProcessMouseMovement(float xpos, float ypos, float deltaTime);
    void ProcessMouseScroll(float xoffset, float yoffset);

    GLFWwindow* m_Window = nullptr;
    std::vector<class InputComponent*> m_InputComponents;

    // Состояние клавиш
    std::unordered_map<int, bool> m_KeyStates;

    // Состояние мыши
    glm::vec2 m_MousePosition{0.0f, 0.0f};
    glm::vec2 m_LastMousePosition{0.0f, 0.0f};
    glm::vec2 m_MouseDelta{0.0f, 0.0f};
    bool m_FirstMouse = true;

    static InputSystem* s_Instance;
  };
}  // namespace CE