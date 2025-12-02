#pragma once
#include "CoreMinimal.h"
#include <cstdint>
#include "AllMath.h"
namespace CEMath
{
    class Color : public Vector4D
    {
    public:
        float& r = x;
        float& g = y;
        float& b = z;
        float& a = w;

        Color() : Vector4D(0,0,0,1) {}
        Color(float r, float g, float b, float a = 1.0f) : Vector4D(r,g,b,a) {}
        Color(const Vector4D& vec) : Vector4D(vec) {}
        
        float GetR() const { return x; }
        float GetG() const { return y; }
        float GetB() const { return z; }
        float GetA() const { return w; }
        
        void SetR(float r) { x = r; }
        void SetG(float g) { y = g; }
        void SetB(float b) { z = b; }
        void SetA(float a) { w = a; }
        
        Vector3D toRGB() const {
          return Vector3D(r, g, b);
        }
        // sRGB преобразования
        Color LinearToSRGB() const
        {
            auto linearToSRGB = [](float linear) -> float {
                if (linear <= 0.0031308f)
                    return linear * 12.92f;
                return 1.055f * std::pow(linear, 1.0f / 2.4f) - 0.055f;
            };
            
            return Color(
                linearToSRGB(x),
                linearToSRGB(y),
                linearToSRGB(z),
                w
            );
        }
        
        Color SRGBToLinear() const
        {
            auto sRGBToLinear = [](float srgb) -> float {
                if (srgb <= 0.04045f)
                    return srgb / 12.92f;
                return std::pow((srgb + 0.055f) / 1.055f, 2.4f);
            };
            
            return Color(
                sRGBToLinear(x),
                sRGBToLinear(y),
                sRGBToLinear(z),
                w
            );
        }
        
        // HSV преобразования
        static Color FromHSV(float h, float s, float v, float a = 1.0f)
        {
            if (s <= 0.0f)
                return Color(v, v, v, a);
                
            h = std::fmod(h, 1.0f);
            if (h < 0.0f)
                h += 1.0f;
                
            h *= 6.0f;
            int i = static_cast<int>(h);
            float f = h - i;
            float p = v * (1.0f - s);
            float q = v * (1.0f - s * f);
            float t = v * (1.0f - s * (1.0f - f));
            
            switch (i)
            {
                case 0: return Color(v, t, p, a);
                case 1: return Color(q, v, p, a);
                case 2: return Color(p, v, t, a);
                case 3: return Color(p, q, v, a);
                case 4: return Color(t, p, v, a);
                case 5: return Color(v, p, q, a);
                default: return Color(v, v, v, a);
            }
        }
        
        Vector4D ToHSV() const
        {
            float max = std::max(std::max(x, y), z);
            float min = std::min(std::min(x, y), z);
            float delta = max - min;
            
            float h = 0.0f;
            if (delta > 0.0f)
            {
                if (max == x)
                    h = (y - z) / delta;
                else if (max == y)
                    h = 2.0f + (z - x) / delta;
                else
                    h = 4.0f + (x - y) / delta;
                    
                h /= 6.0f;
                if (h < 0.0f)
                    h += 1.0f;
            }
            
            float s = (max > 0.0f) ? (delta / max) : 0.0f;
            float v = max;
            
            return Vector4D(h, s, v, w);
        }
        
        // Статические цвета
        static Color Red() { return Color(1,0,0,1); }
        static Color Green() { return Color(0,1,0,1); }
        static Color Blue() { return Color(0,0,1,1); }
        static Color White() { return Color(1,1,1,1); }
        static Color Black() { return Color(0,0,0,1); }
        static Color Yellow() { return Color(1,1,0,1); }
        static Color Cyan() { return Color(0,1,1,1); }
        static Color Magenta() { return Color(1,0,1,1); }
        static Color Gray() { return Color(0.5f,0.5f,0.5f,1); }
        static Color Clear() { return Color(0,0,0,0); }
        
        // Цвета для UI
        static Color Orange() { return Color(1,0.5f,0,1); }
        static Color Purple() { return Color(0.5f,0,0.5f,1); }
        static Color Pink() { return Color(1,0.75f,0.8f,1); }
        static Color Brown() { return Color(0.65f,0.16f,0.16f,1); }
        
        // Создание из 32-битного значения
        static Color FromRGBA32(uint32_t rgba)
        {
            return Color(
                ((rgba >> 24) & 0xFF) / 255.0f,
                ((rgba >> 16) & 0xFF) / 255.0f,
                ((rgba >> 8) & 0xFF) / 255.0f,
                (rgba & 0xFF) / 255.0f
            );
        }
        
        uint32_t ToRGBA32() const
        {
            return (static_cast<uint32_t>(x * 255) << 24) |
                   (static_cast<uint32_t>(y * 255) << 16) |
                   (static_cast<uint32_t>(z * 255) << 8) |
                   static_cast<uint32_t>(w * 255);
        }
        
        // Операции с цветами
        Color operator*(const Color& other) const
        {
            return Color(x * other.x, y * other.y, z * other.z, w * other.w);
        }
        
        Color& operator*=(const Color& other)
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;
            return *this;
        }
        
        // Гамма-коррекция
        Color GammaCorrect(float gamma) const
        {
            float invGamma = 1.0f / gamma;
            return Color(
                std::pow(x, invGamma),
                std::pow(y, invGamma),
                std::pow(z, invGamma),
                w
            );
        }
        
        // Яркость (luminance)
        float GetLuminance() const
        {
            return 0.2126f * x + 0.7152f * y + 0.0722f * z;
        }
        
        // Инвертирование цвета
        Color Inverted() const
        {
            return Color(1.0f - x, 1.0f - y, 1.0f - z, w);
        }
    };
}