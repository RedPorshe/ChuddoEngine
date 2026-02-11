#pragma once

#include "Actor.h"

class CInputComponent;
class CCameraComponent;


class CPawn : public CActor
	{
	CHUDDO_DECLARE_CLASS ( CPawn, CActor );
	public:
		CPawn ( CObject * inOwner = nullptr, const std::string & inDisplayName = "Pawn");
		virtual ~CPawn ();
		virtual void Tick ( float DeltaTime ) override;
		virtual void BeginPlay () override;
		virtual void EndPlay () override;
		virtual void SetupInputComponent ( CInputComponent * InputComponent);
		CInputComponent * GetInputComponent () const { return m_InputComponent; }

		void AddMovementInput ( FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false );
		void AddControllerYawInput ( float Val );
		void AddControllerRollInput ( float Val );
		void AddControllerPitchInput ( float Val );
		virtual void OnPosses ();
		void PossesedBy (/*controller*/ );
		virtual void OnUnPosses ();
		void UnPosses (/*Controller*/ );
		void SetInputEnabled ( bool bEnabled ) { bInputEnabled = bEnabled; }
		bool IsInputEnabled () const { return bInputEnabled; }

		void SetMoveSpeed ( float Speed ) { m_MoveSpeed = Speed; }
		void SetTurnSpeed ( float Speed ) { m_TurnSpeed = Speed; }
		void SetLookUpSpeed ( float Speed ) { m_LookUpSpeed = Speed; }
		void SetCamera ( CCameraComponent * Camera ) { m_CameraComponent = Camera; }
		bool IsMoving () { return bIsMovin; }
	protected:
		CInputComponent * m_InputComponent = nullptr;
		void CreateInputComponent ();
		
		  // Movement parameters - добавить
		float m_MoveSpeed = 600.0f;     // units per second
		float m_TurnSpeed = 90.0f;      // degrees per second
		float m_LookUpSpeed = 60.0f;    // degrees per second

		bool bUseControlRotation = true;
		bool bInputEnabled = true;
		CCameraComponent * m_CameraComponent = nullptr;

	};

REGISTER_CLASS_FACTORY ( CPawn );