#include "Utils/Math/Vector3D.h"
#include <iostream>
#include <iomanip>

namespace CEMath
{
    Vector3D::Vector3D() : x(0.f), y(0.f), z(0.f) {}

    Vector3D::Vector3D(float scalar) : x(scalar), y(scalar), z(scalar) {}

    Vector3D::Vector3D(float inX, float inY, float inZ) :
        x(inX), y(inY), z(inZ) {
    }

    Vector3D::Vector3D(int inX, int inY, int inZ) :
        x(static_cast<float>(inX)), y(static_cast<float>(inY)), z(static_cast<float>(inZ)) {
    }

    Vector3D::Vector3D(const Vector2D& vec2, float z) :
        x(vec2.x), y(vec2.y), z(z) {
    }

    Vector3D::Vector3D(const Vector3D& other) :
        x(other.x), y(other.y), z(other.z) {
    }

    Vector3D& Vector3D::operator=(const Vector3D& other)
    {
        if (this != &other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }

    bool Vector3D::operator==(const Vector3D& other) const
    {
        return IsEqual(x, other.x) && IsEqual(y, other.y) && IsEqual(z, other.z);
    }

    bool Vector3D::operator!=(const Vector3D& other) const
    {
        return !(*this == other);
    }

    Vector3D Vector3D::operator+(const Vector3D& other) const
    {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }

    Vector3D Vector3D::operator-(const Vector3D& other) const
    {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }

    Vector3D Vector3D::operator*(const Vector3D& other) const
    {
        return Vector3D(x * other.x, y * other.y, z * other.z);
    }

    Vector3D Vector3D::operator/(const Vector3D& other) const
    {
        if (CEMath::IsZero(other.x) || CEMath::IsZero(other.y) || CEMath::IsZero(other.z))
        {
            throw std::runtime_error("Vector3D: Division by zero");
        }
        return Vector3D(x / other.x, y / other.y, z / other.z);
    }

    Vector3D Vector3D::operator+(float scalar) const
    {
        return Vector3D(x + scalar, y + scalar, z + scalar);
    }

    Vector3D Vector3D::operator-(float scalar) const
    {
        return Vector3D(x - scalar, y - scalar, z - scalar);
    }

    Vector3D Vector3D::operator*(float scalar) const
    {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D Vector3D::operator/(float scalar) const
    {
        if (CEMath::IsZero(scalar))
        {
            throw std::runtime_error("Vector3D: Division by zero");
        }
        float invScalar = 1.0f / scalar;
        return Vector3D(x * invScalar, y * invScalar, z * invScalar);
    }

    Vector3D& Vector3D::operator+=(const Vector3D& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3D& Vector3D::operator-=(const Vector3D& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vector3D& Vector3D::operator*=(const Vector3D& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    Vector3D& Vector3D::operator/=(const Vector3D& other)
    {
        if (CEMath::IsZero(other.x) || CEMath::IsZero(other.y) || CEMath::IsZero(other.z))
        {
            throw std::runtime_error("Vector3D: Division by zero");
        }
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    Vector3D& Vector3D::operator+=(float scalar)
    {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }

    Vector3D& Vector3D::operator-=(float scalar)
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }

    Vector3D& Vector3D::operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vector3D& Vector3D::operator/=(float scalar)
    {
        if (CEMath::IsZero(scalar))
        {
            throw std::runtime_error("Vector3D: Division by zero");
        }
        float invScalar = 1.0f / scalar;
        x *= invScalar;
        y *= invScalar;
        z *= invScalar;
        return *this;
    }

    Vector3D Vector3D::operator-() const
    {
        return Vector3D(-x, -y, -z);
    }

    // Базовые математические функции
    float Vector3D::Length() const
    {
        return Sqrt(x * x + y * y + z * z);
    }

    float Vector3D::LengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    float Vector3D::Distance(const Vector3D& other) const
    {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return Sqrt(dx * dx + dy * dy + dz * dz);
    }

    float Vector3D::DistanceSquared(const Vector3D& other) const
    {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return dx * dx + dy * dy + dz * dz;
    }

    float Vector3D::Dot(const Vector3D& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3D Vector3D::Cross(const Vector3D& other) const
    {
        return Vector3D(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    Vector3D& Vector3D::Normalize()
    {
        float len = Length();
        if (CEMath::IsZero(len))
        {
            throw std::runtime_error("Vector3D: Cannot normalize zero vector");
        }
        float invLen = 1.0f / len;
        x *= invLen;
        y *= invLen;
        z *= invLen;
        return *this;
    }

    Vector3D Vector3D::Normalized() const
    {
        Vector3D result = *this;
        result.Normalize();
        return result;
    }

    bool Vector3D::IsNormalized() const
    {
        return CEMath::IsEqual(LengthSquared(), 1.0f);
    }

    // Геометрические операции
    Vector3D& Vector3D::Scale(const Vector3D& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    Vector3D Vector3D::Scaled(const Vector3D& other) const
    {
        Vector3D result = *this;
        result.Scale(other);
        return result;
    }

    // Проекции и отражение
    Vector3D Vector3D::Project(const Vector3D& normal) const
    {
        float dot = Dot(normal);
        float lenSq = normal.LengthSquared();
        if (CEMath::IsZero(lenSq))
            return Zero();
        return normal * (dot / lenSq);
    }

    Vector3D Vector3D::ProjectOnPlane(const Vector3D& planeNormal) const
    {
        return *this - Project(planeNormal);
    }

    Vector3D Vector3D::Reflect(const Vector3D& normal) const
    {
        float dot = Dot(normal);
        return *this - normal * (2.0f * dot);
    }

    // Линейная интерполяция
    Vector3D Vector3D::Lerp(const Vector3D& a, const Vector3D& b, float t)
    {
        float clampedT = CEMath::Clamp(t, 0.0f, 1.0f);
        return Vector3D(
            a.x + (b.x - a.x) * clampedT,
            a.y + (b.y - a.y) * clampedT,
            a.z + (b.z - a.z) * clampedT
        );
    }

    Vector3D Vector3D::Slerp(const Vector3D& a, const Vector3D& b, float t)
    {
        float clampedT = CEMath::Clamp(t, 0.0f, 1.0f);
        float dot = CEMath::Clamp(a.Dot(b), -1.0f, 1.0f);
        float theta = Acos(dot) * clampedT;

        Vector3D relativeVec = b - a * dot;
        if (relativeVec.LengthSquared() > EPSILON_SQUARED)
            relativeVec.Normalize();
        else
            relativeVec = a.Cross(Vector3D::UnitY()).Normalized();

        return a * Cos(theta) + relativeVec * Sin(theta);
    }

    // Углы
    float Vector3D::Angle(const Vector3D& a, const Vector3D& b)
    {
        float dot = CEMath::Clamp(a.Dot(b), -1.0f, 1.0f);
        return Acos(dot);
    }

    Vector3D Vector3D::RotateTowards(const Vector3D& current, const Vector3D& target, float maxRadiansDelta)
    {
        float angle = Angle(current, target);
        if (angle < maxRadiansDelta)
            return target;

        float t = maxRadiansDelta / angle;
        return Lerp(current, target, t);
    }

    // Утилиты
    bool Vector3D::IsZero() const
    {
        return LengthSquared() < EPSILON_SQUARED;
    }

    bool Vector3D::IsFinite() const
    {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }

    // Статические константы
    Vector3D Vector3D::Zero() { return Vector3D(0.0f, 0.0f, 0.0f); }
    Vector3D Vector3D::One() { return Vector3D(1.0f, 1.0f, 1.0f); }
    Vector3D Vector3D::UnitX() { return Vector3D(1.0f, 0.0f, 0.0f); }
    Vector3D Vector3D::UnitY() { return Vector3D(0.0f, 1.0f, 0.0f); }
    Vector3D Vector3D::UnitZ() { return Vector3D(0.0f, 0.0f, 1.0f); }

    // Направления
    Vector3D Vector3D::Left() { return Vector3D(-1.0f, 0.0f, 0.0f); }
    Vector3D Vector3D::Right() { return Vector3D(1.0f, 0.0f, 0.0f); }
    Vector3D Vector3D::Up() { return Vector3D(0.0f, 1.0f, 0.0f); }
    Vector3D Vector3D::Down() { return Vector3D(0.0f, -1.0f, 0.0f); }
    Vector3D Vector3D::Forward() { return Vector3D(0.0f, 0.0f, 1.0f); }
    Vector3D Vector3D::Back() { return Vector3D(0.0f, 0.0f, -1.0f); }

    // Минимум/максимум
    Vector3D Vector3D::Min(const Vector3D& a, const Vector3D& b)
    {
        return Vector3D(
            std::min(a.x, b.x),
            std::min(a.y, b.y),
            std::min(a.z, b.z)
        );
    }

    Vector3D Vector3D::Max(const Vector3D& a, const Vector3D& b)
    {
        return Vector3D(
            std::max(a.x, b.x),
            std::max(a.y, b.y),
            std::max(a.z, b.z)
        );
    }

    // Ограничение значений
    Vector3D& Vector3D::Clamp(const Vector3D& min, const Vector3D& max)
    {
        x = CEMath::Clamp(x, min.x, max.x);
        y = CEMath::Clamp(y, min.y, max.y);
        z = CEMath::Clamp(z, min.z, max.z);
        return *this;
    }

    Vector3D& Vector3D::ClampMagnitude(float maxLength)
    {
        float lenSq = LengthSquared();
        if (lenSq > maxLength * maxLength)
        {
            float len = Sqrt(lenSq);
            float scale = maxLength / len;
            x *= scale;
            y *= scale;
            z *= scale;
        }
        return *this;
    }

    Vector3D Vector3D::Clamp(const Vector3D& value, const Vector3D& min, const Vector3D& max)
    {
        Vector3D result = value;
        result.Clamp(min, max);
        return result;
    }

    Vector3D Vector3D::ClampMagnitude(const Vector3D& vector, float maxLength)
    {
        Vector3D result = vector;
        result.ClampMagnitude(maxLength);
        return result;
    }

    // Абсолютные значения и знаки
    Vector3D& Vector3D::Abs()
    {
        x = CEMath::Abs(x);
        y = CEMath::Abs(y);
        z = CEMath::Abs(z);
        return *this;
    }

    Vector3D Vector3D::Abs() const
    {
        Vector3D result = *this;
        result.Abs();
        return result;
    }

    Vector3D& Vector3D::Sign()
    {
        x = CEMath::Sign(x);
        y = CEMath::Sign(y);
        z = CEMath::Sign(z);
        return *this;
    }

    Vector3D Vector3D::Sign() const
    {
        Vector3D result = *this;
        result.Sign();
        return result;
    }

    // Преобразования
    Vector2D Vector3D::ToVector2D() const
    {
        return Vector2D(x, y);
    }

    Vector2D Vector3D::ToVector2DXY() const
    {
        return Vector2D(x, y);
    }

    Vector2D Vector3D::ToVector2DXZ() const
    {
        return Vector2D(x, z);
    }

    Vector2D Vector3D::ToVector2DYZ() const
    {
        return Vector2D(y, z);
    }

    // Компонентные операции
    float Vector3D::MinComponent() const { return std::min(x, std::min(y, z)); }
    float Vector3D::MaxComponent() const { return std::max(x, std::max(y, z)); }
    float Vector3D::Sum() const { return x + y + z; }
    float Vector3D::Average() const { return (x + y + z) / 3.0f; }

    // Расстояния до геометрических объектов
    float Vector3D::DistanceToLine(const Vector3D& linePoint, const Vector3D& lineDirection) const
    {
        Vector3D pointToLineStart = *this - linePoint;
        Vector3D projection = pointToLineStart.Project(lineDirection);
        return (pointToLineStart - projection).Length();
    }

    float Vector3D::DistanceToPlane(const Vector3D& planePoint, const Vector3D& planeNormal) const
    {
        Vector3D normalizedNormal = planeNormal.Normalized();
        Vector3D pointToPlane = *this - planePoint;
        return CEMath::Abs(pointToPlane.Dot(normalizedNormal));
    }

    bool Vector3D::IsEqual(const Vector3D& other, float epsilon) const
    {
        return std::fabs(x - other.x) < epsilon &&
            std::fabs(y - other.y) < epsilon &&
            std::fabs(z - other.z) < epsilon;
    }

    std::ostream& operator<<(std::ostream& os, const Vector3D& vec)
    {
        os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }

    std::istream& operator>>(std::istream& is, Vector3D& vec)
    {
        char open, comma1, comma2, close;
        is >> open >> vec.x >> comma1 >> vec.y >> comma2 >> vec.z >> close;

        // Если формат не "(x, y, z)", попробуем прочитать просто три числа
        if (!is || (open != '(' && open != '[') || comma1 != ',' || comma2 != ',' ||
            (close != ')' && close != ']'))
        {
            is.clear();
            is.seekg(-static_cast<int>(is.tellg()), std::ios_base::cur);
            is >> vec.x >> vec.y >> vec.z;
        }

        return is;
    }
}