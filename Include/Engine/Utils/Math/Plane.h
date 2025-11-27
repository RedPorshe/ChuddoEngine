#pragma once
#include "Vector3.h"
#include "MathConstants.h"

namespace CE::Math
{
    class Plane
    {
    public:
        Vector3f normal;
        float distance;

        Plane() : normal(0, 1, 0), distance(0) {}
        Plane(const Vector3f& inNormal, float inDistance) : normal(inNormal), distance(inDistance) {}
        Plane(const Vector3f& point, const Vector3f& inNormal) : normal(inNormal.Normalized())
        {
            distance = -normal.Dot(point);
        }
        Plane(const Vector3f& a, const Vector3f& b, const Vector3f& c)
        {
            Vector3f ab = b - a;
            Vector3f ac = c - a;
            normal = ab.Cross(ac).Normalized();
            distance = -normal.Dot(a);
        }

        float DistanceToPoint(const Vector3f& point) const
        {
            return normal.Dot(point) + distance;
        }

        Vector3f ProjectPoint(const Vector3f& point) const
        {
            float dist = DistanceToPoint(point);
            return point - normal * dist;
        }

        bool GetSide(const Vector3f& point) const
        {
            return DistanceToPoint(point) > 0;
        }

        bool SameSide(const Vector3f& a, const Vector3f& b) const
        {
            float da = DistanceToPoint(a);
            float db = DistanceToPoint(b);
            return (da > 0 && db > 0) || (da <= 0 && db <= 0);
        }

        void Normalize()
        {
            float magnitude = normal.Length();
            if (magnitude > EPSILON)
            {
                normal /= magnitude;
                distance /= magnitude;
            }
        }

static bool IntersectThreePlanes(const Plane& p1, const Plane& p2, const Plane& p3, Vector3f& result)
{
    // Реализация пересечения трех плоскостей
    Vector3f n1 = p1.normal;
    Vector3f n2 = p2.normal;
    Vector3f n3 = p3.normal;
    
    Vector3f n2n3 = n2.Cross(n3);
    Vector3f n3n1 = n3.Cross(n1);
    Vector3f n1n2 = n1.Cross(n2);
    
    float denom = n1.Dot(n2n3);
    if (std::abs(denom) < EPSILON)
        return false;
        
    result = (n2n3 * -p1.distance + n3n1 * -p2.distance + n1n2 * -p3.distance) / denom;
    return true;
}

        Plane Normalized() const
        {
            Plane result = *this;
            result.Normalize();
            return result;
        }

        static Plane FromPointAndNormal(const Vector3f& point, const Vector3f& normal)
        {
            return Plane(point, normal);
        }

        static Plane FromPoints(const Vector3f& a, const Vector3f& b, const Vector3f& c)
        {
            return Plane(a, b, c);
        }
    };
}