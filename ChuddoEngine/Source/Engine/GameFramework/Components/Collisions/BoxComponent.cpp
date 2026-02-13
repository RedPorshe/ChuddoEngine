#include "Components/Collisions/BoxComponent.h"
#include "Actors/Actor.h"
#include "Core/CollisionSystem.h"
#include <algorithm>
#include <limits>

CBoxComponent::CBoxComponent ( CObject * inOwner, const std::string & inDisplayName )
	: Super ( inOwner, inDisplayName )
	{
		// Устанавливаем тип формы как BOX
	SetShapeType ( ECollisionShape::BOX );

	LOG_DEBUG ( "BoxComponent created: ", inDisplayName,
				", HalfExtents: (", m_HalfExtents.x, ", ", m_HalfExtents.y, ", ", m_HalfExtents.z, ")" );
	}

CBoxComponent::~CBoxComponent ()
	{
	LOG_DEBUG ( "BoxComponent destroyed: ", GetName () );
	}

void CBoxComponent::InitComponent ()
	{
	Super::InitComponent ();
	}

void CBoxComponent::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );
	}

void CBoxComponent::OnBeginPlay ()
	{
	Super::OnBeginPlay ();
	}

float CBoxComponent::GetCollisionRadius () const
	{
		// Возвращаем максимальную половину размера как приблизительный радиус
	return std::max ( { m_HalfExtents.x, m_HalfExtents.y, m_HalfExtents.z } );
	}

FVector CBoxComponent::GetWorldLocation () const  // Добавлен const
	{
	CActor * ownerActor = GetOwnerActor ();
	if (ownerActor)
		{
		return ownerActor->GetActorLocation ();
		}

	CObject * owner = GetOwner ();
	int depth = 0;
	while (owner && depth < 10)
		{
		if (CActor * actor = dynamic_cast< CActor * > ( owner ))
			{
			return actor->GetActorLocation ();
			}

		if (CTransformComponent * transform = dynamic_cast< CTransformComponent * > ( owner ))
			{
			return transform->GetLocation ();
			}

		owner = owner->GetOwner ();
		depth++;
		}

	return FVector::Zero ();
	}

FVector CBoxComponent::GetWorldRotation () const  // Добавлен const
	{
	CActor * ownerActor = GetOwnerActor ();
	if (ownerActor)
		{
		return ownerActor->GetActorRotation ();
		}

	CObject * owner = GetOwner ();
	int depth = 0;
	while (owner && depth < 10)
		{
		if (CActor * actor = dynamic_cast< CActor * > ( owner ))
			{
			return actor->GetActorRotation ();
			}

		if (CTransformComponent * transform = dynamic_cast< CTransformComponent * > ( owner ))
			{
			return transform->GetRotation ();
			}

		owner = owner->GetOwner ();
		depth++;
		}

	return FVector::Zero ();
	}

bool CBoxComponent::CheckCollision ( CBaseCollisionComponent * other, FCollisionInfo & outInfo ) const
	{
	if (!other || !IsCollisionEnabled () || !other->IsCollisionEnabled ())
		return false;

	if (!CanCollideWith ( other ))
		return false;

	ECollisionShape otherShape = other->GetShapeType ();
	
	CBoxComponent * nonConstThis = const_cast< CBoxComponent * >( this );

	switch (otherShape)
		{
			case ECollisionShape::NONE:
				{
				return false;
				}

			case ECollisionShape::SPHERE:
				{
				return COLLISION_SYSTEM.CheckSphereBox ( nonConstThis, other, outInfo );
				}

			case ECollisionShape::BOX:
				{
				return COLLISION_SYSTEM.CheckBoxBox ( nonConstThis, other, outInfo );
				}

			case ECollisionShape::CAPSULE:
				{
				return COLLISION_SYSTEM.CheckBoxCapsule ( nonConstThis, other, outInfo );
				}

			case ECollisionShape::CYLINDER:
				{
					// TODO: Implement Sphere-Cylinder collision
				LOG_DEBUG ( "stub for Sphere-Cylinder collision" );
				return false;
				}

			case ECollisionShape::CONE:
				{
					// TODO: Implement Sphere-Cone collision
				LOG_DEBUG ( "stub for Sphere-Cone collision" );
				return false;
				}

			case ECollisionShape::COMPOUND:
				{
					// TODO: Implement Sphere-Compound collision
				LOG_DEBUG ( "stub for Sphere-Compound collision" );
				return false;
				}

			case ECollisionShape::MESH:
				{
					// TODO: Implement Sphere-Mesh collision
				LOG_DEBUG ( "stub for Sphere-Mesh collision" );
				return false;
				}

			case ECollisionShape::TERRAIN:
				{				
				return COLLISION_SYSTEM.CheckBoxTerrain(nonConstThis,other,outInfo);
				}

			case ECollisionShape::RAY:
				{
					// TODO: Implement Sphere-Ray collision
				LOG_DEBUG ( "stub for Sphere-Ray collision" );
				return false;
				}

			case ECollisionShape::PLANE:
				{
					// TODO: Implement Sphere-Plane collision
				LOG_DEBUG ( "stub for Sphere-Plane collision" );
				return false;
				}

			case ECollisionShape::MAX:
			default:
				break;
		}

	return false;
	}
 