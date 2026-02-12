#pragma once

#include "Actors/Actor.h"

class CPlayerController;

class CPawn : public CActor
    {
    CHUDDO_DECLARE_CLASS ( CPawn, CActor );

    public:
        CPawn ( CObject * inOwner = nullptr, const std::string & inDisplayName = "Pawn" );
        virtual ~CPawn ();

        // ========== CONTROLLER ==========
        void SetController ( CPlayerController * NewController );
        CPlayerController * GetController () const { return Controller; }
        bool IsPlayerControlled () const { return Controller != nullptr; }

        // ========== INPUT ==========
        virtual void SetupPlayerInputComponent ();
        virtual void ProcessPlayerInput ( float DeltaTime );

        // ========== MOVEMENT METHODS FOR TESTS ==========
        void SetInputEnabled ( bool bEnabled ) { bInputEnabled = bEnabled; }
        bool IsInputEnabled () const { return bInputEnabled; }

        void SetMoveSpeed ( float Speed ) { MaxSpeed = Speed; }
        float GetMoveSpeed () const { return MaxSpeed; }

        void AddMovementInput ( const FVector & WorldDirection, float ScaleValue = 1.0f );
        bool IsMoving () const { return !Velocity.IsZero (); }

        // ========== MOVEMENT ==========
        virtual void MoveForward ( float Value );
        virtual void MoveRight ( float Value );
        virtual void MoveUp ( float Value );
        virtual void Turn ( float Value );
        virtual void LookUp ( float Value );

        // ========== ACTOR OVERRIDES ==========
        virtual void Tick ( float DeltaTime ) override;
        virtual void BeginPlay () override;
        virtual void EndPlay () override;

      

    protected:
        CPlayerController * Controller = nullptr;

        // Input state
        bool bInputEnabled = true;

        // Movement state
        FVector Velocity;
        FVector PendingMovementInput; // Накопленный ввод за кадр
        float MaxSpeed = 600.0f;
        float Acceleration = 1000.0f;
        float Deceleration = 1500.0f;
        float TurnRate = 90.0f; // Degrees per second

        // Input values (accumulated per frame)
        float ForwardInput = 0.0f;
        float RightInput = 0.0f;
        float UpInput = 0.0f;
        float TurnInput = 0.0f;
        float LookUpInput = 0.0f;
    };

REGISTER_CLASS_FACTORY ( CPawn );