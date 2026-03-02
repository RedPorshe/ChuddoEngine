#pragma once

#include "Components/BaseComponent.h"
/*
 * @Brief: Base Movement component
 * Handles movement input and velocity for pawns
 */

class CPawn;

class CMovementComponent : public CBaseComponent
    {
    CHUDDO_DECLARE_CLASS ( CMovementComponent, CBaseComponent );

    public:
        CMovementComponent ( CObject * inOwner = nullptr, const std::string & inName = "MovementComponent" );
        virtual ~CMovementComponent () = default;

        void InitComponent () override;
        void Tick ( float DeltaTime ) override;
        void OnBeginPlay () override;

        // Input accumulation (как в UE)
        void AddInputVector ( const FVector & WorldDirection, float ScaleValue, bool bForce = false );
        void AddPitchInput ( float Value );     // В радианах
        void AddYawInput ( float Value );       // В радианах
        void AddRollInput ( float Value );      // В радианах

        // Вспомогательные методы для градусов (для удобства)
        void AddPitchInputDegrees ( float Degrees ) { AddPitchInput ( CEMath::DegreesToRadians ( Degrees ) ); }
        void AddYawInputDegrees ( float Degrees ) { AddYawInput ( CEMath::DegreesToRadians ( Degrees ) ); }
        void AddRollInputDegrees ( float Degrees ) { AddRollInput ( CEMath::DegreesToRadians ( Degrees ) ); }

        // Velocity management
        FVector GetVelocity () const { return m_Velocity; }
        void SetVelocity ( const FVector & NewVelocity ) { m_Velocity = NewVelocity; }

        // Owner access
        CPawn * GetOwnerPawn () const { return OwnerPawn; }
        void SetOwnerPawn ( CPawn * InPawn ) { OwnerPawn = InPawn; }

        // Movement settings
        void SetMaxWalkSpeed ( float Speed ) { MaxWalkSpeed = Speed; }
        float GetMaxWalkSpeed () const { return MaxWalkSpeed; }

        void SetMaxAirSpeed ( float Speed ) { MaxAirSpeed = Speed; }
        float GetMaxAirSpeed () const { return MaxAirSpeed; }

        void SetAirControl ( float Control ) { AirControl = Control; }
        float GetAirControl () const { return AirControl; }

        void SetBrakingDeceleration ( float Decel ) { BrakingDeceleration = Decel; }
        float GetBrakingDeceleration () const { return BrakingDeceleration; }

    protected:
        // Internal methods
        virtual void ProcessMovementInput ( float DeltaTime );
        virtual void ProcessRotationInput ( float DeltaTime );
        virtual void ApplyVelocity ( float DeltaTime );

        // Input accumulators
        FVector MovementInputAccumulator;
        float YawInputAccumulator = 0.0f;
        float PitchInputAccumulator = 0.0f;
        float RollInputAccumulator = 0.0f;

        bool bHasMovementInput = false;
        bool bHasRotationInput = false;

        // Movement settings
        float MaxWalkSpeed = 600.0f;
        float MaxAirSpeed = 400.0f;
        float AirControl = 0.3f;
        float BrakingDeceleration = 8.0f;

        // Current state
        FVector m_Velocity = FVector::Zero ();
        bool bIsGrounded = true;  // Будет обновляться из GravityComponent

        // Owner
        CPawn * OwnerPawn = nullptr;
    };

REGISTER_CLASS_FACTORY ( CMovementComponent );