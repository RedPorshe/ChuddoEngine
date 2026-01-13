#pragma once

#include <cmath>
#include <stdexcept>
#include <array>
#include <iostream>
#include <iomanip>
#include "Utils/Math/CE_MathConstants.h"
#include "Utils/Math/CE_MathHelpers.h"
#include "Vector3D.h"
#include "Vector4.h"

namespace CEMath
{
    struct Matrix4x4
    {
        // 4x4 матрица, хранится как column-major для OpenGL
        // m[col][row] или m[column * 4 + row]
        float m[16];

        // Конструкторы
        Matrix4x4();
        Matrix4x4(float diagonal);
        Matrix4x4(const float data[16]); // Массив в column-major порядке
        Matrix4x4(const Matrix4x4& other);

        // Конструкторы из отдельных компонентов (column-major)
        Matrix4x4(float m00, float m10, float m20, float m30,
            float m01, float m11, float m21, float m31,
            float m02, float m12, float m22, float m32,
            float m03, float m13, float m23, float m33);

        // Операторы присваивания
        Matrix4x4& operator=(const Matrix4x4& other);

        // Операторы доступа
        float& operator()(int row, int col);
        float operator()(int row, int col) const;

        float& operator[](int index);
        float operator[](int index) const;

        // Операторы сравнения
        bool operator==(const Matrix4x4& other) const;
        bool operator!=(const Matrix4x4& other) const;

        // Арифметические операторы
        Matrix4x4 operator+(const Matrix4x4& other) const;
        Matrix4x4 operator-(const Matrix4x4& other) const;
        Matrix4x4 operator*(const Matrix4x4& other) const;
        Matrix4x4 operator*(float scalar) const;
        Matrix4x4 operator/(float scalar) const;

        Vector4D operator*(const Vector4D& vec) const;
        Vector3D operator*(const Vector3D& vec) const;

        // Составные операторы присваивания
        Matrix4x4& operator+=(const Matrix4x4& other);
        Matrix4x4& operator-=(const Matrix4x4& other);
        Matrix4x4& operator*=(const Matrix4x4& other);
        Matrix4x4& operator*=(float scalar);
        Matrix4x4& operator/=(float scalar);

        // Унарные операторы
        Matrix4x4 operator-() const;

        // Базовые математические функции
        Matrix4x4 Transposed() const;
        Matrix4x4& Transpose();

        float Determinant() const;
        Matrix4x4 Inversed() const;
        Matrix4x4& Inverse();

        Matrix4x4& Identity();
        bool IsIdentity() const;

        // Проверки
        bool IsInvertible() const;
        bool IsOrthogonal() const;
        bool IsAffine() const;

        // Извлечение компонентов
        Vector3D GetTranslation() const;
        Vector3D GetScale() const;
        Matrix4x4 GetRotation() const;

        void SetTranslation(const Vector3D& translation);
        void SetScale(const Vector3D& scale);

        // Преобразования
        Matrix4x4& Translate(const Vector3D& translation);
        Matrix4x4& Translate(float x, float y, float z);

        Matrix4x4& Scale(const Vector3D& scale);
        Matrix4x4& Scale(float x, float y, float z);
        Matrix4x4& Scale(float uniformScale);

        Matrix4x4& RotateX(float radians);
        Matrix4x4& RotateY(float radians);
        Matrix4x4& RotateZ(float radians);
        Matrix4x4& Rotate(const Vector3D& axis, float radians);

        Matrix4x4& LookAt(const Vector3D& eye, const Vector3D& target, const Vector3D& up);
        Matrix4x4& Perspective(float fovY, float aspect, float zNear, float zFar);
        Matrix4x4& Orthographic(float left, float right, float bottom, float top,
            float zNear, float zFar);

        // Статические фабричные методы
        static Matrix4x4 Zero();
        static Matrix4x4 IdentityMatrix();

        static Matrix4x4 Translation(const Vector3D& translation);
        static Matrix4x4 Translation(float x, float y, float z);

        static Matrix4x4 Scaling(const Vector3D& scale);
        static Matrix4x4 Scaling(float x, float y, float z);
        static Matrix4x4 Scaling(float uniformScale);

        static Matrix4x4 RotationX(float radians);
        static Matrix4x4 RotationY(float radians);
        static Matrix4x4 RotationZ(float radians);
        static Matrix4x4 Rotation(const Vector3D& axis, float radians);

        static Matrix4x4 LookAtMatrix(const Vector3D& eye, const Vector3D& target, const Vector3D& up);
        static Matrix4x4 PerspectiveMatrix(float fovY, float aspect, float zNear, float zFar);
        static Matrix4x4 OrthographicMatrix(float left, float right, float bottom, float top,
            float zNear, float zFar);

        // Утилиты
        void ToArray(float out[16]) const;
        std::array<float, 16> ToStdArray() const;

       friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& mat);
       friend std::istream& operator>>(std::istream& is, Matrix4x4& mat);
    };

    // Внешние операторы
    Vector4D operator*(const Vector4D& vec, const Matrix4x4& mat);
    Vector3D operator*(const Vector3D& vec, const Matrix4x4& mat);
    Matrix4x4 operator*(float scalar, const Matrix4x4& mat);
}