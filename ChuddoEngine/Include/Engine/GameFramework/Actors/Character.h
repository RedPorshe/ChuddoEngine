#pragma once
#include "Actors/Pawn.h"

class CCapsuleComponent;
class CCameraComponent;


class CCharacter : public CPawn
	{
	CHUDDO_DECLARE_CLASS ( CCharacter, CPawn );
	public:
		CCharacter ( CObject * inOwner, const std::string & DisplayName );
		virtual ~CCharacter () = default;
		void BeginPlay () override ;
		void Tick ( float DeltaTime ) override ;
		void EndPlay () override ;
	protected:
		void SetupPlayerInputComponent ( CInputComponent * InputComponent ) override ;
		CStaticMeshComponent * Mesh = nullptr; // TODO: change to SkeletalMesh
		CCapsuleComponent * Capsule = nullptr;
		CCameraComponent * Camera = nullptr;

		float MoveSpeed = 600.f;
		float JumpForce = 500.0f;
		void MoveRight ( float value );
		void MoveForward ( float Value );
		void MoveUp ( float Value );
		void Jump ();
	};
REGISTER_CLASS_FACTORY ( CCharacter );