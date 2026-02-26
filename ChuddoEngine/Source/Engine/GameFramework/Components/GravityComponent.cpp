#include "Components/GravityComponent.h"
#include "Actors/Actor.h"
#include "Core/CollisionSystem.h"
#include "Components/Collisions/TerrainComponent.h"
#include "Components/SceneComponent.h"
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

void CGravityComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );

    CActor * owner = GetOwnerActor ();
    if (!owner) return;

   

    bool wasGrounded = bIsOnGround;

    // Получаем коллизионный компонент
    CBaseCollisionComponent * myCollision = owner->FindComponent<CBaseCollisionComponent> ();
    if (!myCollision) return;

    // Получаем имя канала для рейкаста
    std::string channelName = myCollision->GetCollisionChannel ().GetName ();

    FVector currentPos = owner->GetActorLocation ();

    // Если мы на земле, не применяем гравитацию, но проверяем, не ушли ли мы с земли
    if (bIsOnGround)
        {
            // Проверяем, всё ещё ли мы на земле
        CheckGrounded ();

        // Если после проверки мы всё ещё на земле, ничего не делаем
        if (bIsOnGround)
            {
                // Корректируем позицию если нужно
            FVector groundCheck = currentPos + FVector ( 0.f, -m_GroundCheckDistance * 2, 0.f );
            auto result = COLLISION_SYSTEM.Raycast ( currentPos, groundCheck, channelName );

            if (result.bHit && result.HitComponent && myCollision->ShouldBlockWith ( result.HitComponent ))
                {
                float collisionRadius = myCollision->GetCollisionRadius ();
                float targetY = result.Location.y + collisionRadius + 0.1f;

                // Если персонаж провалился, поднимаем его
                if (currentPos.y < targetY - 0.5f)
                    {
                    FVector newPos ( currentPos.x, targetY, currentPos.z );
                    owner->SetActorLocation ( newPos );
                    LOG_DEBUG ( "[GRAVITY] Corrected position to ", targetY );
                    }
                }
            }
        else
            {
                // Мы покинули землю - начинаем падать
            LOG_DEBUG ( "[GRAVITY] Left ground, starting fall" );
            }
        }
    else
        {
            // Мы в воздухе - применяем гравитацию
        m_VerticalVelocity -= m_GravityStrength * m_GravityScale * DeltaTime;

        // Ограничиваем максимальную скорость падения
        float maxFallSpeed = -100.0f;
        if (m_VerticalVelocity < maxFallSpeed)
            {
            m_VerticalVelocity = maxFallSpeed;
            }

            // Рассчитываем вертикальное перемещение за этот кадр
        float verticalDelta = m_VerticalVelocity * DeltaTime;

        // Создаем дельту движения
        FVector moveDelta ( 0.f, verticalDelta, 0.f );
        FVector desiredPos = currentPos + moveDelta;

        // Проверяем коллизии
        FVector safePos = ResolveCollision ( desiredPos, currentPos, channelName );

        // Применяем движение
        if (safePos != currentPos)
            {
            FVector finalDelta = safePos - currentPos;
            
            owner->MoveActor ( finalDelta, false );
            }
        }

        // Логируем изменение состояния
    if (bIsOnGround != wasGrounded)
        {
        LOG_WARN ( "[GRAVITY] ", owner->GetName (),
                   bIsOnGround ? " приземлился" : " покинул землю" );
        }

        // Kill zone check
    if (currentPos.y <= m_KillZone)
        {
        LOG_DEBUG ( "Reached kill zone: ", m_KillZone );
        owner->SetPendingToDestroy ();
        }
    }

void CGravityComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();
    }

