#pragma once

#include <cmath>
#include <stdexcept>
#include "Utils/Math/CE_MathConstants.h"
#include "Utils/Math/CE_MathHelpers.h"
#include "Vector2D.h"
#include "Vector3D.h"

namespace CEMath
{
    struct Vector4D
    {
        float x{}, y{}, z{}, w{};

        // Конструкторы
        Vector4D();
        Vector4D(float scalar);
        Vector4D(float inX, float inY, float inZ, float inW);
        Vector4D(int inX, int inY, int inZ, int inW);
        Vector4D(const Vector2D& vec2, float z = 0.0f, float w = 0.0f);
        Vector4D(const Vector3D& vec3, float w = 0.0f);
        Vector4D(const Vector4D& other);

        // Операторы присваивания
        Vector4D& operator=(const Vector4D& other);

        // Операторы сравнения
        bool operator==(const Vector4D& other) const;
        bool operator!=(const Vector4D& other) const;

        // Арифметические операторы
        Vector4D operator+(const Vector4D& other) const;
        Vector4D operator-(const Vector4D& other) const;
        Vector4D operator*(const Vector4D& other) const;
        Vector4D operator/(const Vector4D& other) const;

        Vector4D operator+(float scalar) const;
        Vector4D operator-(float scalar) const;
        Vector4D operator*(float scalar) const;
        Vector4D operator/(float scalar) const;

        // Составные операторы присваивания
        Vector4D& operator+=(const Vector4D& other);
        Vector4D& operator-=(const Vector4D& other);
        Vector4D& operator*=(const Vector4D& other);
        Vector4D& operator/=(const Vector4D& other);

        Vector4D& operator+=(float scalar);
        Vector4D& operator-=(float scalar);
        Vector4D& operator*=(float scalar);
        Vector4D& operator/=(float scalar);

        // Унарные операторы
        Vector4D operator-() const;
        Vector4D operator+() const { return *this; }

        // Базовые математические функции
        float Length() const;
        float LengthSquared() const;
        float Distance(const Vector4D& other) const;
        float DistanceSquared(const Vector4D& other) const;
        float Dot(const Vector4D& other) const;

        Vector4D& Normalize();
        Vector4D Normalized() const;
        bool IsNormalized() const;

        // Геометрические операции
        Vector4D& Scale(const Vector4D& other);
        Vector4D Scaled(const Vector4D& other) const;

        // Линейная интерполяция
        static Vector4D Lerp(const Vector4D& a, const Vector4D& b, float t);
        static Vector4D Slerp(const Vector4D& a, const Vector4D& b, float t);

        // Однородные координаты
        Vector3D ToCartesian() const;
        static Vector4D FromCartesian(const Vector3D& cartesian, float w = 1.0f);

        // Утилиты
        bool IsZero() const;
        bool IsFinite() const;

        // Статические константы
        static Vector4D Zero();
        static Vector4D One();
        static Vector4D UnitX();
        static Vector4D UnitY();
        static Vector4D UnitZ();
        static Vector4D UnitW();

        // Цветовые константы
        static Vector4D Red();
        static Vector4D Green();
        static Vector4D Blue();
        static Vector4D Yellow();
        static Vector4D Cyan();
        static Vector4D Magenta();
        static Vector4D White();
        static Vector4D Black();
        static Vector4D Gray();
        static Vector4D Clear(); // Прозрачный (0,0,0,0)

        // Минимум/максимум
        static Vector4D Min(const Vector4D& a, const Vector4D& b);
        static Vector4D Max(const Vector4D& a, const Vector4D& b);

        // Ограничение значений
        Vector4D& Clamp(const Vector4D& min, const Vector4D& max);
        Vector4D& Clamp01(); // Ограничение между 0 и 1 для цветов
        Vector4D& ClampMagnitude(float maxLength);
        static Vector4D Clamp(const Vector4D& value, const Vector4D& min, const Vector4D& max);
        static Vector4D Clamp01(const Vector4D& value);
        static Vector4D ClampMagnitude(const Vector4D& vector, float maxLength);

        // Абсолютные значения и знаки
        Vector4D& Abs();
        Vector4D Abs() const;
        Vector4D& Sign();
        Vector4D Sign() const;

        // Преобразования
        Vector2D ToVector2D() const;
        Vector3D ToVector3D() const;
        Vector3D ToVector3DCartesian() const;

        // Цветовые операции
        Vector4D& Saturate();  // Ограничение значений между 0 и 1
        Vector4D Saturated() const;
        float GetBrightness() const; // Яркость цвета
        Vector4D& Invert(); // Инвертирование цвета
        Vector4D Inverted() const;

        // Компонентные операции
        float MinComponent() const;
        float MaxComponent() const;
        float Sum() const;
        float Average() const;

        // Псевдонимы для координат
        float& Width() { return x; }
        float& Height() { return y; }
        float& Depth() { return z; }
        float& Time() { return w; }
        const float& Width() const { return x; }
        const float& Height() const { return y; }
        const float& Depth() const { return z; }
        const float& Time() const { return w; }

        // Псевдонимы для цветов (RGBA)
        float& R() { return x; }
        float& G() { return y; }
        float& B() { return z; }
        float& A() { return w; }
        const float& R() const { return x; }
        const float& G() const { return y; }
        const float& B() const { return z; }
        const float& A() const { return w; }

        // Псевдонимы для текстур (UVST)
        float& U() { return x; }
        float& V() { return y; }
        float& S() { return z; }
        float& T() { return w; }
        const float& U() const { return x; }
        const float& V() const { return y; }
        const float& S() const { return z; }
        const float& T() const { return w; }
    };
    std::ostream& operator<<(std::ostream& os, const Vector4D& vec);
    std::istream& operator>>(std::istream& is, Vector4D& vec);
}