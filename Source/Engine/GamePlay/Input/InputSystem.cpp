#include "Engine/GamePlay/Input/InputSystem.h"
#include "Engine/GamePlay/Components/InputComponent.h"
#include <SDL3/SDL.h>
#include <iostream>

// Initialize static member
CInputSystem* CInputSystem::s_Instance = nullptr;

// Constructor
CInputSystem::CInputSystem()
    : m_ShouldQuit(false)
    , m_FPSMode(false)
    , m_FirstMouse(true)
{
    CORE_DEBUG("[InputSystem] Constructor called");
}

// Get instance
CInputSystem& CInputSystem::Get()
{
    static CInputSystem instance;
    if (!s_Instance)
    {
        s_Instance = &instance;
    }
    return instance;
}

// Initialize
void CInputSystem::Initialize(SDL_Window* Window)
{
    CORE_DEBUG("[InputSystem] Initialize called with window: %p", Window);

    m_Window = Window;

    if (m_Window)
    {
        // Get initial mouse state
        float x, y;
        int buttons = SDL_GetMouseState(&x, &y);
        CORE_DEBUG("[InputSystem] Initial mouse state: x=%.1f, y=%.1f, buttons=0x%08X", x, y, buttons);

        m_LastMousePosition = FVector2D(x, y);
        m_MousePosition = FVector2D(x, y);

        // Set initial mouse mode
        if (m_FPSMode)
        {
            SDL_SetWindowRelativeMouseMode(m_Window, true);
            SDL_HideCursor();
            CORE_DEBUG("[InputSystem] Initial mouse mode: FPS (captured and hidden)");
        }
        else
        {
            SDL_SetWindowRelativeMouseMode(m_Window, false);
            SDL_ShowCursor();
            CORE_DEBUG("[InputSystem] Initial mouse mode: Desktop (released and shown)");
        }

        CORE_DEBUG("[InputSystem] Initialized successfully");
    }
    else
    {
        CORE_ERROR("[InputSystem] ERROR: No SDL window provided for initialization");
    }
}

// Shutdown
void CInputSystem::Shutdown()
{
    CORE_DEBUG("[InputSystem] Shutdown called");

    if (m_Window)
    {
        CORE_DEBUG("[InputSystem] Releasing window resources");
    }

    // Clear components
    m_InputComponents.clear();

    m_Window = nullptr;

    CORE_DEBUG("[InputSystem] Shutdown complete");
}

// Register component
void CInputSystem::RegisterInputComponent(CInputComponent* Component)
{
    if (Component)
    {
        auto it = std::find(m_InputComponents.begin(), m_InputComponents.end(), Component);
        if (it == m_InputComponents.end())
        {
            m_InputComponents.push_back(Component);
            CORE_DEBUG("[InputSystem] Registered InputComponent: %p", Component);
            CORE_DEBUG("[InputSystem] Total components: %d", m_InputComponents.size());
        }
        else
        {
            CORE_DEBUG("[InputSystem] InputComponent %p already registered", Component);
        }
    }
    else
    {
        CORE_WARN("[InputSystem] Attempted to register null InputComponent");
    }
}

// Unregister component
void CInputSystem::UnregisterInputComponent(CInputComponent* Component)
{
    auto it = std::find(m_InputComponents.begin(), m_InputComponents.end(), Component);
    if (it != m_InputComponents.end())
    {
        m_InputComponents.erase(it);
        CORE_DEBUG("[InputSystem] Unregistered InputComponent: %p", Component);
        CORE_DEBUG("[InputSystem] Remaining components: %d", m_InputComponents.size());
    }
    else
    {
        CORE_WARN("[InputSystem] InputComponent %p not found for unregistration", Component);
    }
}

// Update
void CInputSystem::Update(float DeltaTime)
{
    // Save previous key states
    m_PreviousKeyStates = m_KeyStates;

    // Process SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ProcessEvent(event);
    }

    // Update mouse delta
    m_MouseDelta = FVector2D(0.0f, 0.0f);

    // Update all components
    for (auto* component : m_InputComponents)
    {
        if (component)
        {
            component->Update(DeltaTime);
        }
    }
}

