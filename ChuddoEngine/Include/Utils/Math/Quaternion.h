#pragma once

#include <cmath>
#include <stdexcept>
#include "Utils/Math/CE_MathConstants.h"
#include "Utils/Math/CE_MathHelpers.h"
#include "Vector3D.h"
#include "Vector4.h"
#include "Matrix4.h"

namespace CEMath
{
    struct Quaternion
    {
        float x, y, z, w;

        // Конструкторы
        Quaternion();
        Quaternion(float x, float y, float z, float w);
        Quaternion(const Vector3D& axis, float radians);
        Quaternion(float pitch, float yaw, float roll); // Euler angles (XYZ order)
        Quaternion(const Quaternion& other);

        // Конструкторы из матрицы
        explicit Quaternion(const Matrix4x4& rotationMatrix);

        // Операторы присваивания
        Quaternion& operator=(const Quaternion& other);

        // Операторы сравнения
        bool operator==(const Quaternion& other) const;
        bool operator!=(const Quaternion& other) const;

        // Арифметические операторы
        Quaternion operator+(const Quaternion& other) const;
        Quaternion operator-(const Quaternion& other) const;
        Quaternion operator*(const Quaternion& other) const;
        Quaternion operator*(float scalar) const;
        Quaternion operator/(float scalar) const;

        // Умножение на вектор (вращение вектора)
        Vector3D operator*(const Vector3D& vec) const;

        // Составные операторы присваивания
        Quaternion& operator+=(const Quaternion& other);
        Quaternion& operator-=(const Quaternion& other);
        Quaternion& operator*=(const Quaternion& other);
        Quaternion& operator*=(float scalar);
        Quaternion& operator/=(float scalar);

        // Унарные операторы
        Quaternion operator-() const;
        Quaternion operator+() const { return *this; }

        // Базовые математические функции
        float Length() const;
        float LengthSquared() const;
        float Dot(const Quaternion& other) const;

        Quaternion& Normalize();
        Quaternion Normalized() const;
        Quaternion& Conjugate();
        Quaternion Conjugated() const;
        Quaternion& Inverse();
        Quaternion Inversed() const;

        // Проверки
        bool IsUnit() const;
        bool IsIdentity() const;
        bool IsZero() const;

        // Получение информации
        float GetAngle() const;
        Vector3D GetAxis() const;
        Vector3D GetEulerAngles() const; // В углах Эйлера (XYZ порядок)

        // Преобразования
        Matrix4x4 ToMatrix() const;
        Vector4D ToVector4() const;

        // Операции вращения
        Quaternion& Rotate(const Quaternion& rotation);
        Quaternion& Rotate(const Vector3D& axis, float radians);
        Quaternion& RotateX(float radians);
        Quaternion& RotateY(float radians);
        Quaternion& RotateZ(float radians);

        // Линейная интерполяция
        static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t);
        static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
        static Quaternion Nlerp(const Quaternion& a, const Quaternion& b, float t);

        bool IsEqual(const Quaternion& other, float epsilon = EPSILON) const;

        // Сферическая интерполяция с кратчайшим путем
        static Quaternion SlerpShortestPath(const Quaternion& a, const Quaternion& b, float t);

        // Вращение к цели
        static Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, float maxRadiansDelta);

        // Утилиты
        static Quaternion Identity();
        static Quaternion Zero();

        // Создание из углов Эйлера
        static Quaternion FromEulerAngles(float pitch, float yaw, float roll);
        static Quaternion FromEulerAngles(const Vector3D& eulerAngles);

        // Создание из осей (правая, верхняя, вперед)
        static Quaternion FromAxes(const Vector3D& right, const Vector3D& up, const Vector3D& forward);

        // Создание из матрицы поворота
        static Quaternion FromRotationMatrix(const Matrix4x4& rotationMatrix);

        // Создание из направления взгляда
        static Quaternion LookRotation(const Vector3D& forward, const Vector3D& up = Vector3D::Up());

        // Псевдонимы
        float& R() { return x; }
        float& I() { return x; }
        float& J() { return y; }
        float& K() { return z; }
        float& Real() { return w; }

        const float& R() const { return x; }
        const float& I() const { return x; }
        const float& J() const { return y; }
        const float& K() const { return z; }
        const float& Real() const { return w; }
    };

    std::ostream& operator<<(std::ostream& os, const Quaternion& q);
    std::istream& operator>>(std::istream& is, Quaternion& q);

    // Внешние операторы
    Quaternion operator*(float scalar, const Quaternion& q);
    Vector3D operator*(const Vector3D& vec, const Quaternion& q);

    inline bool IsEqual(const Quaternion& a, const Quaternion& b, float epsilon = EPSILON)
    {
        return a.IsEqual(b, epsilon);
    }
}