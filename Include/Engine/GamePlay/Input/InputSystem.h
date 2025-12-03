#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <SDL3/SDL.h>
#include "CoreMinimal.h"
#include "Engine/Core/CoreTypes.h"
#include "Math/Vector2D.hpp"

// Forward declarations
class CInputComponent;

// Input event types
enum class EInputEvent
{
    Pressed,
    Released,
    Repeat
};

// Input action binding
struct FInputActionBinding
{
    std::function<void()> Callback;
    EInputEvent EventType;
};

// Input axis binding
struct FInputAxisBinding
{
    std::function<void(float)> Callback;
    float Scale = 1.0f;
};

// Key mapping for actions
struct FKeyActionMapping
{
    SDL_Keycode Key;
    FString ActionName;
    EInputEvent EventType;
};

// Key mapping for axes
struct FKeyAxisMapping
{
    SDL_Keycode Key;
    FString AxisName;
    float Scale = 1.0f;
};

// Mouse button mapping for actions
struct FMouseActionMapping
{
    Uint8 Button;
    FString ActionName;
    EInputEvent EventType;
};

// Mouse axis mapping (for wheel or movement)
struct FMouseAxisMapping
{
    FString AxisName; // e.g., "MouseX", "MouseY", "MouseWheel"
    float Scale = 1.0f;
};

class CInputSystem
{
public:
    // Singleton
    static CInputSystem& Get();

    // Initialization/Shutdown
    void Initialize(SDL_Window* Window);
    void Shutdown();

    // Component registration
    void RegisterInputComponent(CInputComponent* Component);
    void UnregisterInputComponent(CInputComponent* Component);

    // Input processing
    void Update(float DeltaTime);

    // Key mappings
    void AddKeyActionMapping(SDL_Keycode Key, const FString& ActionName, EInputEvent EventType);
    void AddKeyAxisMapping(SDL_Keycode Key, const FString& AxisName, float Scale = 1.0f);

    // Mouse mappings
    void AddMouseActionMapping(Uint8 Button, const FString& ActionName, EInputEvent EventType);
    void AddMouseAxisMapping(const FString& AxisName, float Scale = 1.0f);

    // State queries
    bool IsKeyDown(SDL_Keycode Key) const;
    bool IsMouseButtonDown(Uint8 Button) const;
    FVector2D GetMousePosition() const;
    FVector2D GetMouseDelta() const;

    // Quit flag
    bool ShouldQuit() const { return m_ShouldQuit; }
    void SetShouldQuit(bool quit) { m_ShouldQuit = quit; }

private:
    CInputSystem();
    ~CInputSystem() = default;

    // Process SDL events
    void ProcessEvent(const SDL_Event& Event);

    // Trigger actions/axes
    void TriggerAction(const FString& ActionName, EInputEvent EventType);
    void TriggerAxis(const FString& AxisName, float Value);

    static CInputSystem* s_Instance;

    SDL_Window* m_Window = nullptr;
    bool m_ShouldQuit = false;

    // Registered components
    std::vector<CInputComponent*> m_InputComponents;

    // Key states
    std::unordered_map<SDL_Keycode, bool> m_KeyStates;
    std::unordered_map<SDL_Keycode, bool> m_PreviousKeyStates;

    // Mouse states
    std::unordered_map<Uint8, bool> m_MouseButtonStates;
    FVector2D m_MousePosition{0.0f, 0.0f};
    FVector2D m_LastMousePosition{0.0f, 0.0f};
    FVector2D m_MouseDelta{0.0f, 0.0f};

    // Mappings
    std::vector<FKeyActionMapping> m_KeyActionMappings;
    std::vector<FKeyAxisMapping> m_KeyAxisMappings;
    std::vector<FMouseActionMapping> m_MouseActionMappings;
    std::vector<FMouseAxisMapping> m_MouseAxisMappings;

    // For FPS mode (capture mouse)
    bool m_FPSMode = false;
    bool m_FirstMouse = true;
};
