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
         void OnComponentBeginOverlap ( CBaseCollisionComponent * other ) override;
         void OnComponentEndOverlap ( CBaseCollisionComponent * other ) override;
         void OnComponentHit ( CBaseCollisionComponent * other )override;

    protected:
        void SetupPlayerInputComponent ( CInputComponent * InputComponent ) override;
        void MoveRight ( float value );
        void MoveForward ( float Value );
        void MoveUp ( float Value );
        void Jump ();
        void LookUp ( float value );
        void Turn ( float value );
        void DebugInfo ( float dt ) override;
        CStaticMeshComponent * Mesh = nullptr;
        CCapsuleComponent * Capsule = nullptr;
        CCameraComponent * Camera = nullptr;
        CTerrainMeshComponent * terrainMesh = nullptr;
    private:
        void CreateCharacterMovementComponent ();
        FQuat StartRotation = FQuat::Identity ();
    };

REGISTER_CLASS_FACTORY ( CCharacter );