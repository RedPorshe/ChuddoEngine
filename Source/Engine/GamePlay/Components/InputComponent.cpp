#include "Engine/GamePlay/Components/InputComponent.h"
#include "Engine/GamePlay/Input/InputSystem.h"
#include <SDL3/SDL.h>

CInputComponent::CInputComponent(CObject* Owner, FString NewName)
    : CComponent(Owner, NewName)
    , m_MouseSensitivity(0.1f)
{
    // Setup default mappings
    SetupDefaultMappings();

    // Register with input system
    CInputSystem::Get().RegisterInputComponent(this);

    CORE_DEBUG("InputComponent created: %s", NewName.c_str());
}

CInputComponent::~CInputComponent()
{
    // Unregister from input system
    CInputSystem::Get().UnregisterInputComponent(this);

    CORE_DEBUG("InputComponent destroyed: %s", GetName().c_str());
}

// Bind action
void CInputComponent::BindAction(const FString& ActionName, EInputEvent EventType, std::function<void()> Callback)
{
    m_ActionBindings[ActionName].push_back({Callback, EventType});
    CORE_DEBUG("Action bound: %s (event type: %d)", ActionName.c_str(), static_cast<int>(EventType));
}

// Bind axis
void CInputComponent::BindAxis(const FString& AxisName, std::function<void(float)> Callback, float Scale)
{
    m_AxisBindings[AxisName] = {Callback, Scale};
    CORE_DEBUG("Axis bound: %s (scale: %.2f)", AxisName.c_str(), Scale);
}

// Trigger action
void CInputComponent::TriggerAction(const FString& ActionName, EInputEvent EventType)
{
    auto it = m_ActionBindings.find(ActionName);
    if (it != m_ActionBindings.end())
    {
        for (const auto& binding : it->second)
        {
            if (binding.EventType == EventType)
            {
                binding.Callback();
            }
        }
    }
}

// Trigger axis
void CInputComponent::TriggerAxis(const FString& AxisName, float Value)
{
    auto it = m_AxisBindings.find(AxisName);
    if (it != m_AxisBindings.end())
    {
        float scaledValue = Value * it->second.Scale;
        it->second.Callback(scaledValue);
    }
}

// State queries
bool CInputComponent::IsKeyDown(SDL_Keycode Key) const
{
    return CInputSystem::Get().IsKeyDown(Key);
}

bool CInputComponent::IsMouseButtonDown(Uint8 Button) const
{
    return CInputSystem::Get().IsMouseButtonDown(Button);
}

FVector2D CInputComponent::GetMousePosition() const
{
    return CInputSystem::Get().GetMousePosition();
}

FVector2D CInputComponent::GetMouseDelta() const
{
    return CInputSystem::Get().GetMouseDelta();
}

// Update
void CInputComponent::Update(float DeltaTime)
{
    CComponent::Update(DeltaTime);
}

// Setup default mappings
void CInputComponent::SetupDefaultMappings()
{
    // Movement axes
    CInputSystem::Get().AddKeyAxisMapping(SDLK_W, "MoveForward", 1.0f);
    CInputSystem::Get().AddKeyAxisMapping(SDLK_S, "MoveForward", -1.0f);
    CInputSystem::Get().AddKeyAxisMapping(SDLK_D, "MoveRight", 1.0f);
    CInputSystem::Get().AddKeyAxisMapping(SDLK_A, "MoveRight", -1.0f);
    CInputSystem::Get().AddKeyAxisMapping(SDLK_SPACE, "MoveUp", 1.0f);
    CInputSystem::Get().AddKeyAxisMapping(SDLK_LCTRL, "MoveUp", -1.0f);

    // Actions
    CInputSystem::Get().AddKeyActionMapping(SDLK_SPACE, "Jump", EInputEvent::Pressed);
    CInputSystem::Get().AddKeyActionMapping(SDLK_LSHIFT, "Sprint", EInputEvent::Pressed);
    CInputSystem::Get().AddKeyActionMapping(SDLK_ESCAPE, "Pause", EInputEvent::Pressed);
    CInputSystem::Get().AddKeyActionMapping(SDLK_F, "Interact", EInputEvent::Pressed);
    CInputSystem::Get().AddKeyActionMapping(SDLK_R, "Reload", EInputEvent::Pressed);
    CInputSystem::Get().AddKeyActionMapping(SDLK_E, "Use", EInputEvent::Pressed);

    // Mouse
    CInputSystem::Get().AddMouseActionMapping(SDL_BUTTON_LEFT, "Fire", EInputEvent::Pressed);
    CInputSystem::Get().AddMouseActionMapping(SDL_BUTTON_RIGHT, "Aim", EInputEvent::Pressed);

    // Mouse axes
    CInputSystem::Get().AddMouseAxisMapping("MouseX", m_MouseSensitivity);
    CInputSystem::Get().AddMouseAxisMapping("MouseY", m_MouseSensitivity);
    CInputSystem::Get().AddMouseAxisMapping("MouseWheel", 1.0f);

    CORE_DEBUG("Default mappings setup complete");
}
