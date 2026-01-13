#pragma once

#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "CE_MathConstants.h"
#include "CE_MathHelpers.h"

namespace CEMath
{
	using Vec2 = Vector2D;
	using Vec3 = Vector3D;
	using Vec4 = Vector4D;
	using Mat4 = Matrix4x4;
	using Quat = Quaternion;
} // namespace CEMath

using FVector2D = CEMath::Vec2;
using FVector = CEMath::Vec3;
using FVector4D = CEMath::Vec4;
using FMat4 = CEMath::Mat4;
using FQuat = CEMath::Quat;