#include "Components/GravityComponent.h"
#include "Actors/Actor.h"
#include "Core/CollisionSystem.h"
#include "Components/Collisions/BaseCollisionComponent.h"

CGravityComponent::CGravityComponent ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {
    LOG_DEBUG ( "GravityComponent created: ", GetName () );
    }

CGravityComponent::~CGravityComponent ()
    {}

void CGravityComponent::InitComponent ()
    {
    Super::InitComponent ();
    CActor * owner = GetOwnerActor ();
    if (owner)
        {
        m_LastPosition = owner->GetActorLocation ();
        }
    }

void CGravityComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();
    }

void CGravityComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );

    CActor * owner = GetOwnerActor ();
    if (!owner) return;

    CBaseCollisionComponent * myCollision = owner->FindComponent<CBaseCollisionComponent> ();
    if (!myCollision) return;

    FVector currentPos = owner->GetActorLocation ();
    float collisionRadius = myCollision->GetCollisionRadius ();

    // Проверяем, есть ли земля ПРЯМО ПОД НАМИ
    FVector checkStart = currentPos;
    FVector checkEnd = currentPos + FVector ( 0.0f, -( collisionRadius + 1.0f ), 0.0f );

    FRaycastResult rayResult = COLLISION_SYSTEM.Raycast (
        checkStart,
        checkEnd,
        myCollision->GetCollisionChannel ().GetName ()
    );

    // Если нашли землю прямо под нами - мы на земле
    bool bFoundGround = rayResult.bHit && rayResult.HitComponent;

    // Запоминаем предыдущее состояние
    m_bWasGrounded = bIsOnGround;
    bIsOnGround = bFoundGround;

    // Если только что приземлились
    if (bIsOnGround && !m_bWasGrounded)
        {
        m_VerticalVelocity = 0.0f;

        // Корректируем позицию, чтобы стоять точно на земле
        float targetY = rayResult.Location.y + collisionRadius;
        if (std::abs ( currentPos.y - targetY ) > 0.001f)
            {
            owner->SetActorLocation ( FVector ( currentPos.x, targetY, currentPos.z ) );
            }

        LOG_DEBUG ( "[ПРИЗЕМЛЕНИЕ] ", owner->GetName (), " на Y=", owner->GetActorLocation ().y );
        return;
        }

        // Если не на земле - падаем
    if (!bIsOnGround)
        {
            // Применяем гравитацию
        m_VerticalVelocity -= m_GravityStrength * m_GravityScale * DeltaTime;

        // Ограничиваем скорость падения
        if (m_VerticalVelocity < m_MaxFallSpeed)
            {
            m_VerticalVelocity = m_MaxFallSpeed;
            }

        float deltaMove = m_VerticalVelocity * DeltaTime;

        // Проверяем, не встретим ли мы землю на пути вниз
        FVector moveStart = currentPos;
        FVector moveEnd = currentPos + FVector ( 0.0f, deltaMove, 0.0f );

        FRaycastResult moveResult = COLLISION_SYSTEM.Raycast (
            moveStart,
            moveEnd,
            myCollision->GetCollisionChannel ().GetName ()
        );

        if (moveResult.bHit && moveResult.HitComponent)
            {
                // Нашли землю на пути - останавливаемся
            float targetY = moveResult.Location.y + collisionRadius;
            owner->SetActorLocation ( FVector ( currentPos.x, targetY, currentPos.z ) );
            m_VerticalVelocity = 0.0f;
            bIsOnGround = true;

            LOG_DEBUG ( "[ПРИЗЕМЛЕНИЕ В ПРОЦЕССЕ] ", owner->GetName (), " на Y=", targetY );
            }
        else
            {
                // Нет земли - двигаемся
            owner->SetActorLocation ( moveEnd );
            }
        }

        // Проверка kill zone
    if (owner->GetActorLocation ().y <= m_KillZone)
        {
        LOG_DEBUG ( "Объект ", owner->GetName (), " достиг kill zone" );
        owner->SetPendingToDestroy ();
        }
    }