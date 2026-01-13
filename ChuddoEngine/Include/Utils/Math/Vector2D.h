#pragma once
#include "Utils/Math/CE_MathConstants.h"
#include "Utils/Math/CE_MathHelpers.h"
#include <stdexcept>

namespace CEMath
{
    struct Vector2D
    {
        float x{}, y{};

        // Конструкторы
        Vector2D();
        Vector2D(float scalar);
        Vector2D(float inX, float inY);
        Vector2D(int inX, int inY);
        Vector2D(const Vector2D& other);

        // Операторы присваивания
        Vector2D& operator=(const Vector2D& other);

        // Операторы сравнения
        bool operator==(const Vector2D& other) const;
        bool operator!=(const Vector2D& other) const;

        // Арифметические операторы
        Vector2D operator+(const Vector2D& other) const;
        Vector2D operator-(const Vector2D& other) const;
        Vector2D operator*(const Vector2D& other) const;
        Vector2D operator/(const Vector2D& other) const;

        Vector2D operator+(float scalar) const;
        Vector2D operator-(float scalar) const;
        Vector2D operator*(float scalar) const;
        Vector2D operator/(float scalar) const;

        // Составные операторы присваивания
        Vector2D& operator+=(const Vector2D& other);
        Vector2D& operator-=(const Vector2D& other);
        Vector2D& operator*=(const Vector2D& other);
        Vector2D& operator/=(const Vector2D& other);

        Vector2D& operator+=(float scalar);
        Vector2D& operator-=(float scalar);
        Vector2D& operator*=(float scalar);
        Vector2D& operator/=(float scalar);

        // Унарные операторы
        Vector2D operator-() const;
        Vector2D operator+() const { return *this; }

        // Инкремент/декремент
        Vector2D& operator++();
        Vector2D operator++(int);
        Vector2D& operator--();
        Vector2D operator--(int);

        // Базовые математические функции
        float Length() const;
        float LengthSquared() const;
        float Distance(const Vector2D& other) const;
        float DistanceSquared(const Vector2D& other) const;
        float Dot(const Vector2D& other) const;
        float Cross(const Vector2D& other) const;

        Vector2D& Normalize();
        Vector2D Normalized() const;
        bool IsNormalized() const;

        // Геометрические функции для игр
        Vector2D& Rotate(float radians);
        Vector2D Rotated(float radians) const;
        float Angle() const;  // Угол относительно оси X
        static float Angle(const Vector2D& from, const Vector2D& to);
        Vector2D& RotateTowards(const Vector2D& target, float maxRadiansDelta);

        // Отражение и проекция (для физики)
        Vector2D Reflect(const Vector2D& normal) const;
        Vector2D Project(const Vector2D& normal) const;

        // Линейная интерполяция (критически важно!)
        static Vector2D Lerp(const Vector2D& a, const Vector2D& b, float t);
        static Vector2D LerpUnclamped(const Vector2D& a, const Vector2D& b, float t);
        static Vector2D Slerp(const Vector2D& a, const Vector2D& b, float t); // Сферическая

        // Ограничение и обрезка
        Vector2D& Clamp(const Vector2D& min, const Vector2D& max);
        Vector2D& ClampMagnitude(float maxLength);
        static Vector2D Clamp(const Vector2D& value, const Vector2D& min, const Vector2D& max);
        static Vector2D ClampMagnitude(const Vector2D& vector, float maxLength);

        // Минимум/максимум
        static Vector2D Min(const Vector2D& a, const Vector2D& b);
        static Vector2D Max(const Vector2D& a, const Vector2D& b);
        Vector2D& Abs();
        Vector2D Abs() const;

        // Масштабирование
        Vector2D& Scale(const Vector2D& other);
        Vector2D& Scale(float scalar);
        Vector2D Scaled(const Vector2D& other) const;
        Vector2D Scaled(float scalar) const;

        // Утилиты
        bool IsZero() const;
        bool IsFinite() const;  // Проверка на NaN/Infinity

        // Статические константы
        static Vector2D Zero();
        static Vector2D One();
        static Vector2D UnitX();
        static Vector2D UnitY();
        static Vector2D Left();
        static Vector2D Right();
        static Vector2D Up();
        static Vector2D Down();
        static Vector2D UpLeft();
        static Vector2D UpRight();
        static Vector2D DownLeft();
        static Vector2D DownRight();

        // Вспомогательные функции
        float MinComponent() const;
        float MaxComponent() const;
        float Sum() const;
        float Average() const;

        // Перпендикулярные векторы
        Vector2D PerpendicularClockwise() const;
        Vector2D PerpendicularCounterClockwise() const;

        // Расстояние до линии/сегмента (для AI и коллизий)
        float DistanceToLine(const Vector2D& lineStart, const Vector2D& lineEnd) const;
        float DistanceToSegment(const Vector2D& segmentStart, const Vector2D& segmentEnd) const;

        // Преобразования
        Vector2D& Floor();
        Vector2D Floor() const;
        Vector2D& Ceil();
        Vector2D Ceil() const;
        Vector2D& Round();
        Vector2D Round() const;
        Vector2D& Sign();
        Vector2D Sign() const;
    };
    std::ostream& operator<<(std::ostream& os, const Vector2D& vec);
    std::istream& operator>>(std::istream& is, Vector2D& vec);
}