#include "Math/Vector2D.hpp"

namespace CEMath
{
    float& Vector2D::operator[](uint32_t index) noexcept
    {
        return (&x)[index];
    }
    
    float Vector2D::operator[](uint32_t index) const noexcept
    {
        return (&x)[index];
    }
    
    Vector2D Vector2D::operator+(const Vector2D& other) const noexcept
    {
        return Vector2D(x + other.x, y + other.y);
    }
    
    Vector2D Vector2D::operator-(const Vector2D& other) const noexcept
    {
        return Vector2D(x - other.x, y - other.y);
    }
    
    Vector2D Vector2D::operator*(const Vector2D& other) const noexcept
    {
        return Vector2D(x * other.x, y * other.y);
    }
    
    Vector2D Vector2D::operator/(const Vector2D& other) const noexcept
    {
        return Vector2D(x / other.x, y / other.y);
    }
    
    Vector2D Vector2D::operator*(float scalar) const noexcept
    {
        return Vector2D(x * scalar, y * scalar);
    }
    
    Vector2D Vector2D::operator/(float scalar) const noexcept
    {
        const float invScalar = 1.0f / scalar;
        return Vector2D(x * invScalar, y * invScalar);
    }
    
    Vector2D& Vector2D::operator+=(const Vector2D& other) noexcept
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    Vector2D& Vector2D::operator-=(const Vector2D& other) noexcept
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    Vector2D& Vector2D::operator*=(const Vector2D& other) noexcept
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    
    Vector2D& Vector2D::operator/=(const Vector2D& other) noexcept
    {
        x /= other.x;
        y /= other.y;
        return *this;
    }
    
    Vector2D& Vector2D::operator*=(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    
    Vector2D& Vector2D::operator/=(float scalar) noexcept
    {
        const float invScalar = 1.0f / scalar;
        x *= invScalar;
        y *= invScalar;
        return *this;
    }
    
    Vector2D Vector2D::operator-() const noexcept
    {
        return Vector2D(-x, -y);
    }
    
    bool Vector2D::operator==(const Vector2D& other) const noexcept
    {
        return std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON;
    }
    
    bool Vector2D::operator!=(const Vector2D& other) const noexcept
    {
        return !(*this == other);
    }
    
    float Vector2D::Length() const noexcept
    {
        return std::sqrt(x * x + y * y);
    }
    
    float Vector2D::LengthSquared() const noexcept
    {
        return x * x + y * y;
    }
    
    Vector2D Vector2D::Normalized() const noexcept
    {
        float len = Length();
        if (len > EPSILON)
        {
            const float invLen = 1.0f / len;
            return Vector2D(x * invLen, y * invLen);
        }
        return *this;
    }
    
    Vector2D& Vector2D::Normalize() noexcept
    {
        float len = Length();
        if (len > EPSILON)
        {
            const float invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
        }
        return *this;
    }
    
    float Vector2D::Dot(const Vector2D& other) const noexcept
    {
        return x * other.x + y * other.y;
    }
    
    float Vector2D::Cross(const Vector2D& other) const noexcept
    {
        return x * other.y - y * other.x;
    }
    
    float Vector2D::Distance(const Vector2D& a, const Vector2D& b) noexcept
    {
        return (a - b).Length();
    }
    
    float Vector2D::DistanceSquared(const Vector2D& a, const Vector2D& b) noexcept
    {
        return (a - b).LengthSquared();
    }
    
    Vector2D Vector2D::Lerp(const Vector2D& a, const Vector2D& b, float t) noexcept
    {
        return Vector2D(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t
        );
    }
    
    Vector2D Vector2D::Reflect(const Vector2D& direction, const Vector2D& normal) noexcept
    {
        return direction - normal * (2.0f * direction.Dot(normal));
    }
    
    const Vector2D Vector2D::Zero(0.0f, 0.0f);
    const Vector2D Vector2D::One(1.0f, 1.0f);
    const Vector2D Vector2D::UnitX(1.0f, 0.0f);
    const Vector2D Vector2D::UnitY(0.0f, 1.0f);
}