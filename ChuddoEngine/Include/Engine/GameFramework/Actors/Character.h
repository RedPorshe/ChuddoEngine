#pragma once
#include "Actors/Pawn.h"

class CCapsuleComponent;
class CStaticMeshComponent;
class CCameraComponent;

class CCharacter : public CPawn
	{
	CHUDDO_DECLARE_CLASS ( CCharacter, CPawn );
	public:
		CCharacter ( CObject * inOwner, const std::string & DisplayName );
		virtual ~CCharacter () = default;
		void BeginPlay () override;
		void Tick ( float DeltaTime ) override;
		void EndPlay () override;

		void StartJump ();
		void EndJump ();
		void SpawnCube ();
	protected:
		void SetupPlayerInputComponent ( CInputComponent * InputComponent ) override;
		void MoveRight ( float value );
		void MoveForward ( float Value );
		void MoveUp ( float Value );
		void Jump ();
		
		bool CheckTargetJump ();
		CStaticMeshComponent * Mesh = nullptr; // TODO: change to SkeletalMesh
		CCapsuleComponent * Capsule = nullptr;
		CCameraComponent * Camera = nullptr;

		float MoveSpeed = 600.f;		
		float TargetJumpHeight = 0.f;
		float InitialJumpHeight = 0.f;
		float JumpHeight = 40.f;
		bool bIsJumping = false;
	};

REGISTER_CLASS_FACTORY ( CCharacter );