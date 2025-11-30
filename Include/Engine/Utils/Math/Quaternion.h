#pragma once
#include "Vector3.h"
#include "Matrix4.h"

namespace CEMath
{
    template<typename T>
    class Quaternion
    {
    public:
        T x, y, z, w;

        Quaternion() : x(T(0)), y(T(0)), z(T(0)), w(T(1)) {}
        Quaternion(T inX, T inY, T inZ, T inW) : x(inX), y(inY), z(inZ), w(inW) {}
        Quaternion(const Vector3<T>& axis, T angle)
        {
            T halfAngle = angle * T(0.5);
            T sinHalf = std::sin(halfAngle);
            T cosHalf = std::cos(halfAngle);
            
            x = axis.x * sinHalf;
            y = axis.y * sinHalf;
            z = axis.z * sinHalf;
            w = cosHalf;
        }

        // Базовые операции
        Quaternion operator+(const Quaternion& other) const
        {
            return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
        }

        Quaternion operator-(const Quaternion& other) const
        {
            return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
        }

        Quaternion operator*(const Quaternion& other) const
        {
            return Quaternion(
                w * other.x + x * other.w + y * other.z - z * other.y,
                w * other.y - x * other.z + y * other.w + z * other.x,
                w * other.z + x * other.y - y * other.x + z * other.w,
                w * other.w - x * other.x - y * other.y - z * other.z
            );
        }

        Quaternion operator*(T scalar) const
        {
            return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar);
        }

        Quaternion operator/(T scalar) const
        {
            return Quaternion(x / scalar, y / scalar, z / scalar, w / scalar);
        }

        Quaternion& operator*=(const Quaternion& other)
        {
            *this = *this * other;
            return *this;
        }

        Quaternion& operator*=(T scalar)
        {
            x *= scalar; y *= scalar; z *= scalar; w *= scalar;
            return *this;
        }

        bool operator==(const Quaternion& other) const
        {
            return Approximately(static_cast<float>(x), static_cast<float>(other.x)) &&
                   Approximately(static_cast<float>(y), static_cast<float>(other.y)) &&
                   Approximately(static_cast<float>(z), static_cast<float>(other.z)) &&
                   Approximately(static_cast<float>(w), static_cast<float>(other.w));
        }

        bool operator!=(const Quaternion& other) const { return !(*this == other); }

        // Векторное умножение
        Vector3<T> operator*(const Vector3<T>& point) const
        {
            Vector3<T> qvec(x, y, z);
            Vector3<T> uv = qvec.Cross(point) * T(2);
            Vector3<T> uuv = qvec.Cross(uv) * T(2);
            return point + uv * w + uuv;
        }

