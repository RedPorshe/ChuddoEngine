#pragma once
#include "Vector3.h"
#include "AABB.h"
#include "Plane.h"
#include "Sphere.h"
#include "MathConstants.h"

namespace CE::Math
{
    class Ray
    {
    public:
        Vector3f origin;
        Vector3f direction;

        Ray() : origin(Vector3f::Zero()), direction(Vector3f::Forward()) {}
        Ray(const Vector3f& inOrigin, const Vector3f& inDirection) 
            : origin(inOrigin), direction(inDirection.Normalized()) {}

        Vector3f GetPoint(float distance) const
        {
            return origin + direction * distance;
        }

bool Intersects(const AABB& aabb, float& tMin, float& tMax) const
{
    tMin = 0.0f;
    tMax = FLOAT_MAX;

    for (int i = 0; i < 3; ++i)
    {
        // Используем оператор[] вместо direct access
        if (std::abs(direction[i]) < EPSILON)
        {
            // Луч параллелен плоскости
            if (origin[i] < aabb.min[i] || origin[i] > aabb.max[i])
                return false;
        }
        else
        {
            float invDir = 1.0f / direction[i];
            float t1 = (aabb.min[i] - origin[i]) * invDir;
            float t2 = (aabb.max[i] - origin[i]) * invDir;

            if (t1 > t2)
                std::swap(t1, t2);

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax)
                return false;
        }
    }

    return true;
}


        bool Intersects(const Sphere& sphere, float& t) const
        {
            Vector3f oc = origin - sphere.center;
            float a = direction.Dot(direction);
            float b = 2.0f * oc.Dot(direction);
            float c = oc.Dot(oc) - sphere.radius * sphere.radius;
            
            float discriminant = b * b - 4 * a * c;
            
            if (discriminant < 0)
            {
                t = -1.0f;
                return false;
            }
            
            t = (-b - std::sqrt(discriminant)) / (2.0f * a);
            return t >= 0;
        }

        bool Intersects(const Plane& plane, float& t) const
        {
            float denom = plane.normal.Dot(direction);
            
            if (std::abs(denom) < EPSILON)
                return false; // Луч параллелен плоскости
                
            t = -(plane.normal.Dot(origin) + plane.distance) / denom;
            return t >= 0;
        }

        bool IntersectsTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float& t, float& u, float& v) const
        {
            Vector3f edge1 = v1 - v0;
            Vector3f edge2 = v2 - v0;
            Vector3f pvec = direction.Cross(edge2);
            
            float det = edge1.Dot(pvec);
            
            if (det < EPSILON)
                return false;
                
            Vector3f tvec = origin - v0;
            u = tvec.Dot(pvec);
            if (u < 0.0f || u > det)
                return false;
                
            Vector3f qvec = tvec.Cross(edge1);
            v = direction.Dot(qvec);
            if (v < 0.0f || u + v > det)
                return false;
                
            t = edge2.Dot(qvec);
            float invDet = 1.0f / det;
            t *= invDet;
            u *= invDet;
            v *= invDet;
            
            return t >= 0;
        }

        bool IntersectsTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float& t) const
        {
            float u, v;
            return IntersectsTriangle(v0, v1, v2, t, u, v);
        }

        float DistanceToPoint(const Vector3f& point) const
        {
            Vector3f pointToOrigin = point - origin;
            float projection = pointToOrigin.Dot(direction);
            
            if (projection < 0)
                return (point - origin).Length();
                
            Vector3f projectedPoint = origin + direction * projection;
            return (point - projectedPoint).Length();
        }

        Vector3f ClosestPoint(const Vector3f& point) const
        {
            Vector3f pointToOrigin = point - origin;
            float projection = pointToOrigin.Dot(direction);
            
            if (projection < 0)
                return origin;
                
            return origin + direction * projection;
        }

        static Ray FromScreenPoint(const Vector2f& screenPos, const Matrix4f& viewProjInv, const Vector2f& viewportSize)
        {
            // Преобразование из экранных координат в мировые
            Vector2f normalizedCoords = screenPos.ToNormalizedSpace(viewportSize) * 2.0f - Vector2f(1.0f, 1.0f);
            normalizedCoords.y = -normalizedCoords.y; // Vulkan Y flip
            
            Vector4f nearPoint = viewProjInv * Vector4f(normalizedCoords.x, normalizedCoords.y, 0.0f, 1.0f);
            Vector4f farPoint = viewProjInv * Vector4f(normalizedCoords.x, normalizedCoords.y, 1.0f, 1.0f);
            
            nearPoint = nearPoint.PerspectiveDivide();
            farPoint = farPoint.PerspectiveDivide();
            
            Vector3f origin = nearPoint.XYZ();
            Vector3f direction = (farPoint.XYZ() - origin).Normalized();
            
            return Ray(origin, direction);
        }
    };
}