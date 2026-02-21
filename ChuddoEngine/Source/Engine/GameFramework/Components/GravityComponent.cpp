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
    CheckGrounded ();

    CActor * owner = GetOwnerActor ();
    if (!owner) return;

    FVector currentPos = owner->GetActorLocation ();

    if (!bIsOnGround)
        {
            // Гравитация: v = v - g * dt
        m_VerticalVelocity -= m_GravityStrength * m_GravityScale * DeltaTime;

        // Смещение = скорость * dt
        float verticalDelta = m_VerticalVelocity * DeltaTime;

        // Применяем смещение
        FVector newPos = currentPos;
        newPos.y += verticalDelta;

        // Проверяем коллизию и получаем безопасную позицию
        FVector safePos = ResolveTerrainCollision ( newPos, currentPos );

        // Вычисляем фактическое смещение от текущей позиции
        FVector finalDelta = safePos - currentPos;

        // Перемещаем актора на это смещение
        if (!finalDelta.IsZero ())
            {
            owner->MoveActor ( finalDelta, false );  // false = мгновенное перемещение
            }

            // Логируем изменение состояния
        if (bIsOnGround != m_bWasGrounded)
            {
            LOG_DEBUG ( "[GRAVITY] ", owner->GetName (),
                        bIsOnGround ? " landed" : " left ground" );
            m_bWasGrounded = bIsOnGround;
            }
        }

        // Проверка kill zone
    if (currentPos.y <= m_KillZone)
        {
        LOG_DEBUG ( "Reached kill zone : ", m_KillZone );
        GetOwnerActor ()->SetPendingToDestroy ();
        return;
        }

        // Проверка на бесконечные координаты
    if (CEMath::Abs ( currentPos.y ) > 100000.0f)
        {
        LOG_ERROR ( "[GRAVITY] Actor ", owner->GetName (),
                    " at impossible Y position: ", currentPos.y );
        currentPos.y = 100.0f;
        owner->SetActorLocation ( currentPos );
        m_VerticalVelocity = 0.0f;
        return;
        }
    }


void CGravityComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();
    }



void CGravityComponent::CheckGrounded ()
    {
    CActor * owner = GetOwnerActor ();
    if (!owner) return;

    FVector start = owner->GetActorLocation ();
    FVector end = start + FVector ( 0.0f, -m_GroundCheckDistance, 0.0f );

    // Делаем рейкаст вниз
    auto result = COLLISION_SYSTEM.Raycast ( start, end, "All" );

    bIsOnGround = result.bHit &&
        result.HitComponent &&
        result.HitComponent->GetShapeType () == ECollisionShape::TERRAIN &&
        result.Distance <= m_GroundCheckDistance;
    }

FVector CGravityComponent::ResolveTerrainCollision ( const FVector & desiredPos, const FVector & currentPos )
    {
    CActor * owner = GetOwnerActor ();
    if (!owner) return desiredPos;

    // Рейкаст от текущей позиции к желаемой
    auto result = COLLISION_SYSTEM.Raycast ( currentPos, desiredPos, "All" );

    if (result.bHit && result.HitComponent &&
         result.HitComponent->GetShapeType () == ECollisionShape::TERRAIN)
        {
            // Нашли террейн на пути - останавливаемся перед ним
        float safeDistance = std::max ( result.Distance - 0.01f, 0.0f );
        FVector safePos = currentPos + ( desiredPos - currentPos ).Normalized () * safeDistance;

        // Обнуляем скорость
        m_VerticalVelocity = 0.0f;

        return safePos;
        }

    return desiredPos;
    }