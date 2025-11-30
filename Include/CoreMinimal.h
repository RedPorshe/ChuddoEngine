#pragma once

// Core types
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
#define CE_PLATFORM_WINDOWS 1
#elif defined(__linux__)
#define CE_PLATFORM_LINUX 1
#elif defined(__APPLE__)
#define CE_PLATFORM_MAC  ​1
#else
#define CE_PLATFORM_UNKNOWN 1
#endif

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;
// Core type definitions
// Integer types
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

// Floating point types
using float32 = float;
using float64 = double;

// Character types
using Char8 = char;
using Char16 = char16_t;
using Char32 = char32_t;

// String types
using FString = std::string;
using WString = std::wstring;

// Smart pointers
template <typename T>
using TUniquePtr = std::unique_ptr<T>;

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

template <typename T>
using TWeakPtr = std::weak_ptr<T>;

// Forward declarations
class CLogger;
class CActor;
class CComponent;
class CWorld;
#include "Engine/Core/AppInfo.h"
#include "Engine/Utils/Logger.h"
