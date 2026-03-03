#pragma once
#include "Actors/Pawn.h"

class CCapsuleComponent;
class CStaticMeshComponent;
class CCameraComponent;
class CTerrainMeshComponent;
class CCharacter : public CPawn
    {
    CHUDDO_DECLARE_CLASS ( CCharacter, CPawn );

    public:
        CCharacter ( CObject * inOwner, const std::string & DisplayName );
        virtual ~CCharacter () = default;

        void BeginPlay () override;
        void Tick ( float DeltaTime ) override;
        void EndPlay () override;

        void SpawnCube ();
        bool IsJumping () const;

    protected:
        void SetupPlayerInputComponent ( CInputComponent * InputComponent ) override;
        void MoveRight ( float value );
        void MoveForward ( float Value );
        void MoveUp ( float Value );
        void Jump ();

        CStaticMeshComponent * Mesh = nullptr;
        CCapsuleComponent * Capsule = nullptr;
        CCameraComponent * Camera = nullptr;
        CTerrainMeshComponent * terrainMesh = nullptr;
    private:
        void CreateCharacterMovementComponent ();
    };

REGISTER_CLASS_FACTORY ( CCharacter );