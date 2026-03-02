#include "Components/MovementComponent.h"
#include "Actors/Pawn.h"
#include "Components/GravityComponent.h"
#include "Utils/Math/CE_MathHelpers.h"

CMovementComponent::CMovementComponent ( CObject * inOwner, const std::string & inName )
    : Super ( inOwner, inName )
    , MovementInputAccumulator ( FVector::Zero () )
    , YawInputAccumulator ( 0.0f )
    , PitchInputAccumulator ( 0.0f )
    , RollInputAccumulator ( 0.0f )
    , bHasMovementInput ( false )
    , bHasRotationInput ( false )
    {}

void CMovementComponent::InitComponent ()
    {
    Super::InitComponent ();
    }

void CMovementComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );

    if (!OwnerPawn) return;

    // Обновляем состояние на земле из GravityComponent
    if (auto * Gravity = OwnerPawn->GetGravityComponent ())
        {
        bIsGrounded = Gravity->IsGrounded ();
        }

        // Обрабатываем накопленный ввод
    ProcessMovementInput ( DeltaTime );
    ProcessRotationInput ( DeltaTime );

    // Применяем скорость
    ApplyVelocity ( DeltaTime );
    }

void CMovementComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();
    }

void CMovementComponent::AddInputVector ( const FVector & WorldDirection, float ScaleValue, bool bForce )
    {
    FVector Input = WorldDirection * ScaleValue;

    if (bForce)
        {
        MovementInputAccumulator = Input;
        }
    else
        {
        MovementInputAccumulator += Input;
        }

    bHasMovementInput = true;
    }

void CMovementComponent::AddPitchInput ( float Value )
    {
    PitchInputAccumulator += Value;
    bHasRotationInput = true;
    }

void CMovementComponent::AddYawInput ( float Value )
    {
    YawInputAccumulator += Value;
    bHasRotationInput = true;
    }

void CMovementComponent::AddRollInput ( float Value )
    {
    RollInputAccumulator += Value;
    bHasRotationInput = true;
    }

void CMovementComponent::ProcessMovementInput ( float DeltaTime )
    {
    if (!bHasMovementInput || !OwnerPawn) return;

    // Нормализуем направление, если нужно
    FVector InputDirection = MovementInputAccumulator;
    float InputMagnitude = InputDirection.Length ();

    if (InputMagnitude > 1.0f)
        {
        InputDirection.Normalize ();
        InputMagnitude = 1.0f;
        }

        // Вычисляем желаемую скорость
    float CurrentMaxSpeed = bIsGrounded ? MaxWalkSpeed : MaxAirSpeed;
    float CurrentControl = bIsGrounded ? 1.0f : AirControl;

    FVector DesiredVelocity = InputDirection * CurrentMaxSpeed * InputMagnitude * CurrentControl;

    // Плавное изменение скорости
    float Smoothness = bIsGrounded ? 8.0f : 4.0f;
    m_Velocity = FVector::Lerp ( m_Velocity, DesiredVelocity, Smoothness * DeltaTime );

    // Сбрасываем аккумулятор
    MovementInputAccumulator = FVector::Zero ();
    bHasMovementInput = false;
    }

void CMovementComponent::ProcessRotationInput ( float DeltaTime )
    {
    if (!bHasRotationInput || !OwnerPawn) return;

    // Применяем вращение
    if (YawInputAccumulator != 0.0f)
        {
        FQuat YawRotation ( FVector::Up (), YawInputAccumulator );
        OwnerPawn->AddActorWorldRotation ( YawRotation, true );
        }

    if (PitchInputAccumulator != 0.0f)
        {
            // Pitch обычно применяется к камере, не к самому pawn
            // Здесь можно вызвать метод камеры или накопить для другого компонента
        }

    if (RollInputAccumulator != 0.0f)
        {
        FQuat RollRotation ( FVector::Forward (), RollInputAccumulator );
        OwnerPawn->AddActorWorldRotation ( RollRotation, true );
        }

        // Сбрасываем аккумуляторы
    YawInputAccumulator = 0.0f;
    PitchInputAccumulator = 0.0f;
    RollInputAccumulator = 0.0f;
    bHasRotationInput = false;
    }

void CMovementComponent::ApplyVelocity ( float DeltaTime )
    {
    if (!OwnerPawn || m_Velocity.IsZero ()) return;

    // Применяем движение
    OwnerPawn->MoveActor ( m_Velocity * DeltaTime );

    // Торможение (если нет ввода)
    if (!bHasMovementInput)
        {
        float Braking = BrakingDeceleration * DeltaTime;
        m_Velocity = m_Velocity * ( 1.0f - Braking );

        if (m_Velocity.LengthSquared () < 1.0f)
            {
            m_Velocity = FVector::Zero ();
            }
        }
    }