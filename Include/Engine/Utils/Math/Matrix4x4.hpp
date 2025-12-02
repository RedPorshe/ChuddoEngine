#pragma once

#include "Math/MathConstants.hpp"
#include "Math/Vector3D.hpp"
#include "Math/Vector4D.hpp"


#include <cmath>
#include <cstdint>
#include <iostream>
#include <iomanip>

namespace CEMath
{
    class Quaternion;

    class Matrix4x4
    {
    public:
        float m[4][4];
        
        // Constructors
        Matrix4x4() noexcept;
        explicit Matrix4x4(float diagonal) noexcept;
        
 Matrix4x4(float m00, float m01, float m02, float m03,
                  float m10, float m11, float m12, float m13,
                  float m20, float m21, float m22, float m23,
                  float m30, float m31, float m32, float m33) noexcept;

        // Access operators
        float* operator[](uint32_t index) noexcept;
        const float* operator[](uint32_t index) const noexcept;
        
        // Arithmetic operators
        Matrix4x4 operator+(const Matrix4x4& other) const noexcept;
        Matrix4x4 operator-(const Matrix4x4& other) const noexcept;
        Matrix4x4 operator*(const Matrix4x4& other) const noexcept;
        Matrix4x4 operator*(float scalar) const noexcept;
        Matrix4x4 operator/(float scalar) const noexcept;
        
        Vector4D operator*(const Vector4D& vector) const noexcept;
        Vector3D operator*(const Vector3D& vector) const noexcept;
        
        // Assignment operators
        Matrix4x4& operator+=(const Matrix4x4& other) noexcept;
        Matrix4x4& operator-=(const Matrix4x4& other) noexcept;
        Matrix4x4& operator*=(const Matrix4x4& other) noexcept;
        Matrix4x4& operator*=(float scalar) noexcept;
        Matrix4x4& operator/=(float scalar) noexcept;
        
        // Comparison operators
        bool operator==(const Matrix4x4& other) const noexcept;
        bool operator!=(const Matrix4x4& other) const noexcept;
        
        // Matrix operations
        Matrix4x4 Transposed() const noexcept;
        Matrix4x4& Transpose() noexcept;

        bool IsOrthogonal() const;

                float Determinant() const noexcept;
        Matrix4x4 Inversed() const noexcept;
        Matrix4x4& Inverse() noexcept;
        
        // Transformation matrices
        static Matrix4x4 Translate(const Vector3D& translation) noexcept;
        static Matrix4x4 Scale(const Vector3D& scale) noexcept;
        static Matrix4x4 RotateX(float angle) noexcept;
        static Matrix4x4 RotateY(float angle) noexcept;
        static Matrix4x4 RotateZ(float angle) noexcept;
        static Matrix4x4 Rotate(const Quaternion& rotation) noexcept;
        
        // View and projection matrices (for Vulkan)
        static Matrix4x4 LookAt(const Vector3D& eye, const Vector3D& center, const Vector3D& up) noexcept;
        static Matrix4x4 Perspective(float fovY, float aspect, float zNear, float zFar) noexcept;
        static Matrix4x4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar) noexcept;
        
        // For Vulkan (Y-axis down, Z-axis [0, 1])
        static Matrix4x4 VulkanPerspective(float fovY, float aspect, float zNear, float zFar) noexcept;
        static Matrix4x4 VulkanOrthographic(float left, float right, float bottom, float top, float zNear, float zFar) noexcept;
        
         friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& mat);
         std::string ToString() const;
         std::string ToStringCompact() const;
         std::string ToStringGLSL() const;
         // Constants
         static const Matrix4x4 Identity;
         static const Matrix4x4 Zero;
    };
    
     inline std::ostream& operator<<(std::ostream& os, const Matrix4x4& mat)
    {
        os << "Matrix4x4:\n";
        for (int i = 0; i < 4; ++i)
        {
            os << "  [ ";
            for (int j = 0; j < 4; ++j)
            {
                os << std::fixed << std::setprecision(6) << std::setw(10) << mat.m[i][j];
                if (j < 3) os << ", ";
            }
            os << " ]\n";
        }
        return os;
    }
}