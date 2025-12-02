#include "Engine/Editor/UI/SlateInputComponent.h"
#include "Engine/Editor/UI/SlateCore.h"
#include "Engine/GamePlay/Input/InputSystem.h"
#include <SDL.h>

CSlateInputComponent::CSlateInputComponent()
{
}

CSlateInputComponent::~CSlateInputComponent()
{
}

void CSlateInputComponent::ProcessKey(int key, int action, float deltaTime)
{
    (void)deltaTime; // Not used for key events

    EKey slateKey = ConvertSDLKeyToSlateKey(key);
    bool bIsDown = (action == 1); // SDL_PRESS = 1

    if (bIsDown)
    {
        CSlateApplication::Get().ProcessKeyDown(slateKey);
    }
    else
    {
        CSlateApplication::Get().ProcessKeyUp(slateKey);
    }
}

void CSlateInputComponent::ProcessMouseMovement(float xoffset, float yoffset, float deltaTime)
{
    (void)xoffset;
    (void)yoffset;
    (void)deltaTime; // Not used for mouse movement

    // Get absolute mouse position from InputSystem
    FVector2D mousePos = CInputSystem::Get().GetMousePosition();
    CSlateApplication::Get().ProcessMouseMove(mousePos);
}

void CSlateInputComponent::ProcessMouseScroll(float yoffset)
{
    (void)yoffset; // Mouse scroll not implemented in Slate yet
}

EKey CSlateInputComponent::ConvertSDLKeyToSlateKey(int sdlKey) const
{
    switch (sdlKey)
    {
        // Letters
        case SDLK_A: return EKey::A;
        case SDLK_B: return EKey::B;
        case SDLK_C: return EKey::C;
        case SDLK_D: return EKey::D;
        case SDLK_E: return EKey::E;
        case SDLK_F: return EKey::F;
        case SDLK_G: return EKey::G;
        case SDLK_H: return EKey::H;
        case SDLK_I: return EKey::I;
        case SDLK_J: return EKey::J;
        case SDLK_K: return EKey::K;
        case SDLK_L: return EKey::L;
        case SDLK_M: return EKey::M;
        case SDLK_N: return EKey::N;
        case SDLK_O: return EKey::O;
        case SDLK_P: return EKey::P;
        case SDLK_Q: return EKey::Q;
        case SDLK_R: return EKey::R;
        case SDLK_S: return EKey::S;
        case SDLK_T: return EKey::T;
        case SDLK_U: return EKey::U;
        case SDLK_V: return EKey::V;
        case SDLK_W: return EKey::W;
        case SDLK_X: return EKey::X;
        case SDLK_Y: return EKey::Y;
        case SDLK_Z: return EKey::Z;

        // Numbers
        case SDLK_0: return EKey::Zero;
        case SDLK_1: return EKey::One;
        case SDLK_2: return EKey::Two;
        case SDLK_3: return EKey::Three;
        case SDLK_4: return EKey::Four;
        case SDLK_5: return EKey::Five;
        case SDLK_6: return EKey::Six;
        case SDLK_7: return EKey::Seven;
        case SDLK_8: return EKey::Eight;
        case SDLK_9: return EKey::Nine;

        // Function keys
        case SDLK_F1: return EKey::F1;
        case SDLK_F2: return EKey::F2;
        case SDLK_F3: return EKey::F3;
        case SDLK_F4: return EKey::F4;
        case SDLK_F5: return EKey::F5;
        case SDLK_F6: return EKey::F6;
        case SDLK_F7: return EKey::F7;
        case SDLK_F8: return EKey::F8;
        case SDLK_F9: return EKey::F9;
        case SDLK_F10: return EKey::F10;
        case SDLK_F11: return EKey::F11;
        case SDLK_F12: return EKey::F12;

        // Special keys
        case SDLK_ESCAPE: return EKey::Escape;
        case SDLK_RETURN: return EKey::Enter;
        case SDLK_SPACE: return EKey::Space;
        case SDLK_BACKSPACE: return EKey::Backspace;
        case SDLK_TAB: return EKey::Tab;
        case SDLK_CAPSLOCK: return EKey::CapsLock;
        case SDLK_PRINTSCREEN: return EKey::PrintScreen;
        case SDLK_SCROLLLOCK: return EKey::ScrollLock;
        case SDLK_PAUSE: return EKey::Pause;
        case SDLK_INSERT: return EKey::Insert;
        case SDLK_DELETE: return EKey::Delete;
        case SDLK_HOME: return EKey::Home;
        case SDLK_END: return EKey::End;
        case SDLK_PAGEUP: return EKey::PageUp;
        case SDLK_PAGEDOWN: return EKey::PageDown;

        // Modifier keys
        case SDLK_LSHIFT: return EKey::LeftShift;
        case SDLK_RSHIFT: return EKey::RightShift;
        case SDLK_LCTRL: return EKey::LeftCtrl;
        case SDLK_RCTRL: return EKey::RightCtrl;
        case SDLK_LALT: return EKey::LeftAlt;
        case SDLK_RALT: return EKey::RightAlt;

        // Arrow keys
        case SDLK_LEFT: return EKey::LeftArrow;
        case SDLK_RIGHT: return EKey::RightArrow;
        case SDLK_UP: return EKey::UpArrow;
        case SDLK_DOWN: return EKey::DownArrow;

        // Numpad keys
        case SDLK_NUMLOCKCLEAR: return EKey::NumLock;
        case SDLK_KP_0: return EKey::Numpad0;
        case SDLK_KP_1: return EKey::Numpad1;
        case SDLK_KP_2: return EKey::Numpad2;
        case SDLK_KP_3: return EKey::Numpad3;
        case SDLK_KP_4: return EKey::Numpad4;
        case SDLK_KP_5: return EKey::Numpad5;
        case SDLK_KP_6: return EKey::Numpad6;
        case SDLK_KP_7: return EKey::Numpad7;
        case SDLK_KP_8: return EKey::Numpad8;
        case SDLK_KP_9: return EKey::Numpad9;
        case SDLK_KP_PLUS: return EKey::NumpadAdd;
        case SDLK_KP_MINUS: return EKey::NumpadSubtract;
        case SDLK_KP_MULTIPLY: return EKey::NumpadMultiply;
        case SDLK_KP_DIVIDE: return EKey::NumpadDivide;
        case SDLK_KP_DECIMAL: return EKey::NumpadDecimal;
        case SDLK_KP_ENTER: return EKey::NumpadEnter;

        // Other keys
        case SDLK_GRAVE: return EKey::Backquote;
        case SDLK_MINUS: return EKey::Minus;
        case SDLK_EQUALS: return EKey::Equals;
        case SDLK_LEFTBRACKET: return EKey::LeftBracket;
        case SDLK_RIGHTBRACKET: return EKey::RightBracket;
        case SDLK_BACKSLASH: return EKey::Backslash;
        case SDLK_SEMICOLON: return EKey::Semicolon;
        case SDLK_APOSTROPHE: return EKey::Quote;
        case SDLK_COMMA: return EKey::Comma;
        case SDLK_PERIOD: return EKey::Period;
        case SDLK_SLASH: return EKey::Slash;

        default: return EKey::Invalid;
    }
}

EMouseButton CSlateInputComponent::ConvertSDLButtonToSlateButton(int sdlButton) const
{
    switch (sdlButton)
    {
        case SDL_BUTTON_LEFT: return EMouseButton::Left;
        case SDL_BUTTON_RIGHT: return EMouseButton::Right;
        case SDL_BUTTON_MIDDLE: return EMouseButton::Middle;
        default: return EMouseButton::Left; // Default to left button
    }
}
