#pragma once

#include <cmath>
#include "CE_MathConstants.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Quaternion.h"
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

struct FTransform 
{
	FVector Location;
	FQuat Rotation;
	FVector Scale;
	
	FTransform()
		: Location(FVector::Zero()), Rotation(FQuat()), Scale(FVector::One()) {}
	
	FTransform(const FVector& InLocation, const FQuat& InRotation, const FVector& InScale)
		: Location(InLocation), Rotation(InRotation), Scale(InScale) {}
	
	// Constructor that accepts Euler angles in degrees (for convenience)
	FTransform(const FVector& InLocation, const FVector& InRotationEulerDegrees, const FVector& InScale, bool bEulerDegrees)
		: Location(InLocation), Scale(InScale)
	{
		if (bEulerDegrees)
		{
			FVector eulerRad(
				CEMath::DegreesToRadians(InRotationEulerDegrees.x),
				CEMath::DegreesToRadians(InRotationEulerDegrees.y),
				CEMath::DegreesToRadians(InRotationEulerDegrees.z)
			);
			Rotation = FQuat::FromEulerAngles(eulerRad.x, eulerRad.y, eulerRad.z);
		}
		else
		{
			Rotation = FQuat::FromEulerAngles(InRotationEulerDegrees.x, InRotationEulerDegrees.y, InRotationEulerDegrees.z);
		}
	}
	
	// Get rotation as Euler angles in degrees with snap-to-grid for precision
	FVector GetRotationEulerDegrees() const
	{
		FVector eulerRad = Rotation.GetEulerAngles();
		FVector eulerDeg(
			CEMath::RadiansToDegrees(eulerRad.x),
			CEMath::RadiansToDegrees(eulerRad.y),
			CEMath::RadiansToDegrees(eulerRad.z)
		);
		
		// Apply snap-to-grid to fix floating point errors from Quaternion->Euler conversion
		const float EPSILON = 0.1f;
		
		// Snap X (Pitch)
		if (std::fabs(eulerDeg.x - 90.0f) < EPSILON) eulerDeg.x = 90.0f;
		else if (std::fabs(eulerDeg.x - 180.0f) < EPSILON) eulerDeg.x = 180.0f;
		else if (std::fabs(eulerDeg.x - 270.0f) < EPSILON) eulerDeg.x = 270.0f;
		else if (std::fabs(eulerDeg.x - 360.0f) < EPSILON || std::fabs(eulerDeg.x) < EPSILON) eulerDeg.x = 0.0f;
		
		// Snap Y (Yaw)
		if (std::fabs(eulerDeg.y - 90.0f) < EPSILON) eulerDeg.y = 90.0f;
		else if (std::fabs(eulerDeg.y - 180.0f) < EPSILON) eulerDeg.y = 180.0f;
		else if (std::fabs(eulerDeg.y - 270.0f) < EPSILON) eulerDeg.y = 270.0f;
		else if (std::fabs(eulerDeg.y - 360.0f) < EPSILON || std::fabs(eulerDeg.y) < EPSILON) eulerDeg.y = 0.0f;
		
		// Snap Z (Roll)
		if (std::fabs(eulerDeg.z - 90.0f) < EPSILON) eulerDeg.z = 90.0f;
		else if (std::fabs(eulerDeg.z - 180.0f) < EPSILON) eulerDeg.z = 180.0f;
		else if (std::fabs(eulerDeg.z - 270.0f) < EPSILON) eulerDeg.z = 270.0f;
		else if (std::fabs(eulerDeg.z - 360.0f) < EPSILON || std::fabs(eulerDeg.z) < EPSILON) eulerDeg.z = 0.0f;
		
		return eulerDeg;
	}
	
	static FTransform Identity()
	{
		return FTransform(FVector::Zero(), FQuat(), FVector::One());
	}
};
