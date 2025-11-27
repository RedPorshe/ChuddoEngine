#pragma once
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdint>  // Добавлено для uint32_t

namespace CE::Math
{
    constexpr float PI = 3.14159265358979323846f;
    constexpr float TWO_PI = 2.0f * PI;
    constexpr float HALF_PI = 0.5f * PI;
    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;
    constexpr float EPSILON = 1e-6f;
    constexpr float FLOAT_MAX = 3.402823466e+38f;
    constexpr float FLOAT_MIN = 1.175494351e-38f;

    inline bool IsPowerOfTwo(uint32_t value)
    {
        return value != 0 && (value & (value - 1)) == 0;
    }

     inline bool Approximately(float a, float b, float epsilon = EPSILON)
    {
        return std::abs(a - b) <= epsilon;
    }
    
    inline uint32_t NextPowerOfTwo(uint32_t value)
    {
        if (value == 0) return 1;
        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        return value + 1;
    }

    template<typename T>
    inline T Clamp(T value, T min, T max)
    {
        return (value < min) ? min : (value > max) ? max : value;
    }

    template<typename T>
    inline T Lerp(T a, T b, float t)
    {
        return a + (b - a) * t;
    }

    inline float ToRadians(float degrees)
    {
        return degrees * DEG_TO_RAD;
    }

    inline float ToDegrees(float radians)
    {
        return radians * RAD_TO_DEG;
    }
}