// Process SDL event
void CInputSystem::ProcessEvent(const SDL_Event& Event)
{
    switch (Event.type)
    {
        case SDL_EVENT_QUIT:
            CORE_DEBUG("[InputSystem] SDL_EVENT_QUIT received");
            m_ShouldQuit = true;
            break;

        case SDL_EVENT_KEY_DOWN:
        {
            SDL_Keycode key = Event.key.key;
            bool repeat = Event.key.repeat;
            m_KeyStates[key] = true;

            // Check key action mappings
            for (const auto& mapping : m_KeyActionMappings)
            {
                if (mapping.Key == key)
                {
                    EInputEvent eventType = repeat ? EInputEvent::Repeat : EInputEvent::Pressed;
                    if (mapping.EventType == eventType)
                    {
                        TriggerAction(mapping.ActionName, eventType);
                    }
                }
            }

            // Check key axis mappings
            for (const auto& mapping : m_KeyAxisMappings)
            {
                if (mapping.Key == key)
                {
                    TriggerAxis(mapping.AxisName, mapping.Scale);
                }
            }
            break;
        }

        case SDL_EVENT_KEY_UP:
        {
            SDL_Keycode key = Event.key.key;
            m_KeyStates[key] = false;

            // Check key action mappings
            for (const auto& mapping : m_KeyActionMappings)
            {
                if (mapping.Key == key && mapping.EventType == EInputEvent::Released)
                {
                    TriggerAction(mapping.ActionName, EInputEvent::Released);
                }
            }
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            Uint8 button = Event.button.button;
            m_MouseButtonStates[button] = true;

            // Check mouse action mappings
            for (const auto& mapping : m_MouseActionMappings)
            {
                if (mapping.Button == button && mapping.EventType == EInputEvent::Pressed)
                {
                    TriggerAction(mapping.ActionName, EInputEvent::Pressed);
                }
            }
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            Uint8 button = Event.button.button;
            m_MouseButtonStates[button] = false;

            // Check mouse action mappings
            for (const auto& mapping : m_MouseActionMappings)
            {
                if (mapping.Button == button && mapping.EventType == EInputEvent::Released)
                {
                    TriggerAction(mapping.ActionName, EInputEvent::Released);
                }
            }
            break;
        }

        case SDL_EVENT_MOUSE_MOTION:
        {
            float x = static_cast<float>(Event.motion.x);
            float y = static_cast<float>(Event.motion.y);
            float relX = static_cast<float>(Event.motion.xrel);
            float relY = static_cast<float>(Event.motion.yrel);

            if (m_FirstMouse && m_FPSMode)
            {
                m_LastMousePosition = FVector2D(x, y);
                m_FirstMouse = false;
                return;
            }

            if (m_FPSMode)
            {
                m_MouseDelta = FVector2D(relX, relY);
            }
            else
            {
                float xOffset = x - m_LastMousePosition.x;
                float yOffset = y - m_LastMousePosition.y;
                m_MouseDelta = FVector2D(xOffset, yOffset);
                m_LastMousePosition = FVector2D(x, y);
            }

            m_MousePosition = FVector2D(x, y);

            // Trigger mouse axes
            TriggerAxis("MouseX", m_MouseDelta.x);
            TriggerAxis("MouseY", m_MouseDelta.y);
            break;
        }

        case SDL_EVENT_MOUSE_WHEEL:
        {
            float y = static_cast<float>(Event.wheel.y);
            TriggerAxis("MouseWheel", y);
            break;
        }

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            CORE_DEBUG("[InputSystem] Window close requested");
            m_ShouldQuit = true;
            break;

        default:
            break;
    }
}

// Trigger action
void CInputSystem::TriggerAction(const FString& ActionName, EInputEvent EventType)
{
    for (auto* component : m_InputComponents)
    {
        if (component)
        {
            component->TriggerAction(ActionName, EventType);
        }
    }
}

// Trigger axis
void CInputSystem::TriggerAxis(const FString& AxisName, float Value)
{
    for (auto* component : m_InputComponents)
    {
        if (component)
        {
            component->TriggerAxis(AxisName, Value);
        }
    }
}

// Add key action mapping
void CInputSystem::AddKeyActionMapping(SDL_Keycode Key, const FString& ActionName, EInputEvent EventType)
{
    m_KeyActionMappings.push_back({Key, ActionName, EventType});
    CORE_DEBUG("[InputSystem] Added key action mapping: %d -> %s (%d)", Key, ActionName.c_str(), static_cast<int>(EventType));
}

// Add key axis mapping
void CInputSystem::AddKeyAxisMapping(SDL_Keycode Key, const FString& AxisName, float Scale)
{
    m_KeyAxisMappings.push_back({Key, AxisName, Scale});
    CORE_DEBUG("[InputSystem] Added key axis mapping: %d -> %s (scale: %.2f)", Key, AxisName.c_str(), Scale);
}

// Add mouse action mapping
void CInputSystem::AddMouseActionMapping(Uint8 Button, const FString& ActionName, EInputEvent EventType)
{
    m_MouseActionMappings.push_back({Button, ActionName, EventType});
    CORE_DEBUG("[InputSystem] Added mouse action mapping: %d -> %s (%d)", Button, ActionName.c_str(), static_cast<int>(EventType));
}

// Add mouse axis mapping
void CInputSystem::AddMouseAxisMapping(const FString& AxisName, float Scale)
{
    m_MouseAxisMappings.push_back({AxisName, Scale});
    CORE_DEBUG("[InputSystem] Added mouse axis mapping: %s (scale: %.2f)", AxisName.c_str(), Scale);
}

// State queries
bool CInputSystem::IsKeyDown(SDL_Keycode Key) const
{
    auto it = m_KeyStates.find(Key);
    return it != m_KeyStates.end() && it->second;
}

bool CInputSystem::IsMouseButtonDown(Uint8 Button) const
{
    auto it = m_MouseButtonStates.find(Button);
    return it != m_MouseButtonStates.end() && it->second;
}

FVector2D CInputSystem::GetMousePosition() const
{
    return m_MousePosition;
}

FVector2D CInputSystem::GetMouseDelta() const
{
    return m_MouseDelta;
}
