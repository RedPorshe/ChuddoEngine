#pragma once

#include <unordered_map>

#include "CoreMinimal.h"
#include "glm/glm.hpp"

typedef struct GLFWwindow GLFWwindow;

namespace CE
{
  class InputSystem
  {
   public:
    static InputSystem& Get();

    void Initialize(GLFWwindow* Window);
    void Shutdown();

    void RegisterInputComponent(class InputComponent* Component);
    void UnregisterInputComponent(class InputComponent* Component);
    void setFPSInputMode(bool mode);

    void Update(float DeltaTime);

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

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

    void ProcessKeyInput(int key, int scancode, int action, int mods);
    void ProcessMouseMovement(double xpos, double ypos);
    void ProcessMouseButton(int button, int action, int mods);
    void ProcessMouseScroll(double xoffset, double yoffset);

    GLFWwindow* m_Window = nullptr;
    std::vector<class InputComponent*> m_InputComponents;

    std::unordered_map<int, bool> m_KeyStates;

    glm::vec2 m_MousePosition{0.0f, 0.0f};
    glm::vec2 m_LastMousePosition{0.0f, 0.0f};
    glm::vec2 m_MouseDelta{0.0f, 0.0f};
    bool m_FirstMouse = true;
    bool bIsFPS{true};
    static InputSystem* s_Instance;
  };
}  // namespace CE