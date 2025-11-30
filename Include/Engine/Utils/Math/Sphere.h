#pragma once
#include "Vector3.h"
#include "AABB.h"

namespace CEMath
{
    class Sphere
    {
    public:
        Vector3f center;
        float radius;

        Sphere() : center(Vector3f::Zero()), radius(1.0f) {}
        Sphere(const Vector3f& inCenter, float inRadius) : center(inCenter), radius(inRadius) {}

        bool Contains(const Vector3f& point) const
        {
            return (point - center).LengthSquared() <= radius * radius;
        }

        bool Intersects(const Sphere& other) const
        {
            float distanceSq = (center - other.center).LengthSquared();
            float radiusSum = radius + other.radius;
            return distanceSq <= radiusSum * radiusSum;
        }

        bool Intersects(const AABB& aabb) const
        {
            Vector3f closestPoint = Vector3f::Max(aabb.min, Vector3f::Min(center, aabb.max));
            float distanceSq = (center - closestPoint).LengthSquared();
            return distanceSq <= radius * radius;
        }

        float DistanceToPoint(const Vector3f& point) const
        {
            return (point - center).Length() - radius;
        }

        float GetVolume() const
        {
            return (4.0f / 3.0f) * PI * radius * radius * radius;
        }

        float GetSurfaceArea() const
        {
            return 4.0f * PI * radius * radius;
        }

        AABB GetBoundingBox() const
        {
            return AABB(center, radius);
        }

        void Expand(float amount)
        {
            radius += amount;
            if (radius < 0) radius = 0;
        }

        void Encapsulate(const Vector3f& point)
        {
            float distanceSq = (point - center).LengthSquared();
            if (distanceSq > radius * radius)
            {
                float distance = std::sqrt(distanceSq);
                radius = (radius + distance) * 0.5f;
                center = (center + point) * 0.5f;
            }
        }

        void Encapsulate(const Sphere& other)
        {
            Vector3f direction = other.center - center;
            float distance = direction.Length();
            
            if (distance + other.radius <= radius)
                return; // Already contained
                
            if (distance + radius <= other.radius)
            {
                // Other sphere contains this one
                center = other.center;
                radius = other.radius;
                return;
            }
            
            // Union of two spheres
            float newRadius = (radius + distance + other.radius) * 0.5f;
            float ratio = (newRadius - radius) / distance;
            center = center + direction * ratio;
            radius = newRadius;
        }

        static Sphere FromPoints(const Vector3f* points, int count)
        {
            if (count == 0)
                return Sphere();
                
            Vector3f center = Vector3f::Zero();
            for (int i = 0; i < count; ++i)
                center += points[i];
            center /= static_cast<float>(count);
            
            float maxDistanceSq = 0.0f;
            for (int i = 0; i < count; ++i)
            {
                float distanceSq = (points[i] - center).LengthSquared();
                if (distanceSq > maxDistanceSq)
                    maxDistanceSq = distanceSq;
            }
            
            return Sphere(center, std::sqrt(maxDistanceSq));
        }

        static Sphere Transform(const Sphere& sphere, const Matrix4f& transform)
        {
             Vector4f newCenter4 = transform * Vector4f(sphere.center, 1.0f);
    Vector3f newCenter = newCenter4.XYZ();
            
            // Для сферы нужно найти максимальный масштаб
            Vector3f scale(
                Vector3f(transform(0, 0), transform(0, 1), transform(0, 2)).Length(),
                Vector3f(transform(1, 0), transform(1, 1), transform(1, 2)).Length(),
                Vector3f(transform(2, 0), transform(2, 1), transform(2, 2)).Length()
            );
            
            float maxScale = scale.MaxComponent();
            float newRadius = sphere.radius * maxScale;
            
            return Sphere(newCenter, newRadius);
        }
    };
}