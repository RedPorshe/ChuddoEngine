#pragma once

#include "Math/MathConstants.hpp"
#include <cmath>
#include <cstdint>
#include <limits>
#include <iostream>

namespace CEMath
{
    class Vector2D
    {
    public:
        float x, y;
        
        // Constructors
        Vector2D() noexcept : x(0.0f), y(0.0f) {}
        explicit Vector2D(float scalar) noexcept : x(scalar), y(scalar) {}
        Vector2D(float x_, float y_) noexcept : x(x_), y(y_) {}
        
        // Access operators
        float& operator[](uint32_t index) noexcept;
        float operator[](uint32_t index) const noexcept;
        
        // Arithmetic operators
        Vector2D operator+(const Vector2D& other) const noexcept;
        Vector2D operator-(const Vector2D& other) const noexcept;
        Vector2D operator*(const Vector2D& other) const noexcept;
        Vector2D operator/(const Vector2D& other) const noexcept;
        
        Vector2D operator*(float scalar) const noexcept;
        Vector2D operator/(float scalar) const noexcept;
        
        // Assignment operators
        Vector2D& operator+=(const Vector2D& other) noexcept;
        Vector2D& operator-=(const Vector2D& other) noexcept;
        Vector2D& operator*=(const Vector2D& other) noexcept;
        Vector2D& operator/=(const Vector2D& other) noexcept;
        
        Vector2D& operator*=(float scalar) noexcept;
        Vector2D& operator/=(float scalar) noexcept;

        bool operator>(const Vector2D& other) const;
        bool operator<(const Vector2D& other) const;
        

        // Unary operators
        Vector2D operator-() const noexcept;
        
        // Comparison operators
        bool operator==(const Vector2D& other) const noexcept;
        bool operator!=(const Vector2D& other) const noexcept;
        
        // Vector operations
        float Length() const noexcept;
        float LengthSquared() const noexcept;
        Vector2D Normalized() const noexcept;
        Vector2D& Normalize() noexcept;
        
        float Dot(const Vector2D& other) const noexcept;
        float Cross(const Vector2D& other) const noexcept;
        
        // Static methods
        static float Distance(const Vector2D& a, const Vector2D& b) noexcept;
        static float DistanceSquared(const Vector2D& a, const Vector2D& b) noexcept;
        static Vector2D Lerp(const Vector2D& a, const Vector2D& b, float t) noexcept;
        static Vector2D Reflect(const Vector2D& direction, const Vector2D& normal) noexcept;
        
         friend std::ostream& operator<<(std::ostream& os, const Vector2D& vec);
        // Constants
        static const Vector2D Zero;
        static const Vector2D One;
        static const Vector2D UnitX;
        static const Vector2D UnitY;
    };
     inline std::ostream& operator<<(std::ostream& os, const Vector2D& vec)
    {
        os << "(x =" << vec.x << ", y=" << vec.y << ")";
        return os;
    }
}