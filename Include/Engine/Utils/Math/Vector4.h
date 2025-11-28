#pragma once
#include "Vector3.h"
#include <iostream>

namespace CE::Math
{
    template<typename T>
    class Vector4
    {
    public:
        T x, y, z, w;

        Vector4() : x(T(0)), y(T(0)), z(T(0)), w(T(1)) {}
        Vector4(T inX, T inY, T inZ, T inW = T(1)) : x(inX), y(inY), z(inZ), w(inW) {}
        Vector4(const Vector3<T>& vec3, T inW = T(1)) : x(vec3.x), y(vec3.y), z(vec3.z), w(inW) {}
        Vector4(const Vector2<T>& vec2, T inZ = T(0), T inW = T(1)) : x(vec2.x), y(vec2.y), z(inZ), w(inW) {}
        Vector4(T Zero) : x(T(Zero)), y(T(Zero)), z(T(Zero)), w(T(Zero)) {}

        // Базовые операции
        Vector4 operator+(const Vector4& other) const { 
            return Vector4(x + other.x, y + other.y, z + other.z, w + other.w); 
        }
        Vector4 operator-(const Vector4& other) const { 
            return Vector4(x - other.x, y - other.y, z - other.z, w - other.w); 
        }
        Vector4 operator*(T scalar) const { 
            return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); 
        }
        Vector4 operator/(T scalar) const { 
            return Vector4(x / scalar, y / scalar, z / scalar, w / scalar); 
        }

        Vector4& operator+=(const Vector4& other) { 
            x += other.x; y += other.y; z += other.z; w += other.w; return *this; 
        }
        Vector4& operator-=(const Vector4& other) { 
            x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; 
        }
        Vector4& operator*=(T scalar) { 
            x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; 
        }
        Vector4& operator/=(T scalar) { 
            x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; 
        }

        Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }
        
        bool operator==(const Vector4& other) const { 
            return Approximately(static_cast<float>(x), static_cast<float>(other.x)) && 
                   Approximately(static_cast<float>(y), static_cast<float>(other.y)) &&
                   Approximately(static_cast<float>(z), static_cast<float>(other.z)) &&
                   Approximately(static_cast<float>(w), static_cast<float>(other.w));
        }
        bool operator!=(const Vector4& other) const { return !(*this == other); }

        // Математические функции
        T Length() const { 
            return static_cast<T>(std::sqrt(x * x + y * y + z * z + w * w)); 
        }
        
        T LengthSquared() const { 
            return x * x + y * y + z * z + w * w; 
        }
        
        T Dot(const Vector4& other) const { 
            return x * other.x + y * other.y + z * other.z + w * other.w; 
        }

        Vector4 Normalized() const 
        { 
            T len = Length();
            return (len > T(0)) ? *this / len : Vector4(T(0), T(0), T(0), T(1));
        }

        void Normalize() { *this = Normalized(); }

        // Преобразования
        Vector3<T> XYZ() const { return Vector3<T>(x, y, z); }
        Vector2<T> XY() const { return Vector2<T>(x, y); }

        // Перспективное деление (для координат клип-пространства)
        Vector3<T> PerspectiveDivide() const
        {
            return (w != T(0)) ? Vector3<T>(x / w, y / w, z / w) : Vector3<T>(x, y, z);
        }

        // Утилиты
        static Vector4 Lerp(const Vector4& a, const Vector4& b, T t) { 
            return a + (b - a) * t; 
        }
        
        Vector4 Abs() const { 
            return Vector4(std::abs(x), std::abs(y), std::abs(z), std::abs(w)); 
        }

        // Для цветов RGBA
        static Vector4 FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        {
            return Vector4(
                T(r) / T(255), 
                T(g) / T(255), 
                T(b) / T(255), 
                T(a) / T(255)
            );
        }

        uint32_t ToRGBA32() const
        {
            return (static_cast<uint32_t>(x * 255) << 24) |
                   (static_cast<uint32_t>(y * 255) << 16) |
                   (static_cast<uint32_t>(z * 255) << 8) |
                   static_cast<uint32_t>(w * 255);
        }

        // Базовые векторы
        static Vector4 Zero() { return Vector4(T(0), T(0), T(0), T(0)); }
        static Vector4 One() { return Vector4(T(1), T(1), T(1), T(1)); }
    };

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vector4<T>& vec)
    {
        os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
        return os;
    }

    using Vector4f = Vector4<float>;
    using Vector4i = Vector4<int32_t>;
}