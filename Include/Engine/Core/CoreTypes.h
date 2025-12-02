#pragma once

#include "Engine/Utils/Math/Vector2.h"
#include "Engine/Utils/Math/Vector3.h"
#include "Engine/Utils/Math/Vector4.h"
#include "Engine/Utils/Math/Matrix4.h"
#include "Engine/Utils/Math/Quaternion.h"
#include "Engine/Utils/Math/Color.h"
#include <cfloat>

// Math type aliases following Unreal Engine naming conventions
using FVector2D = CEMath::Vector2f;
using FVector = CEMath::Vector3f;
using FVector4 = CEMath::Vector4f;
using FMatrix = CEMath::Matrix4f;
using FQuat = CEMath::Quaternionf;
using FLinearColor = CEMath::Color;

// Additional UI-specific types
using FString = std::string;

// Enum prefixes
enum class EMouseButton
{
    Left,
    Middle,
    Right
};

enum class EKey
{
    // Letters
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // Main keyboard numbers
    Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,

    // Function keys
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    // Special keys
    Escape,
    Enter,
    Space,
    Backspace,
    Tab,
    CapsLock,
    PrintScreen,
    ScrollLock,
    Pause,
    Insert,
    Delete,
    Home,
    End,
    PageUp,
    PageDown,

    // Modifier keys
    LeftShift,
    RightShift,
    LeftCtrl,
    RightCtrl,
    LeftAlt,
    RightAlt,

    // Arrow keys
    LeftArrow,
    RightArrow,
    UpArrow,
    DownArrow,

    // Numpad keys
    NumLock,
    Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
    Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
    NumpadAdd,
    NumpadSubtract,
    NumpadMultiply,
    NumpadDivide,
    NumpadDecimal,
    NumpadEnter,

    // Other keys
    Backquote,
    Minus,
    Equals,
    LeftBracket,
    RightBracket,
    Backslash,
    Semicolon,
    Quote,
    Comma,
    Period,
    Slash,

    // Invalid key
    Invalid
};

enum class EVisibility
{
    Visible,
    Hidden,
    Collapsed
};
