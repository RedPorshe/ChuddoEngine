#include "Actors/Pawn.h"
#include "Actors/PlayerController.h"
#include "Components/TransformComponent.h"
#include "Utils/Math/Quaternion.h"

CPawn::CPawn ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {
    LOG_DEBUG ( "[PAWN] Created: ", GetName () );
    }

CPawn::~CPawn ()
    {
    if (Controller)
        {
        Controller->Unpossess ();
        Controller = nullptr;
        }
    }

void CPawn::SetController ( CPlayerController * NewController )
    {
    Controller = NewController;
    }

void CPawn::SetupPlayerInputComponent ()
    {
    LOG_DEBUG ( "[PAWN] SetupPlayerInputComponent for: ", GetName () );
    }

void CPawn::AddMovementInput ( const FVector & WorldDirection, float ScaleValue )
    {
    if (!bInputEnabled) return;

    FVector NormalizedDir = WorldDirection;
    NormalizedDir.Normalize ();
    PendingMovementInput = NormalizedDir * ScaleValue;

    // Разбиваем на компоненты для движения
    ForwardInput = PendingMovementInput.Dot ( GetActorForwardVector () );
    RightInput = PendingMovementInput.Dot ( GetActorRightVector () );
    UpInput = PendingMovementInput.Dot ( GetActorUpVector () );
    }

void CPawn::ProcessPlayerInput ( float DeltaTime )
    {
    if (!Controller || !bInputEnabled)
        return;

    // Apply movement based on input
    if (!CEMath::IsZero ( ForwardInput ) || !CEMath::IsZero ( RightInput ) || !CEMath::IsZero ( UpInput ))
        {
        FQuat Rotation = GetActorRotationQuat ();

        FVector Forward = Rotation * FVector::Forward ();
        FVector Right = Rotation * FVector::Right ();
        FVector Up = Rotation * FVector::Up ();

        FVector DesiredMovement = Forward * ForwardInput + Right * RightInput + Up * UpInput;
        DesiredMovement.Normalize ();
        DesiredMovement *= MaxSpeed;

        // Simple acceleration model
        Velocity = Velocity + ( DesiredMovement - Velocity ) * Acceleration * DeltaTime;

        // Clamp to max speed
        if (Velocity.Length () > MaxSpeed)
            {
            Velocity = Velocity.Normalized () * MaxSpeed;
            }

            // Apply movement
        FVector NewLocation = GetActorLocation () + Velocity * DeltaTime;
        SetActorLocation ( NewLocation );
        }
    else
        {
            // Deceleration
        if (Velocity.Length () > 0.01f)
            {
            Velocity = Velocity - Velocity.Normalized () * Deceleration * DeltaTime;
            if (Velocity.Length () < 0.01f)
                {
                Velocity = FVector::Zero ();
                }
            else
                {
                FVector NewLocation = GetActorLocation () + Velocity * DeltaTime;
                SetActorLocation ( NewLocation );
                }
            }
        }

        // Apply rotation - ИСПРАВЛЕНО: конвертируем FVector в FQuat
    if (!CEMath::IsZero ( TurnInput ))
        {
            // Создаем кватернион вращения вокруг оси Y (yaw)
        FQuat YawRotation ( FVector::Up (), CEMath::DegreesToRadians ( TurnInput * TurnRate * DeltaTime ) );
        AddActorWorldRotation ( YawRotation, true );
        }

    if (!CEMath::IsZero ( LookUpInput ))
        {
            // Создаем кватернион вращения вокруг оси X (pitch) в локальном пространстве
        FQuat PitchRotation ( FVector::Right (), CEMath::DegreesToRadians ( LookUpInput * TurnRate * DeltaTime ) );
        AddActorLocalRotation ( PitchRotation, true );
        }

        // Reset input for next frame
    ForwardInput = 0.0f;
    RightInput = 0.0f;
    UpInput = 0.0f;
    TurnInput = 0.0f;
    LookUpInput = 0.0f;
    PendingMovementInput = FVector::Zero ();
    }

void CPawn::MoveForward ( float Value )
    {
    ForwardInput = CEMath::Clamp ( Value, -1.0f, 1.0f );
    }

void CPawn::MoveRight ( float Value )
    {
    RightInput = CEMath::Clamp ( Value, -1.0f, 1.0f );
    }

void CPawn::MoveUp ( float Value )
    {
    UpInput = CEMath::Clamp ( Value, -1.0f, 1.0f );
    }

void CPawn::Turn ( float Value )
    {
    TurnInput = CEMath::Clamp ( Value, -1.0f, 1.0f );
    }

void CPawn::LookUp ( float Value )
    {
    LookUpInput = CEMath::Clamp ( Value, -1.0f, 1.0f );
    }

void CPawn::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    ProcessPlayerInput ( DeltaTime );
    }

void CPawn::BeginPlay ()
    {
    Super::BeginPlay ();
    LOG_DEBUG ( "[PAWN] BeginPlay: ", GetName () );
    }

void CPawn::EndPlay ()
    {
    Super::EndPlay ();
    LOG_DEBUG ( "[PAWN] EndPlay: ", GetName () );
    }

