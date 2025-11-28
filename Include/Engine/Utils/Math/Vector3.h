#pragma once
#include "Vector2.h"
#include "Engine/Utils/Logger.h"
#include <iostream>


namespace CE::Math
{
    template<typename T>
    class Vector3
    {
    public:
        T x, y, z;

        Vector3() : x(T(0)), y(T(0)), z(T(0)) {}
        Vector3(T inX, T inY, T inZ) : x(inX), y(inY), z(inZ) {}
        Vector3(const Vector2<T>& vec2, T inZ = T(0)) : x(vec2.x), y(vec2.y), z(inZ) {}
        Vector3(T zero) : x(T(zero)), y(T(zero)),z(T(zero)) {}

        // Базовые операции
        Vector3 operator+(const Vector3& other) const { 
            return Vector3(x + other.x, y + other.y, z + other.z); 
        }
        Vector3 operator-(const Vector3& other) const { 
            return Vector3(x - other.x, y - other.y, z - other.z); 
        }
        Vector3 operator*(T scalar) const { 
            return Vector3(x * scalar, y * scalar, z * scalar); 
        }
        Vector3 operator/(T scalar) const { 
            return Vector3(x / scalar, y / scalar, z / scalar); 
        }
        Vector3 operator*(const Vector3& other) const { 
            return Vector3(x * other.x, y * other.y, z * other.z); 
        }

        Vector3& operator+=(const Vector3& other) { 
            x += other.x; y += other.y; z += other.z; return *this; 
        }
        Vector3& operator-=(const Vector3& other) { 
            x -= other.x; y -= other.y; z -= other.z; return *this; 
        }
        Vector3& operator*=(T scalar) { 
            x *= scalar; y *= scalar; z *= scalar; return *this; 
        }
        Vector3& operator/=(T scalar) { 
            x /= scalar; y /= scalar; z /= scalar; return *this; 
        }

 T& operator[](int index)
{
    switch(index)
    {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: 
            static T dummy;
            CE_CORE_ERROR("Vector3 index out of range");
            return dummy;
    }
}

const T& operator[](int index) const
{
    switch(index)
    {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: 
            static T dummy;
            CE_CORE_ERROR("Vector3 index out of range");
            return dummy;
    }
}

        Vector3 operator-() const { return Vector3(-x, -y, -z); }
        
        bool operator==(const Vector3& other) const { 
            return Approximately(static_cast<float>(x), static_cast<float>(other.x)) && 
                   Approximately(static_cast<float>(y), static_cast<float>(other.y)) &&
                   Approximately(static_cast<float>(z), static_cast<float>(other.z));
        }
        bool operator!=(const Vector3& other) const { return !(*this == other); }

        // Математические функции
        T Length() const { 
            return static_cast<T>(std::sqrt(x * x + y * y + z * z)); 
        }
        
        T LengthSquared() const { 
            return x * x + y * y + z * z; 
        }
        


        T Dot(const Vector3& other) const { 
            return x * other.x + y * other.y + z * other.z; 
        }

        Vector3 Cross(const Vector3& other) const
        {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        Vector3 Normalized() const 
        { 
            T len = Length();
            return (len > T(0)) ? *this / len : Vector3(T(0), T(0), T(0));
        }

        void Normalize() { *this = Normalized(); }

        // Преобразования
        Vector2<T> XY() const { return Vector2<T>(x, y); }
        Vector2<T> XZ() const { return Vector2<T>(x, z); }
        Vector2<T> YZ() const { return Vector2<T>(y, z); }

        // Утилиты
        static Vector3 Lerp(const Vector3& a, const Vector3& b, T t) { 
            return a + (b - a) * t; 
        }
        
        static Vector3 Min(const Vector3& a, const Vector3& b) { 
            return Vector3(
                std::min(a.x, b.x), 
                std::min(a.y, b.y), 
                std::min(a.z, b.z)
            ); 
        }
        
        static Vector3 Max(const Vector3& a, const Vector3& b) { 
            return Vector3(
                std::max(a.x, b.x), 
                std::max(a.y, b.y), 
                std::max(a.z, b.z)
            ); 
        }
        
        Vector3 Abs() const { 
            return Vector3(std::abs(x), std::abs(y), std::abs(z)); 
        }
        
        T MaxComponent() const { 
            return std::max(std::max(x, y), z); 
        }
        
        T MinComponent() const { 
            return std::min(std::min(x, y), z); 
        }

        // Отражение и преломление
        static Vector3 Reflect(const Vector3& incident, const Vector3& normal)
        {
            return incident - normal * (T(2) * incident.Dot(normal));
        }

        static Vector3 Refract(const Vector3& incident, const Vector3& normal, T eta)
        {
            T N_dot_I = normal.Dot(incident);
            T k = T(1) - eta * eta * (T(1) - N_dot_I * N_dot_I);
            if (k < T(0))
                return Vector3(T(0), T(0), T(0));
            return incident * eta - normal * (eta * N_dot_I + std::sqrt(k));
        }

        // Базовые векторы
        static Vector3 Forward() { return Vector3(T(0), T(0), T(1)); }
        static Vector3 Backward() { return Vector3(T(0), T(0), T(-1)); }
        static Vector3 Up() { return Vector3(T(0), T(1), T(0)); }
        static Vector3 Down() { return Vector3(T(0), T(-1), T(0)); }
        static Vector3 Right() { return Vector3(T(1), T(0), T(0)); }
        static Vector3 Left() { return Vector3(T(-1), T(0), T(0)); }
        static Vector3 Zero() { return Vector3(T(0), T(0), T(0)); }
        static Vector3 One() { return Vector3(T(1), T(1), T(1)); }
    };

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vector3<T>& vec)
    {
        os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }

    using Vector3f = Vector3<float>;
    using Vector3i = Vector3<int32_t>;
}