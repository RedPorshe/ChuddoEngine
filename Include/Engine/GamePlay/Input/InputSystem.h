#pragma once

#include <unordered_map>
#include <SDL3/SDL.h>
#include "CoreMinimal.h"
#include "Engine/Utils/Math/AllMath.h"

typedef struct GLFWwindow GLFWwindow;


  class CInputSystem
  {
   public:
    static CInputSystem& Get();

    void Initialize(SDL_Window* Window);
    void Shutdown();

    void RegisterInputComponent(class CInputComponent* Component);
    void UnregisterInputComponent(class CInputComponent* Component);
    void setFPSInputMode(bool mode);

    void Update(float DeltaTime);

    bool IsKeyPressed(int key) const;
    CEMath::Vector2f GetMousePosition() const
    {
      return m_MousePosition;
    }
    CEMath::Vector2f GetMouseDelta() const
    {
      return m_MouseDelta;
    }

   private:
    CInputSystem() = default;
    ~CInputSystem() = default;

    void ProcessKeyInput(int key, int scancode, bool down, int mods);
    void ProcessMouseMovement(double xpos, double ypos);
    void ProcessMouseButton(int button, bool down, int mods);
    void ProcessMouseScroll(double xoffset, double yoffset);

    SDL_Window* m_Window = nullptr;
    std::vector<class CInputComponent*> m_InputComponents;

    std::unordered_map<int, bool> m_KeyStates;

    CEMath::Vector2f m_MousePosition{0.0f, 0.0f};
    CEMath::Vector2f m_LastMousePosition{0.0f, 0.0f};
    CEMath::Vector2f m_MouseDelta{0.0f, 0.0f};
    bool m_FirstMouse = true;
    bool bIsFPS{true};
    static CInputSystem* s_Instance;
  };