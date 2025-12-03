#include "Math/Vector3D.hpp"
#include "Math/Quaternion.hpp"


namespace CEMath
{
    float& Vector3D::operator[](uint32_t index) noexcept
    {
        return (&x)[index];
    }
    
    float Vector3D::operator[](uint32_t index) const noexcept
    {
        return (&x)[index];
    }
    
    Vector3D Vector3D::operator+(const Vector3D& other) const noexcept
    {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }
    
    Vector3D Vector3D::operator-(const Vector3D& other) const noexcept
    {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }
    
    Vector3D Vector3D::operator*(const Vector3D& other) const noexcept
    {
        return Vector3D(x * other.x, y * other.y, z * other.z);
    }
    Vector3D Vector3D::operator/(const Vector3D& other) const noexcept
    {
        return Vector3D(x / other.x, y / other.y, z / other.z);
    }
    
    Vector3D Vector3D::operator*(float scalar) const noexcept
    {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3D Vector3D::operator/(float scalar) const noexcept
    {
        const float invScalar = 1.0f / scalar;
        return Vector3D(x * invScalar, y * invScalar, z * invScalar);
    }
    
    Vector3D& Vector3D::operator+=(const Vector3D& other) noexcept
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    
    Vector3D& Vector3D::operator-=(const Vector3D& other) noexcept
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    
    Vector3D& Vector3D::operator*=(const Vector3D& other) noexcept
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }
    
