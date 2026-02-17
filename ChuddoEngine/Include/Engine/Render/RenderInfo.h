
#pragma once

#include "CoreMinimal.h"
#include "Render/Mesh.h"
#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include <type_traits>

// Basic render data structures used to collect frame data from game side

struct RenderCamera
	{
	// Camera transform (world space)
	FTransform Transform;

	// Projection params
	float FovY = 45.0f; // degrees
	float Aspect = 4.0f / 3.0f;
	float Near = 0.1f;
	float Far = 1000.0f;
	};

struct RenderLight
	{
	// Simple directional light for now
	FVector Direction = FVector ( 0.0f, -1.0f, 0.0f );
	FVector Color = FVector ( 1.0f, 1.0f, 1.0f );
	float Intensity = 1.0f;
	};

// Per-object data that renderer will consume
struct RenderObject
	{
	Mesh * MeshPtr = nullptr; // CPU-side mesh pointer (renderer may upload GPU buffers)
	FTransform Transform;     // Model transform
	uint32_t MaterialId = 0;  // Placeholder for material identification
	bool Visible = true;
	};

struct RenderScene // main package of data per-frame
	{
	float DeltaTime = 0.0f;

	// Camera used to render this frame
	RenderCamera Camera;

	// Simple global ambient color
	FVector AmbientColor = FVector ( 0.05f, 0.05f, 0.05f );

	// Lights
	std::vector< RenderLight > Lights;

	// Objects to render
	std::vector< RenderObject > Objects;

	void Clear ()
		{
		DeltaTime = 0.0f;
		Camera = RenderCamera {};
		AmbientColor = FVector ( 0.05f, 0.05f, 0.05f );
		Lights.clear ();
		Objects.clear ();
		}
	};

