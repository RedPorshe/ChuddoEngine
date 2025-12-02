#pragma once

#include "Math/MathConstants.hpp"
#include "Math/Vector3D.hpp"
#include "Math/Vector4D.hpp"


#include <cmath>
#include <cstdint>
#include <sstream>  
#include <string> 

namespace CEMath
{
  class Matrix4x4;
  class Quaternion
  {
   public:
    float x, y, z, w;

    // Constructors
    Quaternion() noexcept : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
    {
    }
    Quaternion(float x_, float y_, float z_, float w_) noexcept : x(x_), y(y_), z(z_), w(w_)
    {
    }

    // Create from axis-angle
    static Quaternion FromAxisAngle(const Vector3D& axis, float angle) noexcept;

    // Create from Euler angles (in radians)
    static Quaternion FromEuler(float pitch, float yaw, float roll) noexcept;
    static Quaternion FromEuler(const Vector3D& euler) noexcept;

    // Create from rotation matrix
    static Quaternion FromRotationMatrix(const float mat[4][4]) noexcept;

    // Identity
    static Quaternion Identity() noexcept
    {
      return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
    }

    // Access operators
    float& operator[](uint32_t index) noexcept;
    float operator[](uint32_t index) const noexcept;

    // Arithmetic operators
    Quaternion operator+(const Quaternion& other) const noexcept;
    Quaternion operator-(const Quaternion& other) const noexcept;
    Quaternion operator*(const Quaternion& other) const noexcept;
    
    Vector3D operator*(const Vector3D& other) const noexcept;

    Quaternion operator*(float scalar) const noexcept;
    Quaternion operator/(float scalar) const noexcept;

    // Assignment operators
    Quaternion& operator+=(const Quaternion& other) noexcept;
    Quaternion& operator-=(const Quaternion& other) noexcept;
    Quaternion& operator*=(const Quaternion& other) noexcept;
    Quaternion& operator*=(float scalar) noexcept;
    Quaternion& operator/=(float scalar) noexcept;

    // Unary operators
    Quaternion operator-() const noexcept;

    // Comparison operators
    bool operator==(const Quaternion& other) const noexcept;
    bool operator!=(const Quaternion& other) const noexcept;
    bool IsUnit() const noexcept;

    // Quaternion operations
    float Length() const noexcept;
    float LengthSquared() const noexcept;
    Quaternion Normalized() const noexcept;
    Quaternion& Normalize() noexcept;

    Quaternion Conjugate() const noexcept;
    Quaternion Inverse() const noexcept;

    float Dot(const Quaternion& other) const noexcept;

    // Transformations
    Vector3D Rotate(const Vector3D& vector) const noexcept;
    Quaternion Slerp(const Quaternion& to, float t) const noexcept;

    // Conversion methods
    Vector3D ToEuler() const noexcept;
    void ToAxisAngle(Vector3D& axis, float& angle) const noexcept;
    Matrix4x4 ToMatrix() const noexcept;

    // Static methods
    static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t) noexcept;
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) noexcept;
    static Quaternion LookRotation(const Vector3D& forward, const Vector3D& up = Vector3D::UnitY) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const Quaternion& quat);
    std::string ToStringAngleAxis() const noexcept;
  };
    inline std::ostream& operator<<(std::ostream& os, const Quaternion& quat)
    {
        os << "(x=" << quat.x << ", y=" << quat.y << ", z=" << quat.z << ", w=" << quat.w << ")";
        return os;
    }
}