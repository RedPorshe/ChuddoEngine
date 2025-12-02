#include "Math/Quaternion.hpp"
#include "Math/Matrix4x4.hpp"



namespace CEMath
{
    Quaternion Quaternion::FromAxisAngle(const Vector3D& axis, float angle) noexcept
    {
        float halfAngle = angle * 0.5f;
        float sinHalf = std::sin(halfAngle);
        Vector3D normalizedAxis = axis.Normalized();
        
        return Quaternion(
            normalizedAxis.x * sinHalf,
            normalizedAxis.y * sinHalf,
            normalizedAxis.z * sinHalf,
            std::cos(halfAngle)
        );
    }
    
    Quaternion Quaternion::FromEuler(float pitch, float yaw, float roll) noexcept
    {
        float halfPitch = pitch * 0.5f;
        float halfYaw = yaw * 0.5f;
        float halfRoll = roll * 0.5f;
        
        float sinPitch = std::sin(halfPitch);
        float cosPitch = std::cos(halfPitch);
        float sinYaw = std::sin(halfYaw);
        float cosYaw = std::cos(halfYaw);
        float sinRoll = std::sin(halfRoll);
        float cosRoll = std::cos(halfRoll);
        
        return Quaternion(
            cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
            cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw,
            sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw,
            cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw
        );
    }
    
   std::string Quaternion::ToStringAngleAxis() const noexcept
{
    Vector3D axis;
    float angle;
    ToAxisAngle(axis, angle);
    
    std::ostringstream oss;
    oss << "Angle: " << angle * RAD_TO_DEG << " degrees, Axis: " << axis;
    return oss.str();
}

    Quaternion Quaternion::FromEuler(const Vector3D& euler) noexcept
    {
        return FromEuler(euler.x, euler.y, euler.z);
    }
    
    Quaternion Quaternion::FromRotationMatrix(const float mat[4][4]) noexcept
    {
        float trace = mat[0][0] + mat[1][1] + mat[2][2];
        
        if (trace > 0.0f)
        {
            float s = std::sqrt(trace + 1.0f) * 2.0f;
            return Quaternion(
                (mat[2][1] - mat[1][2]) / s,
                (mat[0][2] - mat[2][0]) / s,
                (mat[1][0] - mat[0][1]) / s,
                0.25f * s
            );
        }
        else if (mat[0][0] > mat[1][1] && mat[0][0] > mat[2][2])
        {
            float s = std::sqrt(1.0f + mat[0][0] - mat[1][1] - mat[2][2]) * 2.0f;
            return Quaternion(
                0.25f * s,
                (mat[0][1] + mat[1][0]) / s,
                (mat[0][2] + mat[2][0]) / s,
                (mat[2][1] - mat[1][2]) / s
            );
        }
        else if (mat[1][1] > mat[2][2])
        {
            float s = std::sqrt(1.0f + mat[1][1] - mat[0][0] - mat[2][2]) * 2.0f;
            return Quaternion(
                (mat[0][1] + mat[1][0]) / s,
                0.25f * s,
                (mat[1][2] + mat[2][1]) / s,
                (mat[0][2] - mat[2][0]) / s
            );
        }
        else
        {
            float s = std::sqrt(1.0f + mat[2][2] - mat[0][0] - mat[1][1]) * 2.0f;
            return Quaternion(
                (mat[0][2] + mat[2][0]) / s,
                (mat[1][2] + mat[2][1]) / s,
                0.25f * s,
                (mat[1][0] - mat[0][1]) / s
            );
        }
    }
    
    float& Quaternion::operator[](uint32_t index) noexcept
    {
        return (&x)[index];
    }
    
    float Quaternion::operator[](uint32_t index) const noexcept
    {
        return (&x)[index];
    }
    
    Quaternion Quaternion::operator+(const Quaternion& other) const noexcept
    {
        return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
    }
    
    Quaternion Quaternion::operator-(const Quaternion& other) const noexcept
    {
        return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
    }
    
    Quaternion Quaternion::operator*(const Quaternion& other) const noexcept
    {
        return Quaternion(
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w,
            w * other.w - x * other.x - y * other.y - z * other.z
        );
    }

    Vector3D Quaternion::operator*(const Vector3D& other) const noexcept
    {
       Quaternion q = this->Normalized();
    
   
    
    Vector3D qv(q.x, q.y, q.z);
    float dot = qv.Dot(other);
    
    return other * (q.w * q.w - qv.LengthSquared()) + 
           qv * (2.0f * dot) + 
           qv.Cross(other) * (2.0f * q.w);
    }

