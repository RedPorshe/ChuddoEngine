#pragma once
#include "Vector3.h"
#include "Matrix4.h"

namespace CEMath
{
    class AABB
    {
    public:
        Vector3f min;
        Vector3f max;

        AABB() : min(Vector3f::Zero()), max(Vector3f::Zero()) {}
        AABB(const Vector3f& inMin, const Vector3f& inMax) : min(inMin), max(inMax) {}
        AABB(const Vector3f& center, float radius)
        {
            min = center - Vector3f(radius, radius, radius);
            max = center + Vector3f(radius, radius, radius);
        }

        void Expand(const Vector3f& point)
        {
            min = Vector3f::Min(min, point);
            max = Vector3f::Max(max, point);
        }

        void Expand(const AABB& other)
        {
            min = Vector3f::Min(min, other.min);
            max = Vector3f::Max(max, other.max);
        }

        bool Intersects(const AABB& other) const
        {
            return (min.x <= other.max.x && max.x >= other.min.x) &&
                   (min.y <= other.max.y && max.y >= other.min.y) &&
                   (min.z <= other.max.z && max.z >= other.min.z);
        }

        bool Contains(const Vector3f& point) const
        {
            return (point.x >= min.x && point.x <= max.x) &&
                   (point.y >= min.y && point.y <= max.y) &&
                   (point.z >= min.z && point.z <= max.z);
        }

        bool Contains(const AABB& other) const
        {
            return Contains(other.min) && Contains(other.max);
        }

        Vector3f GetCenter() const
        {
            return (min + max) * 0.5f;
        }

        Vector3f GetSize() const
        {
            return max - min;
        }

        Vector3f GetExtents() const
        {
            return GetSize() * 0.5f;
        }

        float GetSurfaceArea() const
        {
            Vector3f size = GetSize();
            return 2.0f * (size.x * size.y + size.x * size.z + size.y * size.z);
        }

        float GetVolume() const
        {
            Vector3f size = GetSize();
            return size.x * size.y * size.z;
        }

        Vector3f GetCorner(int index) const
        {
            switch (index)
            {
                case 0: return Vector3f(min.x, min.y, min.z);
                case 1: return Vector3f(max.x, min.y, min.z);
                case 2: return Vector3f(min.x, max.y, min.z);
                case 3: return Vector3f(max.x, max.y, min.z);
                case 4: return Vector3f(min.x, min.y, max.z);
                case 5: return Vector3f(max.x, min.y, max.z);
                case 6: return Vector3f(min.x, max.y, max.z);
                case 7: return Vector3f(max.x, max.y, max.z);
                default: return Vector3f::Zero();
            }
        }

        void Encapsulate(const Vector3f& point)
        {
            Expand(point);
        }

        void Encapsulate(const AABB& bounds)
        {
            Expand(bounds);
        }

        bool IsValid() const
        {
            return min.x <= max.x && min.y <= max.y && min.z <= max.z;
        }

      static AABB Transform(const AABB& aabb, const Matrix4f& transform)
{
    if (!aabb.IsValid())
        return AABB();

    Vector3f center = aabb.GetCenter();
    Vector3f extents = aabb.GetExtents();

    // Исправленная строка:
    Vector4f newCenter4 = transform * Vector4f(center, 1.0f);
    Vector3f newCenter = newCenter4.XYZ(); // Теперь правильно
    
    Vector3f newExtents(
        std::abs(transform(0, 0)) * extents.x + std::abs(transform(1, 0)) * extents.y + std::abs(transform(2, 0)) * extents.z,
        std::abs(transform(0, 1)) * extents.x + std::abs(transform(1, 1)) * extents.y + std::abs(transform(2, 1)) * extents.z,
        std::abs(transform(0, 2)) * extents.x + std::abs(transform(1, 2)) * extents.y + std::abs(transform(2, 2)) * extents.z
    );

    return AABB(newCenter - newExtents, newCenter + newExtents);
}

        static AABB Union(const AABB& a, const AABB& b)
        {
            if (!a.IsValid()) return b;
            if (!b.IsValid()) return a;
            
            AABB result;
            result.min = Vector3f::Min(a.min, b.min);
            result.max = Vector3f::Max(a.max, b.max);
            return result;
        }

        static AABB Intersection(const AABB& a, const AABB& b)
        {
            if (!a.Intersects(b))
                return AABB();
                
            AABB result;
            result.min = Vector3f::Max(a.min, b.min);
            result.max = Vector3f::Min(a.max, b.max);
            return result;
        }
    };
}