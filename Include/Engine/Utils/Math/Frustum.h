#pragma once
#include "Plane.h"
#include "AABB.h"
#include "Sphere.h"
#include "Matrix4.h"

namespace CEMath
{
    class Frustum
    {
    public:
        enum EPlaneType { 
            LEFT = 0, 
            RIGHT, 
            TOP, 
            BOTTOM, 
            NEAR_PLANE,  // Изменили NEAR -> NEAR_PLANE
            FAR_PLANE,   // Изменили FAR -> FAR_PLANE
            COUNT 
        };
        
        enum class EIntersectionType
        {
            INTERSECTS_NONE,
            INTERSECTS_PARTIALLY,
            INTERSECTS_FULLY
        };

        Plane planes[COUNT];
        
        Frustum() = default;
        
        void ExtractFromMatrix(const Matrix4f& viewProj)
        {
            const float* m = viewProj.Data();
            
            // Left plane
            planes[LEFT] = Plane(
                Vector3f(m[3] + m[0], m[7] + m[4], m[11] + m[8]),
                m[15] + m[12]
            );
            
            // Right plane
            planes[RIGHT] = Plane(
                Vector3f(m[3] - m[0], m[7] - m[4], m[11] - m[8]),
                m[15] - m[12]
            );
            
            // Bottom plane
            planes[BOTTOM] = Plane(
                Vector3f(m[3] + m[1], m[7] + m[5], m[11] + m[9]),
                m[15] + m[13]
            );
            
            // Top plane
            planes[TOP] = Plane(
                Vector3f(m[3] - m[1], m[7] - m[5], m[11] - m[9]),
                m[15] - m[13]
            );
            
            // Near plane
            planes[NEAR_PLANE] = Plane(
                Vector3f(m[3] + m[2], m[7] + m[6], m[11] + m[10]),
                m[15] + m[14]
            );
            
            // Far plane
            planes[FAR_PLANE] = Plane(
                Vector3f(m[3] - m[2], m[7] - m[6], m[11] - m[10]),
                m[15] - m[14]
            );
            
            // Нормализуем все плоскости
            for (int i = 0; i < COUNT; ++i)
                planes[i].Normalize();
        }
        
        bool ContainsPoint(const Vector3f& point) const
        {
            for (int i = 0; i < COUNT; ++i)
            {
                if (planes[i].DistanceToPoint(point) < 0)
                    return false;
            }
            return true;
        }
        
        bool IntersectsSphere(const Sphere& sphere) const
        {
            for (int i = 0; i < COUNT; ++i)
            {
                if (planes[i].DistanceToPoint(sphere.center) < -sphere.radius)
                    return false;
            }
            return true;
        }
        
        bool IntersectsAABB(const AABB& aabb) const
        {
            for (int i = 0; i < COUNT; ++i)
            {
                Vector3f positiveVertex = aabb.min;
                if (planes[i].normal.x >= 0) positiveVertex.x = aabb.max.x;
                if (planes[i].normal.y >= 0) positiveVertex.y = aabb.max.y;
                if (planes[i].normal.z >= 0) positiveVertex.z = aabb.max.z;
                
                if (planes[i].DistanceToPoint(positiveVertex) < 0)
                    return false;
            }
            return true;
        }
        
        EIntersectionType GetAABBIntersectionType(const AABB& aabb) const
        {
            bool completelyInside = true;
            
            for (int i = 0; i < COUNT; ++i)
            {
                Vector3f positiveVertex = aabb.min;
                if (planes[i].normal.x >= 0) positiveVertex.x = aabb.max.x;
                if (planes[i].normal.y >= 0) positiveVertex.y = aabb.max.y;
                if (planes[i].normal.z >= 0) positiveVertex.z = aabb.max.z;
                
                if (planes[i].DistanceToPoint(positiveVertex) < 0)
                    return EIntersectionType::INTERSECTS_NONE;  // Исправлено: EIntersectionType
                    
                Vector3f negativeVertex = aabb.max;
                if (planes[i].normal.x >= 0) negativeVertex.x = aabb.min.x;
                if (planes[i].normal.y >= 0) negativeVertex.y = aabb.min.y;
                if (planes[i].normal.z >= 0) negativeVertex.z = aabb.min.z;
                
                if (planes[i].DistanceToPoint(negativeVertex) < 0)
                    completelyInside = false;
            }
            
            return completelyInside ? 
                EIntersectionType::INTERSECTS_FULLY :  // Исправлено: EIntersectionType
                EIntersectionType::INTERSECTS_PARTIALLY;  // Исправлено: EIntersectionType
        }
        
        void GetCorners(Vector3f corners[8]) const
        {
            // Вычисляем углы frustum через пересечение плоскостей
            // Исправлено: правильный вызов статического метода
            Plane::IntersectThreePlanes(planes[NEAR_PLANE], planes[LEFT], planes[TOP], corners[0]);
            Plane::IntersectThreePlanes(planes[NEAR_PLANE], planes[RIGHT], planes[TOP], corners[1]);
            Plane::IntersectThreePlanes(planes[NEAR_PLANE], planes[RIGHT], planes[BOTTOM], corners[2]);
            Plane::IntersectThreePlanes(planes[NEAR_PLANE], planes[LEFT], planes[BOTTOM], corners[3]);
            
            Plane::IntersectThreePlanes(planes[FAR_PLANE], planes[LEFT], planes[TOP], corners[4]);
            Plane::IntersectThreePlanes(planes[FAR_PLANE], planes[RIGHT], planes[TOP], corners[5]);
            Plane::IntersectThreePlanes(planes[FAR_PLANE], planes[RIGHT], planes[BOTTOM], corners[6]);
            Plane::IntersectThreePlanes(planes[FAR_PLANE], planes[LEFT], planes[BOTTOM], corners[7]);
        }
    };
}