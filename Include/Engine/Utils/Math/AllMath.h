#pragma once

#include "MathConstants.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "AABB.h"
#include "Sphere.h"
#include "Plane.h"
#include "Ray.h"
#include "Frustum.h"
#include "Color.h"

namespace Math
{
    template<typename T>
    T Min(T a, T b) { return std::min(a, b); }

    template<typename T>
    T Max(T a, T b) { return std::max(a, b); }

    template<typename T>
    T Lerp(T a, T b, T t) { return a + (b - a) * t; }


    
}
