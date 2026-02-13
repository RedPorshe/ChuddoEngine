#include "Components/Collisions/SphereComponent.h"
#include "Core/CollisionSystem.h"

CSphereComponent::CSphereComponent ( CObject * inOwner, const std::string & inDisplayName, float InRadius )
    :Super(inOwner,inDisplayName),m_Radius(InRadius)
    {
    SetShapeType ( ECollisionShape::SPHERE );
    }

CSphereComponent::~CSphereComponent ()
    {
    }

void CSphereComponent::InitComponent ()
    {
    Super::InitComponent ();
    }

void CSphereComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }

void CSphereComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();
    }

bool CSphereComponent::CheckCollision ( CBaseCollisionComponent * other, FCollisionInfo & outInfo ) const
    {
    if (!other || !IsCollisionEnabled () || !other->IsCollisionEnabled ())
        return false;

    if (!CanCollideWith ( other ))
        return false;

    ECollisionShape otherShape = other->GetShapeType ();
    
    CSphereComponent * nonConstThis = const_cast< CSphereComponent * >( this );

    switch (otherShape)
        {
            case ECollisionShape::NONE:
                {
                return false;
                }

            case ECollisionShape::SPHERE:
                {
                return COLLISION_SYSTEM.CheckSphereSphere ( nonConstThis, other, outInfo );
                }

            case ECollisionShape::BOX:
                {
                return COLLISION_SYSTEM.CheckSphereBox ( nonConstThis, other, outInfo );
                }

            case ECollisionShape::CAPSULE:
                {
                return COLLISION_SYSTEM.CheckSphereCapsule ( nonConstThis, other, outInfo );
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
                return COLLISION_SYSTEM.CheckSphereTerrain(nonConstThis,other,outInfo);
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
