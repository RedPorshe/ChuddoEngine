#pragma once

#include <cmath>
#include <stdexcept>
#include "Utils/Math/CE_MathConstants.h"
#include "Utils/Math/CE_MathHelpers.h"
#include "Vector2D.h"

namespace CEMath
{
    struct Vector3D
    {
        float x{}, y{}, z{};

        // Конструкторы
        Vector3D();
        Vector3D(float scalar);
        Vector3D(float inX, float inY, float inZ);
        Vector3D(int inX, int inY, int inZ);
        Vector3D(const Vector2D& vec2, float z = 0.0f);
        Vector3D(const Vector3D& other);

        // Операторы присваивания
        Vector3D& operator=(const Vector3D& other);

        // Операторы сравнения
        bool operator==(const Vector3D& other) const;
        bool operator!=(const Vector3D& other) const;

        // Арифметические операторы
        Vector3D operator+(const Vector3D& other) const;
        Vector3D operator-(const Vector3D& other) const;
        Vector3D operator*(const Vector3D& other) const;
        Vector3D operator/(const Vector3D& other) const;

        Vector3D operator+(float scalar) const;
        Vector3D operator-(float scalar) const;
        Vector3D operator*(float scalar) const;
        Vector3D operator/(float scalar) const;

        // Составные операторы присваивания
        Vector3D& operator+=(const Vector3D& other);
        Vector3D& operator-=(const Vector3D& other);
        Vector3D& operator*=(const Vector3D& other);
        Vector3D& operator/=(const Vector3D& other);

        Vector3D& operator+=(float scalar);
        Vector3D& operator-=(float scalar);
        Vector3D& operator*=(float scalar);
        Vector3D& operator/=(float scalar);

        // Унарные операторы
        Vector3D operator-() const;
        Vector3D operator+() const { return *this; }

        // Базовые математические функции
        float Length() const;
        float LengthSquared() const;
        float Distance(const Vector3D& other) const;
        float DistanceSquared(const Vector3D& other) const;
        float Dot(const Vector3D& other) const;
        Vector3D Cross(const Vector3D& other) const;

        Vector3D& Normalize();
        Vector3D Normalized() const;
        bool IsNormalized() const;

        // Геометрические операции
        Vector3D& Scale(const Vector3D& other);
        Vector3D Scaled(const Vector3D& other) const;

        // Проекции и отражение
        Vector3D Project(const Vector3D& normal) const;
        Vector3D ProjectOnPlane(const Vector3D& planeNormal) const;
        Vector3D Reflect(const Vector3D& normal) const;

        // Линейная интерполяция
        static Vector3D Lerp(const Vector3D& a, const Vector3D& b, float t);
        static Vector3D Slerp(const Vector3D& a, const Vector3D& b, float t);

        // Углы и вращения
        static float Angle(const Vector3D& a, const Vector3D& b);
        static Vector3D RotateTowards(const Vector3D& current, const Vector3D& target, float maxRadiansDelta);

        // Утилиты
        bool IsZero() const;
        bool IsFinite() const;

        // Статические константы
        static Vector3D Zero();
        static Vector3D One();
        static Vector3D UnitX();
        static Vector3D UnitY();
        static Vector3D UnitZ();

        // Направления
        static Vector3D Left();
        static Vector3D Right();
        static Vector3D Up();
        static Vector3D Down();
        static Vector3D Forward();
        static Vector3D Back();

        // Минимум/максимум
        static Vector3D Min(const Vector3D& a, const Vector3D& b);
        static Vector3D Max(const Vector3D& a, const Vector3D& b);

        // Ограничение значений
        Vector3D& Clamp(const Vector3D& min, const Vector3D& max);
        Vector3D& ClampMagnitude(float maxLength);
        static Vector3D Clamp(const Vector3D& value, const Vector3D& min, const Vector3D& max);
        static Vector3D ClampMagnitude(const Vector3D& vector, float maxLength);

        // Абсолютные значения и знаки
        Vector3D& Abs();
        Vector3D Abs() const;
        Vector3D& Sign();
        Vector3D Sign() const;

        // Преобразования
        Vector2D ToVector2D() const;
        Vector2D ToVector2DXY() const;
        Vector2D ToVector2DXZ() const;
        Vector2D ToVector2DYZ() const;

        // Компонентные операции
        float MinComponent() const;
        float MaxComponent() const;
        float Sum() const;
        float Average() const;

        // Расстояния до геометрических объектов
        float DistanceToLine(const Vector3D& linePoint, const Vector3D& lineDirection) const;
        float DistanceToPlane(const Vector3D& planePoint, const Vector3D& planeNormal) const;

        // Псевдонимы
        float& Width() { return x; }
        float& Height() { return y; }
        float& Depth() { return z; }
        const float& Width() const { return x; }
        const float& Height() const { return y; }
        const float& Depth() const { return z; }
        bool IsEqual(const Vector3D& other, float epsilon = EPSILON) const;

        float& R() { return x; }
        float& G() { return y; }
        float& B() { return z; }
        const float& R() const { return x; }
        const float& G() const { return y; }
        const float& B() const { return z; }

        float& U() { return x; }
        float& V() { return y; }
        float& W() { return z; }
        const float& U() const { return x; }
        const float& V() const { return y; }
        const float& W() const { return z; }
    };
    std::ostream& operator<<(std::ostream& os, const Vector3D& vec);
    std::istream& operator>>(std::istream& is, Vector3D& vec);
    inline bool IsEqual(const Vector3D& a, const Vector3D& b, float epsilon = EPSILON)
    {
        return a.IsEqual(b, epsilon);
    }
}