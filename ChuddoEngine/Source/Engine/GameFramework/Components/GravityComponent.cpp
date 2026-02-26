// GravityComponent.cpp
#include "Components/GravityComponent.h"
#include "Actors/Actor.h"
#include "Core/CollisionSystem.h"
#include "Components/Collisions/TerrainComponent.h"

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
	m_LastPosition = GetOwnerActor ()->GetActorLocation ();
	}

void CGravityComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );

    CActor * owner = GetOwnerActor ();
    if (!owner) return;

    // Логируем состояние до применения гравитации
    static float logTimer = 0.f;
    logTimer += DeltaTime;
    if (logTimer > 3.0f)
        {
        LOG_WARN ( "[GRAVITY] ", owner->GetName (),
                   " Grounded: ", bIsOnGround ? "Yes" : "No",
                   " Velocity: ", m_VerticalVelocity,
                   " Pos: ", owner->GetActorLocation () );
        logTimer = 0.f;
        }

    bool wasGrounded = bIsOnGround;

    // Проверяем, на земле ли мы
    CheckGrounded ();

    FVector currentPos = owner->GetActorLocation ();

    // ВАЖНО: Если мы на земле, не применяем гравитацию!
    if (!bIsOnGround)
        {
        LOG_DEBUG ( "[GRAVITY] Applying gravity to ", owner->GetName () );
        // Применяем гравитацию
        m_VerticalVelocity -= m_GravityStrength * m_GravityScale * DeltaTime;

        // Рассчитываем вертикальное перемещение за этот кадр
        float verticalDelta = m_VerticalVelocity * DeltaTime;

        // Создаем дельту движения
        FVector moveDelta ( 0.f, verticalDelta, 0.f );

        // Проверяем коллизии
        FVector desiredPos = currentPos + moveDelta;
        FVector safePos = ResolveCollision ( desiredPos, currentPos );

        // Вычисляем финальную дельту
        FVector finalDelta = safePos - currentPos;

        // Применяем движение через актора (ТОЛЬКО если есть куда двигаться)
        if (!finalDelta.IsZero ())
            {
            LOG_DEBUG ( "[GRAVITY] Moving with delta: ", finalDelta );
            owner->MoveActor ( finalDelta );
            }
        }
    else
        {
        // На земле - скорость равна 0 И не двигаемся вниз
        m_VerticalVelocity = 0.0f;

        // Дополнительно: если мы на земле, убеждаемся, что не проваливаемся
        FVector groundCheck = currentPos + FVector ( 0.f, -5.f, 0.f );
        auto result = COLLISION_SYSTEM.Raycast ( currentPos, groundCheck, "All" );
        if (result.bHit && result.Normal.y > 0.7f)
            {
            // Стоим точно на поверхности
            float groundHeight = result.Location.y;
            float characterHeight = currentPos.y;

            // Если персонаж немного провалился, поднимаем его
            if (characterHeight < groundHeight - 0.1f)
                {
                owner->SetActorLocation ( FVector ( currentPos.x, groundHeight, currentPos.z ) );
                }
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
        LOG_DEBUG ( "Достигнута kill zone: ", m_KillZone );
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
    if (!owner || !owner->GetRootComponent ()) return;

    FVector start = owner->GetRootComponent ()->GetLocation ();
    // Увеличим дистанцию проверки для надёжности
    float checkDistance = std::max ( m_GroundCheckDistance, 5.0f );
    FVector end = start + FVector ( 0.0f, -checkDistance, 0.0f );

    auto result = COLLISION_SYSTEM.Raycast ( start, end, "All" );

    // Сбрасываем grounded, если нет хита
    bIsOnGround = false;

    if (result.bHit && result.HitComponent)
        {
        // Проверяем нормаль - должна быть направлена вверх
        if (result.Normal.y > 0.7f)
            {
            // Проверяем дистанцию - должны быть очень близко к поверхности
            if (result.Distance <= checkDistance + 0.1f)
                {
                // Проверяем, что это блокирующее столкновение
                CBaseCollisionComponent * myCollision = owner->FindComponent<CBaseCollisionComponent> ();
                if (myCollision && myCollision->ShouldBlockWith ( result.HitComponent ))
                    {
                    bIsOnGround = true;
                    LOG_DEBUG ( "[GROUND] ", owner->GetName (), " is on ground. Normal: ",
                                result.Normal.x, ", ", result.Normal.y, ", ", result.Normal.z,
                                " Distance: ", result.Distance );
                    return;
                    }
                }
            }
        }

    // Если не на земле, логируем причину (для отладки)
    if (!bIsOnGround)
        {
        if (!result.bHit)
            LOG_DEBUG ( "[GROUND] ", owner->GetName (), " not grounded - no hit" );
        else if (result.Normal.y <= 0.7f)
            LOG_DEBUG ( "[GROUND] ", owner->GetName (), " not grounded - bad normal: ", result.Normal.y );
        else if (result.Distance > checkDistance + 0.1f)
            LOG_DEBUG ( "[GROUND] ", owner->GetName (), " not grounded - too far: ", result.Distance );
        else if (!owner->FindComponent<CBaseCollisionComponent> ()->ShouldBlockWith ( result.HitComponent ))
            LOG_DEBUG ( "[GROUND] ", owner->GetName (), " not grounded - not blocking" );
        }
    }

   
FVector CGravityComponent::ResolveCollision ( const FVector & desiredPos, const FVector & currentPos )
    {
    CActor * owner = GetOwnerActor ();
    if (!owner) return desiredPos;

    // Если движение нулевое - ничего не делаем
    if (desiredPos == currentPos)
        return desiredPos;

    // Рейкаст от текущей позиции к желаемой
    auto result = COLLISION_SYSTEM.Raycast ( currentPos, desiredPos, "All" );

    if (result.bHit && result.HitComponent)
        {
        // Проверяем, блокирующее ли это столкновение
        CBaseCollisionComponent * myCollision = owner->FindComponent<CBaseCollisionComponent> ();
        if (!myCollision || !myCollision->ShouldBlockWith ( result.HitComponent ))
            {
            // Если не блокирующее, разрешаем движение
            return desiredPos;
            }

        // Рассчитываем безопасную позицию (немного перед столкновением)
        float direction = ( desiredPos.y > currentPos.y ) ? 1.0f : -1.0f;
        float safeDistance = std::max ( result.Distance - 0.1f, 0.0f ); // Увеличил зазор

        FVector safePos = currentPos;
        safePos.y += direction * safeDistance;

        // Если двигались вниз И нормаль направлена вверх - мы на земле
        if (desiredPos.y < currentPos.y && result.Normal.y > 0.7f)
            {
            bIsOnGround = true;
            m_VerticalVelocity = 0.0f;
            LOG_DEBUG ( "[COLLISION] ", owner->GetName (), " landed on ground at Y=", safePos.y );
            }
        // Если двигались вверх и упёрлись в потолок - останавливаем вертикальную скорость
        else if (desiredPos.y > currentPos.y && result.Normal.y < -0.7f)
            {
            m_VerticalVelocity = 0.0f;
            LOG_DEBUG ( "[COLLISION] ", owner->GetName (), " hit ceiling" );
            }

        return safePos;
        }

    return desiredPos;
    }