#pragma once

#include "Math/MathConstants.hpp"
#include "Math/Vector2D.hpp"
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace CEMath
{
  class Quaternion;
  class Vector3D
  {
   public:
    float x, y, z;

    // Constructors
    Vector3D() noexcept : x(0.0f), y(0.0f), z(0.0f)
    {
    }
    explicit Vector3D(float scalar) noexcept : x(scalar), y(scalar), z(scalar)
    {
    }
    Vector3D(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_)
    {
    }
    Vector3D(const Vector2D& vec2, float z_ = 0.0f) noexcept : x(vec2.x), y(vec2.y), z(z_)
    {
    }

    // Access operators
    float& operator[](uint32_t index) noexcept;
    float operator[](uint32_t index) const noexcept;

    // Conversion to Vector2D
    Vector2D ToVector2D() const noexcept
    {
      return Vector2D(x, y);
    }

    // Arithmetic operators
    Vector3D operator+(const Vector3D& other) const noexcept;
    Vector3D operator-(const Vector3D& other) const noexcept;
    Vector3D operator*(const Vector3D& other) const noexcept;
    Vector3D operator*(const Quaternion& other) const noexcept;
    Vector3D operator/(const Vector3D& other) const noexcept;

    Vector3D operator*(float scalar) const noexcept;
    Vector3D operator/(float scalar) const noexcept;

    // Assignment operators
    Vector3D& operator+=(const Vector3D& other) noexcept;
    Vector3D& operator-=(const Vector3D& other) noexcept;
    Vector3D& operator*=(const Vector3D& other) noexcept;
    Vector3D& operator/=(const Vector3D& other) noexcept;

    Vector3D& operator*=(float scalar) noexcept;
    Vector3D& operator/=(float scalar) noexcept;

    // Unary operators
    Vector3D operator-() const noexcept;

    // Comparison operators
    bool operator==(const Vector3D& other) const noexcept;
    bool operator!=(const Vector3D& other) const noexcept;

    // Vector operations
    float Length() const noexcept;
    float LengthSquared() const noexcept;
    Vector3D Normalized() const noexcept;
    Vector3D& Normalize() noexcept;

    float Dot(const Vector3D& other) const noexcept;
    Vector3D Cross(const Vector3D& other) const noexcept;

    // Vector transformations
    Vector3D RotateX(float angle) const noexcept;
    Vector3D RotateY(float angle) const noexcept;
    Vector3D RotateZ(float angle) const noexcept;

    // Static methods
    static float Distance(const Vector3D& a, const Vector3D& b) noexcept;
    static float DistanceSquared(const Vector3D& a, const Vector3D& b) noexcept;
    static Vector3D Lerp(const Vector3D& a, const Vector3D& b, float t) noexcept;
    static Vector3D Slerp(const Vector3D& a, const Vector3D& b, float t) noexcept;
    static Vector3D Reflect(const Vector3D& direction, const Vector3D& normal) noexcept;
    static Vector3D Project(const Vector3D& vector, const Vector3D& onto) noexcept;

    // Component-wise Min/Max operations
    static Vector3D Min(const Vector3D& a, const Vector3D& b) noexcept;
    static Vector3D Max(const Vector3D& a, const Vector3D& b) noexcept;

    // Component-wise operations with scalar
    static Vector3D Min(const Vector3D& a, float scalar) noexcept;
    static Vector3D Max(const Vector3D& a, float scalar) noexcept;

    // Clamp vector components
    static Vector3D Clamp(const Vector3D& value, const Vector3D& min, const Vector3D& max) noexcept;
    static Vector3D Clamp(const Vector3D& value, float min, float max) noexcept;

    // Get min/max component values
    float MinComponent() const noexcept;
    float MaxComponent() const noexcept;
    int32_t MinComponentIndex() const noexcept;
    int32_t MaxComponentIndex() const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const Vector3D& vec);
    std::string ToString() const;

    // Constants
    static const Vector3D Zero;
    static const Vector3D One;
    static const Vector3D UnitX;
    static const Vector3D UnitY;
    static const Vector3D UnitZ;
  };
    
    inline std::ostream& operator<<(std::ostream& os, const Vector3D& vec)
    {
        os << "(x=" << vec.x << ", y=" << vec.y << ", z=" << vec.z << ")";
        return os;
    }
}