void CGravityComponent::CheckGrounded ()
    {
    CActor * owner = GetOwnerActor ();
    if (!owner || !owner->GetRootComponent ())
        {
        bIsOnGround = false;
        return;
        }

    CBaseCollisionComponent * myCollision = owner->FindComponent<CBaseCollisionComponent> ();
    if (!myCollision)
        {
        bIsOnGround = false;
        return;
        }

    std::string channelName = myCollision->GetCollisionChannel ().GetName ();

    FVector start = owner->GetRootComponent ()->GetLocation ();
    float collisionRadius = myCollision->GetCollisionRadius ();

    // Дистанция проверки - немного больше, чем радиус
    float checkDistance = collisionRadius + m_GroundCheckDistance * 2.0f;

    FVector end = start + FVector ( 0.0f, -checkDistance, 0.0f );

    auto result = COLLISION_SYSTEM.Raycast ( start, end, channelName );

    bool wasGrounded = bIsOnGround;
    bIsOnGround = false;

    if (result.bHit && result.HitComponent)
        {
            // Проверяем, блокирует ли мой компонент этот хит
        if (myCollision->ShouldBlockWith ( result.HitComponent ))
            {
                // Проверяем нормаль - должна быть направлена вверх
            if (result.Normal.y > 0.7f)
                {
                float distanceToGround = result.Distance - collisionRadius;

                // Если очень близко к земле (в пределах 1 единицы), считаем что на земле
                if (distanceToGround <= 1.0f)
                    {
                    bIsOnGround = true;
                    }
                }
            }
        }

        // Для отладки
    if (wasGrounded != bIsOnGround)
        {
        if (bIsOnGround)
            {
            LOG_DEBUG ( "[GROUND] Now grounded at Y=", start.y );
            }
        else
            {
            LOG_DEBUG ( "[GROUND] No longer grounded" );
            }
        }
    }

FVector CGravityComponent::ResolveCollision ( const FVector & desiredPos, const FVector & currentPos, const std::string & channelName )
    {
    CActor * owner = GetOwnerActor ();
    if (!owner) return desiredPos;

    if (desiredPos == currentPos)
        return desiredPos;

    CBaseCollisionComponent * myCollision = owner->FindComponent<CBaseCollisionComponent> ();
    if (!myCollision)
        return desiredPos;

    float collisionRadius = myCollision->GetCollisionRadius ();

    // Делаем рейкаст на всю дистанцию движения + запас
    FVector extendedEnd = desiredPos + FVector ( 0.f, -collisionRadius * 2, 0.f );
    auto result = COLLISION_SYSTEM.Raycast ( currentPos, extendedEnd, channelName );

    if (result.bHit && result.HitComponent)
        {
            // Проверяем, блокирующее ли это столкновение
        if (!myCollision->ShouldBlockWith ( result.HitComponent ))
            {
            return desiredPos;
            }

            // Движение вниз
        if (desiredPos.y < currentPos.y)
            {
                // Проверяем, действительно ли это земля (нормаль направлена вверх)
            if (result.Normal.y > 0.7f)
                {
                    // Безопасная позиция - над поверхностью с учетом радиуса
                float safeY = result.Location.y + collisionRadius + 0.1f;

                FVector safePos ( currentPos.x, safeY, currentPos.z );

                // Устанавливаем флаг grounded ТОЛЬКО если мы действительно приземлились
                // и безопасная позиция близка к желаемой
                if (std::abs ( safeY - desiredPos.y ) < 1.0f)
                    {
                    bIsOnGround = true;
                    m_VerticalVelocity = 0.0f;

                    LOG_DEBUG ( "[RESOLVE] Landed at Y=", safeY,
                                " from desired=", desiredPos.y,
                                " ground at Y=", result.Location.y );
                    }

                return safePos;
                }
            }
            // Движение вверх
        else if (desiredPos.y > currentPos.y)
            {
                // Столкновение с потолком
            if (result.Normal.y < -0.7f)
                {
                float safeY = result.Location.y - collisionRadius - 0.1f;
                m_VerticalVelocity = 0.0f;
                LOG_DEBUG ( "[RESOLVE] Hit ceiling at Y=", safeY );
                return FVector ( currentPos.x, safeY, currentPos.z );
                }
            }
        }

    return desiredPos;
    }