    Vector3D& Vector3D::operator/=(const Vector3D& other) noexcept
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }
    
    Vector3D& Vector3D::operator*=(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    
    Vector3D& Vector3D::operator/=(float scalar) noexcept
    {
        const float invScalar = 1.0f / scalar;
        x *= invScalar;
        y *= invScalar;
        z *= invScalar;
        return *this;
    }
    
    Vector3D Vector3D::operator-() const noexcept
    {
        return Vector3D(-x, -y, -z);
    }
    
    bool Vector3D::operator==(const Vector3D& other) const noexcept
    {
        return std::abs(x - other.x) < EPSILON && 
               std::abs(y - other.y) < EPSILON && 
               std::abs(z - other.z) < EPSILON;
    }
    
    bool Vector3D::operator!=(const Vector3D& other) const noexcept
    {
        return !(*this == other);
    }
    
    float Vector3D::Length() const noexcept
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    float Vector3D::LengthSquared() const noexcept
    {
        return x * x + y * y + z * z;
    }
    
    Vector3D Vector3D::Normalized() const noexcept
    {
        float len = Length();
        if (len > EPSILON)
        {
            const float invLen = 1.0f / len;
            return Vector3D(x * invLen, y * invLen, z * invLen);
        }
        return *this;
    }
    
    Vector3D& Vector3D::Normalize() noexcept
    {
        float len = Length();
        if (len > EPSILON)
        {
            const float invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
            z *= invLen;
        }
        return *this;
    }
    
    float Vector3D::Dot(const Vector3D& other) const noexcept
    {
        return x * other.x + y * other.y + z * other.z;
    }
    
    Vector3D Vector3D::Cross(const Vector3D& other) const noexcept
    {
        return Vector3D(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    Vector3D Vector3D::RotateX(float angle) const noexcept
    {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        return Vector3D(
            x,
            y * cosA - z * sinA,
            y * sinA + z * cosA
        );
    }
    
    Vector3D Vector3D::RotateY(float angle) const noexcept
    {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        return Vector3D(
            x * cosA + z * sinA,
            y,
            -x * sinA + z * cosA
        );
    }
    
    Vector3D Vector3D::RotateZ(float angle) const noexcept
    {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        return Vector3D(
            x * cosA - y * sinA,
            x * sinA + y * cosA,
            z
        );
    }
    
    float Vector3D::Distance(const Vector3D& a, const Vector3D& b) noexcept
    {
        return (a - b).Length();
    }
    
    float Vector3D::DistanceSquared(const Vector3D& a, const Vector3D& b) noexcept
    {
        return (a - b).LengthSquared();
    }
    
    Vector3D Vector3D::Lerp(const Vector3D& a, const Vector3D& b, float t) noexcept
    {
        return Vector3D(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        );
    }
    
    Vector3D Vector3D::Slerp(const Vector3D& a, const Vector3D& b, float t) noexcept
    {
        float dot = a.Dot(b);
        dot = std::clamp(dot, -1.0f, 1.0f);
        
        float theta = std::acos(dot) * t;
        Vector3D relative = (b - a * dot).Normalized();
        
        return a * std::cos(theta) + relative * std::sin(theta);
    }
    
    Vector3D Vector3D::Reflect(const Vector3D& direction, const Vector3D& normal) noexcept
    {
        return direction - normal * (2.0f * direction.Dot(normal));
    }
    
    Vector3D Vector3D::Project(const Vector3D& vector, const Vector3D& onto) noexcept
    {
        float lenSq = onto.LengthSquared();
        if (lenSq < EPSILON)
            return Vector3D::Zero;
        
        return onto * (vector.Dot(onto) / lenSq);
    }
    
    // Component-wise Min operations
    Vector3D Vector3D::Min(const Vector3D& a, const Vector3D& b) noexcept
    {
        return Vector3D(
            std::min(a.x, b.x),
            std::min(a.y, b.y),
            std::min(a.z, b.z)
        );
    }
    
    // Component-wise Max operations
    Vector3D Vector3D::Max(const Vector3D& a, const Vector3D& b) noexcept
    {
        return Vector3D(
            std::max(a.x, b.x),
            std::max(a.y, b.y),
            std::max(a.z, b.z)
        );
    }
    
    // Component-wise Min with scalar
    Vector3D Vector3D::Min(const Vector3D& a, float scalar) noexcept
    {
        return Vector3D(
            std::min(a.x, scalar),
            std::min(a.y, scalar),
            std::min(a.z, scalar)
        );
    }
    
    // Component-wise Max with scalar
    Vector3D Vector3D::Max(const Vector3D& a, float scalar) noexcept
    {
        return Vector3D(
            std::max(a.x, scalar),
            std::max(a.y, scalar),
            std::max(a.z, scalar)
        );
    }
    
    // Clamp vector components between two vectors
    Vector3D Vector3D::Clamp(const Vector3D& value, const Vector3D& min, const Vector3D& max) noexcept
    {
        return Vector3D(
            std::clamp(value.x, min.x, max.x),
            std::clamp(value.y, min.y, max.y),
            std::clamp(value.z, min.z, max.z)
        );
    }
    
    // Clamp vector components between two scalars
    Vector3D Vector3D::Clamp(const Vector3D& value, float min, float max) noexcept
    {
        return Vector3D(
            std::clamp(value.x, min, max),
            std::clamp(value.y, min, max),
            std::clamp(value.z, min, max)
        );
    }
    
    // Get minimum component value
    float Vector3D::MinComponent() const noexcept
    {
        return std::min({x, y, z});
    }
    
    // Get maximum component value
    float Vector3D::MaxComponent() const noexcept
    {
        return std::max({x, y, z});
    }
    
    // Get index of minimum component
    int32_t Vector3D::MinComponentIndex() const noexcept
    {
        if (x <= y && x <= z) return 0;
        if (y <= x && y <= z) return 1;
        return 2;
    }
    
    // Get index of maximum component
    int32_t Vector3D::MaxComponentIndex() const noexcept
    {
        if (x >= y && x >= z) return 0;
        if (y >= x && y >= z) return 1;
        return 2;
    }

    bool Vector3D::operator<(const Vector3D& other) const
    {
      if (x != other.x) return x < other.x;
      if (y != other.y) return y < other.y;
      return z < other.z;
    }
    bool Vector3D::operator>(const Vector3D& other) const
    {
      if (x != other.x) return x > other.x;
      if (y != other.y) return y > other.y;
      return z > other.z;
    }
    std::string Vector3D::ToString() const
    {
        std::ostringstream oss;
        oss << "(" << x << ", " << y << ", " << z << ")";
        return oss.str();
    }
    
    const Vector3D Vector3D::Zero(0.0f, 0.0f, 0.0f);
    const Vector3D Vector3D::One(1.0f, 1.0f, 1.0f);
    const Vector3D Vector3D::UnitX(1.0f, 0.0f, 0.0f);
    const Vector3D Vector3D::UnitY(0.0f, 1.0f, 0.0f);
    const Vector3D Vector3D::UnitZ(0.0f, 0.0f, 1.0f);
}