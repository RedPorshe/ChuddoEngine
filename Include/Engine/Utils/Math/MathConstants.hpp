#pragma once

#include <numbers>

namespace CEMath
{
    // Mathematical constants
    constexpr float PI = 3.14159265358979323846f;
    constexpr float TWO_PI = 2.0f * PI;
    constexpr float HALF_PI = 0.5f * PI;
    constexpr float QUARTER_PI = 0.25f * PI;
    constexpr float INV_PI = 1.0f / PI;
    constexpr float INV_TWO_PI = 1.0f / TWO_PI;
    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;
    
    constexpr float EPSILON = 1e-6f;
    constexpr float EPSILON_SQUARED = EPSILON * EPSILON;
    
    constexpr float FLOAT_MAX = 3.402823466e+38f;
    constexpr float FLOAT_MIN = 1.175494351e-38f;
}