#include "Math/Vector4D.hpp"

namespace CEMath
{
    float& Vector4D::operator[](uint32_t index) noexcept
    {
        return (&x)[index];
    }
    
    float Vector4D::operator[](uint32_t index) const noexcept
    {
        return (&x)[index];
    }
    
    Vector4D Vector4D::operator+(const Vector4D& other) const noexcept
    {
        return Vector4D(x + other.x, y + other.y, z + other.z, w + other.w);
    }
    
    Vector4D Vector4D::operator-(const Vector4D& other) const noexcept
    {
        return Vector4D(x - other.x, y - other.y, z - other.z, w - other.w);
    }
    
    Vector4D Vector4D::operator*(const Vector4D& other) const noexcept
    {
        return Vector4D(x * other.x, y * other.y, z * other.z, w * other.w);
    }
    
    Vector4D Vector4D::operator/(const Vector4D& other) const noexcept
    {
        return Vector4D(x / other.x, y / other.y, z / other.z, w / other.w);
    }
    
    Vector4D Vector4D::operator*(float scalar) const noexcept
    {
        return Vector4D(x * scalar, y * scalar, z * scalar, w * scalar);
    }
    
    Vector4D Vector4D::operator/(float scalar) const noexcept
    {
        const float invScalar = 1.0f / scalar;
        return Vector4D(x * invScalar, y * invScalar, z * invScalar, w * invScalar);
    }
    
    Vector4D& Vector4D::operator+=(const Vector4D& other) noexcept
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }
    
    Vector4D& Vector4D::operator-=(const Vector4D& other) noexcept
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }
    
    Vector4D& Vector4D::operator*=(const Vector4D& other) noexcept
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }
    
    Vector4D& Vector4D::operator/=(const Vector4D& other) noexcept
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        return *this;
    }
    
    Vector4D& Vector4D::operator*=(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }
    
    Vector4D& Vector4D::operator/=(float scalar) noexcept
    {
        const float invScalar = 1.0f / scalar;
        x *= invScalar;
        y *= invScalar;
        z *= invScalar;
        w *= invScalar;
        return *this;
    }
    
    Vector4D Vector4D::operator-() const noexcept
    {
        return Vector4D(-x, -y, -z, -w);
    }
    
    bool Vector4D::operator==(const Vector4D& other) const noexcept
    {
        return std::abs(x - other.x) < EPSILON && 
               std::abs(y - other.y) < EPSILON && 
               std::abs(z - other.z) < EPSILON && 
               std::abs(w - other.w) < EPSILON;
    }
    
    bool Vector4D::operator!=(const Vector4D& other) const noexcept
    {
        return !(*this == other);
    }
    
    float Vector4D::Length() const noexcept
    {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }
    
    float Vector4D::LengthSquared() const noexcept
    {
        return x * x + y * y + z * z + w * w;
    }
    
    Vector4D Vector4D::Normalized() const noexcept
    {
        float len = Length();
        if (len > EPSILON)
        {
            const float invLen = 1.0f / len;
            return Vector4D(x * invLen, y * invLen, z * invLen, w * invLen);
        }
        return *this;
    }
    
    Vector4D& Vector4D::Normalize() noexcept
    {
        float len = Length();
        if (len > EPSILON)
        {
            const float invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
            z *= invLen;
            w *= invLen;
        }
        return *this;
    }
    
    float Vector4D::Dot(const Vector4D& other) const noexcept
    {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }
    
    Vector4D Vector4D::Homogenized() const noexcept
    {
        if (std::abs(w) > EPSILON)
        {
            const float invW = 1.0f / w;
            return Vector4D(x * invW, y * invW, z * invW, 1.0f);
        }
        return *this;
    }
    
    Vector4D& Vector4D::Homogenize() noexcept
    {
        if (std::abs(w) > EPSILON)
        {
            const float invW = 1.0f / w;
            x *= invW;
            y *= invW;
            z *= invW;
            w = 1.0f;
        }
        return *this;
    }
    
    float Vector4D::Distance(const Vector4D& a, const Vector4D& b) noexcept
    {
        return (a - b).Length();
    }
    
    float Vector4D::DistanceSquared(const Vector4D& a, const Vector4D& b) noexcept
    {
        return (a - b).LengthSquared();
    }
    
    Vector4D Vector4D::Lerp(const Vector4D& a, const Vector4D& b, float t) noexcept
    {
        return Vector4D(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t
        );
    }
    
    const Vector4D Vector4D::Zero(0.0f, 0.0f, 0.0f, 0.0f);
    const Vector4D Vector4D::One(1.0f, 1.0f, 1.0f, 1.0f);
    const Vector4D Vector4D::UnitX(1.0f, 0.0f, 0.0f, 0.0f);
    const Vector4D Vector4D::UnitY(0.0f, 1.0f, 0.0f, 0.0f);
    const Vector4D Vector4D::UnitZ(0.0f, 0.0f, 1.0f, 0.0f);
    const Vector4D Vector4D::UnitW(0.0f, 0.0f, 0.0f, 1.0f);
}