    Quaternion Quaternion::operator*(float scalar) const noexcept
    {
        return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar);
    }
    
    Quaternion Quaternion::operator/(float scalar) const noexcept
    {
        const float invScalar = 1.0f / scalar;
        return Quaternion(x * invScalar, y * invScalar, z * invScalar, w * invScalar);
    }
    
    Quaternion& Quaternion::operator+=(const Quaternion& other) noexcept
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }
    
    Quaternion& Quaternion::operator-=(const Quaternion& other) noexcept
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }
    
    Quaternion& Quaternion::operator*=(const Quaternion& other) noexcept
    {
        *this = *this * other;
        return *this;
    }
    
    Quaternion& Quaternion::operator*=(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }
    
    Quaternion& Quaternion::operator/=(float scalar) noexcept
    {
        const float invScalar = 1.0f / scalar;
        x *= invScalar;
        y *= invScalar;
        z *= invScalar;
        w *= invScalar;
        return *this;
    }
    
    Quaternion Quaternion::operator-() const noexcept
    {
        return Quaternion(-x, -y, -z, -w);
    }
    
    bool Quaternion::operator==(const Quaternion& other) const noexcept
    {
        return std::abs(x - other.x) < EPSILON && 
               std::abs(y - other.y) < EPSILON && 
               std::abs(z - other.z) < EPSILON && 
               std::abs(w - other.w) < EPSILON;
    }
    
    bool Quaternion::operator!=(const Quaternion& other) const noexcept
    {
        return !(*this == other);
    }

    bool Quaternion::IsUnit() const noexcept
    {
         return std::abs(LengthSquared() - 1.0f) < EPSILON;
    }
    float Quaternion::Length() const noexcept
    {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }
    
    float Quaternion::LengthSquared() const noexcept
    {
        return x * x + y * y + z * z + w * w;
    }
    
    Quaternion Quaternion::Normalized() const noexcept
    {
        float len = Length();
        if (len > EPSILON)
        {
            const float invLen = 1.0f / len;
            return Quaternion(x * invLen, y * invLen, z * invLen, w * invLen);
        }
        return *this;
    }
    
    Quaternion& Quaternion::Normalize() noexcept
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
    
    Quaternion Quaternion::Conjugate() const noexcept
    {
        return Quaternion(-x, -y, -z, w);
    }
    
    Quaternion Quaternion::Inverse() const noexcept
    {
        float lenSq = LengthSquared();
        if (lenSq > EPSILON)
        {
            float invLenSq = 1.0f / lenSq;
            return Conjugate() * invLenSq;
        }
        return *this;
    }
    
    float Quaternion::Dot(const Quaternion& other) const noexcept
    {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }
    
    Vector3D Quaternion::Rotate(const Vector3D& vector) const noexcept
    {
        Quaternion vecQuat(vector.x, vector.y, vector.z, 0.0f);
        Quaternion result = (*this) * vecQuat * Conjugate();
        return Vector3D(result.x, result.y, result.z);
    }
    
    Quaternion Quaternion::Slerp(const Quaternion& to, float t) const noexcept
    {
        return Quaternion::Slerp(*this, to, t);
    }
    
    Vector3D Quaternion::ToEuler() const noexcept
    {
        // Roll (X-axis rotation)
        float sinr_cosp = 2.0f * (w * x + y * z);
        float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
        float roll = std::atan2(sinr_cosp, cosr_cosp);
        
        // Pitch (Y-axis rotation)
        float sinp = 2.0f * (w * y - z * x);
        float pitch = 0.0f;
        if (std::abs(sinp) >= 1.0f)
            pitch = std::copysign(PI / 2.0f, sinp); // Use 90 degrees if out of range
        else
            pitch = std::asin(sinp);
        
        // Yaw (Z-axis rotation)
        float siny_cosp = 2.0f * (w * z + x * y);
        float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
        float yaw = std::atan2(siny_cosp, cosy_cosp);
        
        return Vector3D(pitch, yaw, roll);
    }
    
    void Quaternion::ToAxisAngle(Vector3D& axis, float& angle) const noexcept
    {
        Quaternion normalized = Normalized();
        angle = 2.0f * std::acos(normalized.w);
        
        float s = std::sqrt(1.0f - normalized.w * normalized.w);
        if (s < EPSILON)
        {
            axis = Vector3D::UnitX;
        }
        else
        {
            axis = Vector3D(normalized.x / s, normalized.y / s, normalized.z / s);
        }
    }
    
 Matrix4x4 Quaternion::ToMatrix() const noexcept
    {
        // Нормализуем кватернион, чтобы убедиться что он единичный
        Quaternion q = this->Normalized();
        
        // Вычисляем основные компоненты
        float xx = q.x * q.x;
        float yy = q.y * q.y;
        float zz = q.z * q.z;
        float xy = q.x * q.y;
        float xz = q.x * q.z;
        float yz = q.y * q.z;
        float wx = q.w * q.x;
        float wy = q.w * q.y;
        float wz = q.w * q.z;
        
        // Создаем матрицу поворота 4x4 (только поворот, без трансляции и масштаба)
        return Matrix4x4(
            // Row 0
            1.0f - 2.0f * (yy + zz),
            2.0f * (xy - wz),
            2.0f * (xz + wy),
            0.0f,
            
            // Row 1
            2.0f * (xy + wz),
            1.0f - 2.0f * (xx + zz),
            2.0f * (yz - wx),
            0.0f,
            
            // Row 2
            2.0f * (xz - wy),
            2.0f * (yz + wx),
            1.0f - 2.0f * (xx + yy),
            0.0f,
            
            // Row 3
            0.0f,
            0.0f,
            0.0f,
            1.0f
        );
    }


    Quaternion Quaternion::Lerp(const Quaternion& a, const Quaternion& b, float t) noexcept
    {
        return Quaternion(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t
        ).Normalized();
    }
    
    Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t) noexcept
    {
        float cosHalfTheta = a.Dot(b);
        
        Quaternion bTemp = b;
        if (cosHalfTheta < 0.0f)
        {
            bTemp = -b;
            cosHalfTheta = -cosHalfTheta;
        }
        
        if (cosHalfTheta > 0.9999f)
        {
            return Lerp(a, bTemp, t);
        }
        
        float halfTheta = std::acos(cosHalfTheta);
        float sinHalfTheta = std::sqrt(1.0f - cosHalfTheta * cosHalfTheta);
        
        if (std::abs(sinHalfTheta) < EPSILON)
        {
            return Quaternion(
                (a.x + bTemp.x) * 0.5f,
                (a.y + bTemp.y) * 0.5f,
                (a.z + bTemp.z) * 0.5f,
                (a.w + bTemp.w) * 0.5f
            );
        }
        
        float ratioA = std::sin((1.0f - t) * halfTheta) / sinHalfTheta;
        float ratioB = std::sin(t * halfTheta) / sinHalfTheta;
        
        return Quaternion(
            a.x * ratioA + bTemp.x * ratioB,
            a.y * ratioA + bTemp.y * ratioB,
            a.z * ratioA + bTemp.z * ratioB,
            a.w * ratioA + bTemp.w * ratioB
        );
    }
    
    Quaternion Quaternion::LookRotation(const Vector3D& forward, const Vector3D& up) noexcept
    {
        Vector3D f = forward.Normalized();
        Vector3D u = up.Normalized();
        Vector3D r = u.Cross(f).Normalized();
        u = f.Cross(r);
        
        float m00 = r.x;
        float m01 = r.y;
        float m02 = r.z;
        float m10 = u.x;
        float m11 = u.y;
        float m12 = u.z;
        float m20 = f.x;
        float m21 = f.y;
        float m22 = f.z;
        
        float num8 = (m00 + m11) + m22;
        if (num8 > 0.0f)
        {
            float num = std::sqrt(num8 + 1.0f);
            float invNum = 0.5f / num;
            return Quaternion(
                (m12 - m21) * invNum,
                (m20 - m02) * invNum,
                (m01 - m10) * invNum,
                num * 0.5f
            );
        }
        else if ((m00 >= m11) && (m00 >= m22))
        {
            float num7 = std::sqrt(1.0f + m00 - m11 - m22);
            float invNum = 0.5f / num7;
            return Quaternion(
                0.5f * num7,
                (m01 + m10) * invNum,
                (m02 + m20) * invNum,
                (m12 - m21) * invNum
            );
        }
        else if (m11 > m22)
        {
            float num6 = std::sqrt(1.0f + m11 - m00 - m22);
            float invNum = 0.5f / num6;
            return Quaternion(
                (m10 + m01) * invNum,
                0.5f * num6,
                (m21 + m12) * invNum,
                (m20 - m02) * invNum
            );
        }
        else
        {
            float num5 = std::sqrt(1.0f + m22 - m00 - m11);
            float invNum = 0.5f / num5;
            return Quaternion(
                (m20 + m02) * invNum,
                (m21 + m12) * invNum,
                0.5f * num5,
                (m01 - m10) * invNum
            );
        }
    }
}