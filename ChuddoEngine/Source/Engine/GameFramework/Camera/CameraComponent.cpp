#include "Camera/CameraComponent.h"
#include "Actors/Actor.h"
#include "Render/RenderInfo.h"

CCameraComponent::CCameraComponent ( CObject * inOwner, const std::string & inDisplayName )
	: Super ( inOwner, inDisplayName )
	{}

CCameraComponent::~CCameraComponent ()
	{}

void CCameraComponent::InitComponent ()
	{
	Super::InitComponent ();
	}

void CCameraComponent::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );
	}

void CCameraComponent::OnBeginPlay ()
	{
	Super::OnBeginPlay ();
	}

FMat4 CCameraComponent::GetViewMatrix () const
	{
		// Получаем мировую позицию и вращение из TransformComponent
	FVector location = GetLocation ();
	FQuat rotation = GetRotationQuat ();

	// Стандартный up вектор в мировых координатах
	FVector up = FVector::Up ();

	// Направление взгляда (вперёд) в локальных координатах камеры - обычно по Z
	FVector forward = rotation * FVector::Forward ();
	forward.Normalize ();

	// Цель = позиция + направление
	FVector target = location + forward * NearClipPlane;
	FMat4 result = FMat4::LookAtMatrix ( location, target, up );

	return result;
	}

FMat4 CCameraComponent::GetProjectionMatrix ( float AspectRatio ) const
	{
	return FMat4::PerspectiveMatrix (
		FieldOfView * CEMath::DEG_TO_RAD, // FOV в радианах
		AspectRatio,
		NearClipPlane,
		FarClipPlane
	);
	}


FCameraInfo CCameraComponent::GetCameraInfo ( float AspectRatio ) const
	{
	FCameraInfo Info;

	Info.Location = GetLocation ();

	// Направление взгляда
	FQuat rotation = GetRotationQuat ();
	FVector forward = rotation * FVector::Forward ();
	forward.Normalize ();

	Info.NearPlane = NearClipPlane;
	Info.FarPlane = FarClipPlane;
	Info.ViewTarget = Info.Location + forward * NearClipPlane;
	Info.FOV = FieldOfView;
	Info.ViewMatrix = GetViewMatrix ();
	Info.ProjectionMatrix = GetProjectionMatrix ( AspectRatio );
	
	return Info;
	}

