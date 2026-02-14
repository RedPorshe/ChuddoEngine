#include "Components/Collisions/ConeComponent.h"
#include "Actors/Actor.h"
#include "Core/CollisionSystem.h"
#include <cmath>

CConeComponent::CConeComponent ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {
    SetShapeType ( ECollisionShape::CONE );
    LOG_DEBUG ( "ConeComponent created: ", GetName (),
                ", Radius: ", m_Radius,
                ", Height: ", m_Height );
    }

CConeComponent::~CConeComponent ()
    {}

void CConeComponent::InitComponent ()
    {
    Super::InitComponent ();
    }

void CConeComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }

void CConeComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();
    }

    // ============================================================================
    // Collision Checks
    // ============================================================================

bool CConeComponent::CheckCollision ( CBaseCollisionComponent * other, FCollisionInfo & outInfo ) const
    {
    if (!other || !IsCollisionEnabled () || !other->IsCollisionEnabled ())
        return false;

    if (!CanCollideWith ( other ))
        return false;

    ECollisionShape otherShape = other->GetShapeType ();
    CConeComponent * nonConstThis = const_cast< CConeComponent * >( this );

    switch (otherShape)
        {
            case ECollisionShape::NONE:
                return false;

            case ECollisionShape::SPHERE:
                return COLLISION_SYSTEM.CheckSphereCone ( other, nonConstThis, outInfo );

            case ECollisionShape::BOX:
                return COLLISION_SYSTEM.CheckBoxCone ( other, nonConstThis, outInfo );

            case ECollisionShape::CAPSULE:
                return COLLISION_SYSTEM.CheckCapsuleCone ( other, nonConstThis, outInfo );

            case ECollisionShape::CYLINDER:
                return COLLISION_SYSTEM.CheckCylinderCone ( other, nonConstThis, outInfo );

            case ECollisionShape::CONE:
                return COLLISION_SYSTEM.CheckConeCone ( nonConstThis, other, outInfo );

            case ECollisionShape::TERRAIN:
                return COLLISION_SYSTEM.CheckConeTerrain ( nonConstThis, other, outInfo );

            case ECollisionShape::COMPOUND:
            case ECollisionShape::MESH:
            case ECollisionShape::RAY:
            case ECollisionShape::PLANE:
                LOG_DEBUG ( "stub for Cone-", ( int ) otherShape, " collision" );
                return false;

            case ECollisionShape::MAX:
            default:
                break;
        }

    return false;
    }

float CConeComponent::GetCollisionRadius () const
    {
        // Для грубых проверок возвращаем радиус описанной сферы
    return std::sqrt ( m_Radius * m_Radius + m_Height * m_Height );
    }

    // ============================================================================
    // Cone Specific Methods
    // ============================================================================

FVector CConeComponent::GetTip () const
    {
    FVector worldPos = GetWorldLocation ();
    FQuat rotation = GetOwnerActor ()->GetActorRotationQuat ();

    // Острие конуса - вверху по локальной оси Y
    FVector localOffset ( 0.0f, m_Height * 0.5f, 0.0f );
    FVector worldOffset = rotation * localOffset;

    return worldPos + worldOffset;
    }

FVector CConeComponent::GetBaseCenter () const
    {
    FVector worldPos = GetWorldLocation ();
    FQuat rotation = GetOwnerActor ()->GetActorRotationQuat ();

    // Центр основания - внизу по локальной оси Y
    FVector localOffset ( 0.0f, -m_Height * 0.5f, 0.0f );
    FVector worldOffset = rotation * localOffset;

    return worldPos + worldOffset;
    }

float CConeComponent::GetSlope () const
    {
        // Наклон стенки = радиус / высота
    return m_Radius / m_Height;
    }