        // Математические функции
        T Length() const
        {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        T LengthSquared() const
        {
            return x * x + y * y + z * z + w * w;
        }

        Quaternion Normalized() const
        {
            T len = Length();
            if (len > T(0))
                return *this / len;
            return Quaternion(T(0), T(0), T(0), T(1));
        }

        void Normalize()
        {
            *this = Normalized();
        }

        Quaternion Conjugate() const
        {
            return Quaternion(-x, -y, -z, w);
        }

        Quaternion Inverse() const
        {
            T lenSq = LengthSquared();
            if (lenSq > T(0))
                return Conjugate() / lenSq;
            return Quaternion(T(0), T(0), T(0), T(1));
        }

        T Dot(const Quaternion& other) const
        {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        // Преобразования
        Matrix4<T> ToMatrix() const
        {
            Matrix4<T> result;
            
            T xx = x * x, yy = y * y, zz = z * z;
            T xy = x * y, xz = x * z, xw = x * w;
            T yz = y * z, yw = y * w, zw = z * w;
            
            result(0, 0) = T(1) - T(2) * (yy + zz);
            result(0, 1) = T(2) * (xy - zw);
            result(0, 2) = T(2) * (xz + yw);
            
            result(1, 0) = T(2) * (xy + zw);
            result(1, 1) = T(1) - T(2) * (xx + zz);
            result(1, 2) = T(2) * (yz - xw);
            
            result(2, 0) = T(2) * (xz - yw);
            result(2, 1) = T(2) * (yz + xw);
            result(2, 2) = T(1) - T(2) * (xx + yy);
            
            return result;
        }

        Vector3<T> ToEulerAngles() const
        {
            Vector3<T> euler;
            
            // roll (x-axis rotation)
            T sinr_cosp = T(2) * (w * x + y * z);
            T cosr_cosp = T(1) - T(2) * (x * x + y * y);
            euler.x = std::atan2(sinr_cosp, cosr_cosp);
            
            // pitch (y-axis rotation)
            T sinp = T(2) * (w * y - z * x);
            if (std::abs(sinp) >= T(1))
                euler.y = std::copysign(PI / T(2), sinp);
            else
                euler.y = std::asin(sinp);
            
            // yaw (z-axis rotation)
            T siny_cosp = T(2) * (w * z + x * y);
            T cosy_cosp = T(1) - T(2) * (y * y + z * z);
            euler.z = std::atan2(siny_cosp, cosy_cosp);
            
            return euler;
        }

        // Статические конструкторы
        static Quaternion FromAxisAngle(const Vector3<T>& axis, T angle)
        {
            return Quaternion(axis, angle);
        }

        static Quaternion FromEulerAngles(T pitch, T yaw, T roll)
        {
            T cy = std::cos(yaw * T(0.5));
            T sy = std::sin(yaw * T(0.5));
            T cp = std::cos(pitch * T(0.5));
            T sp = std::sin(pitch * T(0.5));
            T cr = std::cos(roll * T(0.5));
            T sr = std::sin(roll * T(0.5));
            
            return Quaternion(
                cy * cp * sr - sy * sp * cr,
                sy * cp * sr + cy * sp * cr,
                sy * cp * cr - cy * sp * sr,
                cy * cp * cr + sy * sp * sr
            );
        }

        static Quaternion FromEulerAngles(const Vector3<T>& euler)
        {
            return FromEulerAngles(euler.x, euler.y, euler.z);
        }

        static Quaternion LookRotation(const Vector3<T>& forward, const Vector3<T>& up = Vector3<T>(0,1,0))
        {
            Vector3<T> f = forward.Normalized();
            Vector3<T> u = up.Normalized();
            Vector3<T> r = u.Cross(f);
            u = f.Cross(r);
            
            T m00 = r.x, m01 = r.y, m02 = r.z;
            T m10 = u.x, m11 = u.y, m12 = u.z;
            T m20 = f.x, m21 = f.y, m22 = f.z;
            
            T num8 = (m00 + m11) + m22;
            Quaternion quat;
            
            if (num8 > T(0))
            {
                T num = std::sqrt(num8 + T(1));
                quat.w = num * T(0.5);
                num = T(0.5) / num;
                quat.x = (m12 - m21) * num;
                quat.y = (m20 - m02) * num;
                quat.z = (m01 - m10) * num;
                return quat;
            }
            
            if ((m00 >= m11) && (m00 >= m22))
            {
                T num7 = std::sqrt(((T(1) + m00) - m11) - m22);
                T num4 = T(0.5) / num7;
                quat.x = T(0.5) * num7;
                quat.y = (m01 + m10) * num4;
                quat.z = (m02 + m20) * num4;
                quat.w = (m12 - m21) * num4;
                return quat;
            }
            
            if (m11 > m22)
            {
                T num6 = std::sqrt(((T(1) + m11) - m00) - m22);
                T num3 = T(0.5) / num6;
                quat.x = (m10 + m01) * num3;
                quat.y = T(0.5) * num6;
                quat.z = (m21 + m12) * num3;
                quat.w = (m20 - m02) * num3;
                return quat;
            }
            
            T num5 = std::sqrt(((T(1) + m22) - m00) - m11);
            T num2 = T(0.5) / num5;
            quat.x = (m20 + m02) * num2;
            quat.y = (m21 + m12) * num2;
            quat.z = T(0.5) * num5;
            quat.w = (m01 - m10) * num2;
            return quat;
        }

        // Интерполяция
        static Quaternion Lerp(const Quaternion& a, const Quaternion& b, T t)
        {
            return (a * (T(1) - t) + b * t).Normalized();
        }

        static Quaternion Slerp(const Quaternion& a, const Quaternion& b, T t)
        {
            T cosHalfTheta = a.Dot(b);
            
            if (cosHalfTheta < T(0))
            {
                return Slerp(a * T(-1), b, t);
            }
            
            if (std::abs(cosHalfTheta) >= T(1))
                return a;
                
            T halfTheta = std::acos(cosHalfTheta);
            T sinHalfTheta = std::sqrt(T(1) - cosHalfTheta * cosHalfTheta);
            
            if (std::abs(sinHalfTheta) < T(0.001))
                return Lerp(a, b, t);
                
            T ratioA = std::sin((T(1) - t) * halfTheta) / sinHalfTheta;
            T ratioB = std::sin(t * halfTheta) / sinHalfTheta;
            
            return (a * ratioA + b * ratioB).Normalized();
        }

        // Статические кватернионы
        static Quaternion Identity() { return Quaternion(T(0), T(0), T(0), T(1)); }
    };

    using Quaternionf = Quaternion<float>;
}