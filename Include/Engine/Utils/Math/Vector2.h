#pragma once
#include "MathConstants.h"
#include <iostream>
#include <vulkan/vulkan.h>

namespace CEMath
{
    template<typename T>
    class Vector2
    {
    public:
        T x, y;

        Vector2() : x(T(0)), y(T(0)) {}
        Vector2(T zero): x(T(zero)), y(T(zero)) {}
        Vector2(T inX, T inY) : x(inX), y(inY) {}
        explicit Vector2(const VkOffset2D& offset) : x(T(offset.x)), y(T(offset.y)) {}
        explicit Vector2(const VkExtent2D& extent) : x(T(extent.width)), y(T(extent.height)) {}

        // Базовые операции
        Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
        Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
        Vector2 operator*(T scalar) const { return Vector2(x * scalar, y * scalar); }
        Vector2 operator/(T scalar) const { return Vector2(x / scalar, y / scalar); }
        Vector2 operator*(const Vector2& other) const { return Vector2(x * other.x, y * other.y); }
        Vector2 operator/(const Vector2& other) const { return Vector2(x / other.x, y / other.y); }

        Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
        Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
        Vector2& operator*=(T scalar) { x *= scalar; y *= scalar; return *this; }
        Vector2& operator/=(T scalar) { x /= scalar; y /= scalar; return *this; }

        Vector2 operator-() const { return Vector2(-x, -y); }
        
        bool operator==(const Vector2& other) const
        {
            return Approximately(static_cast<float>(x), static_cast<float>(other.x)) &&
                   Approximately(static_cast<float>(y), static_cast<float>(other.y));
        }
        bool operator!=(const Vector2& other) const { return !(*this == other); }

        // Математические функции
        T Length() const { return static_cast<T>(std::sqrt(x * x + y * y)); }
        T LengthSquared() const { return x * x + y * y; }
        T Dot(const Vector2& other) const { return x * other.x + y * other.y; }
        T Cross(const Vector2& other) const { return x * other.y - y * other.x; }

        Vector2 Normalized() const 
        { 
            T len = Length();
            return (len > T(0)) ? *this / len : Vector2(T(0), T(0));
        }

        void Normalize() { *this = Normalized(); }

        // Утилиты
        static Vector2 Lerp(const Vector2& a, const Vector2& b, T t) { return a + (b - a) * t; }
        static Vector2 Min(const Vector2& a, const Vector2& b) { 
            return Vector2(std::min(a.x, b.x), std::min(a.y, b.y)); 
        }
        static Vector2 Max(const Vector2& a, const Vector2& b) { 
            return Vector2(std::max(a.x, b.x), std::max(a.y, b.y)); 
        }
        
        Vector2 Abs() const { return Vector2(std::abs(x), std::abs(y)); }
        T MaxComponent() const { return std::max(x, y); }
        T MinComponent() const { return std::min(x, y); }

        // Vulkan преобразования
        VkOffset2D ToVkOffset() const { return VkOffset2D{ static_cast<int32_t>(x), static_cast<int32_t>(y) }; }
        VkExtent2D ToVkExtent() const { return VkExtent2D{ static_cast<uint32_t>(x), static_cast<uint32_t>(y) }; }

        // Для текстурных координат
        Vector2 FlipY() const { return Vector2(x, T(1) - y); }
        
        // Преобразование координат
        Vector2 ToPixelSpace(const Vector2& viewportSize) const { return *this * viewportSize; }
        Vector2 ToNormalizedSpace(const Vector2& viewportSize) const { return *this / viewportSize; }
    };

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vector2<T>& vec)
    {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
    }

    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int32_t>;
    using Vector2u = Vector2<uint32_t>;
}
