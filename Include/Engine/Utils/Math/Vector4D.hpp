#pragma once

#include "Math/MathConstants.hpp"
#include "Math/Vector3D.hpp"
#include <cmath>
#include <cstdint>

namespace CEMath
{
    class Vector4D
    {
    public:
        float x, y, z, w;
        
        // Constructors
        Vector4D() noexcept : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        explicit Vector4D(float scalar) noexcept : x(scalar), y(scalar), z(scalar), w(scalar) {}
        Vector4D(float x_, float y_, float z_, float w_) noexcept : x(x_), y(y_), z(z_), w(w_) {}
        Vector4D(const Vector3D& vec3, float w_ = 0.0f) noexcept : x(vec3.x), y(vec3.y), z(vec3.z), w(w_) {}
        
        // Access operators
        float& operator[](uint32_t index) noexcept;
        float operator[](uint32_t index) const noexcept;
        
        // Conversion to Vector3D
        Vector3D ToVector3D() const noexcept { return Vector3D(x, y, z); }
        
        // Arithmetic operators
        Vector4D operator+(const Vector4D& other) const noexcept;
        Vector4D operator-(const Vector4D& other) const noexcept;
        Vector4D operator*(const Vector4D& other) const noexcept;
        Vector4D operator/(const Vector4D& other) const noexcept;
        
        Vector4D operator*(float scalar) const noexcept;
        Vector4D operator/(float scalar) const noexcept;
        
        // Assignment operators
        Vector4D& operator+=(const Vector4D& other) noexcept;
        Vector4D& operator-=(const Vector4D& other) noexcept;
        Vector4D& operator*=(const Vector4D& other) noexcept;
        Vector4D& operator/=(const Vector4D& other) noexcept;
        
        Vector4D& operator*=(float scalar) noexcept;
        Vector4D& operator/=(float scalar) noexcept;
        
        // Unary operators
        Vector4D operator-() const noexcept;
        
        // Comparison operators
        bool operator==(const Vector4D& other) const noexcept;
        bool operator!=(const Vector4D& other) const noexcept;
        
        // Vector operations
        float Length() const noexcept;
        float LengthSquared() const noexcept;
        Vector4D Normalized() const noexcept;
        Vector4D& Normalize() noexcept;
        
        float Dot(const Vector4D& other) const noexcept;
        
        // For homogeneous coordinates
        Vector4D Homogenized() const noexcept;
        Vector4D& Homogenize() noexcept;
        
        // Static methods
        static float Distance(const Vector4D& a, const Vector4D& b) noexcept;
        static float DistanceSquared(const Vector4D& a, const Vector4D& b) noexcept;
        static Vector4D Lerp(const Vector4D& a, const Vector4D& b, float t) noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Vector4D& vec);
        
        // Constants
        static const Vector4D Zero;
        static const Vector4D One;
        static const Vector4D UnitX;
        static const Vector4D UnitY;
        static const Vector4D UnitZ;
        static const Vector4D UnitW;
    };
      inline std::ostream& operator<<(std::ostream& os, const Vector4D& vec)
    {
        os << "(x=" << vec.x << ", y=" << vec.y << ", z=" << vec.z <<", w="<< vec.w <<")";
        return os;
    }
}