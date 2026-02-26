#pragma once

#include "Actors/Actor.h"

class CPlayerController;
class CInputComponent;
class CStaticMeshComponent;

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



		void AddMovementInput ( const FVector & WorldDirection, float ScaleValue = 1.0f );
		bool IsInputEnabled () const;

		  // ========== ACTOR OVERRIDES ==========
		virtual void Tick ( float DeltaTime ) override;
		virtual void BeginPlay () override;
		virtual void EndPlay () override;

		void OnPossessed ( CPlayerController * NewController );
		void OnUnpossessed ( CPlayerController * OldController );
		virtual void OnPossess ();
		CInputComponent * GetInputComponent () const { return m_InputComponent; }
		void SetInputEnabled ( bool value ) { bInputEnabled = value; }
	protected:
		virtual void SetupPlayerInputComponent ( CInputComponent * InputComponent );
		void ProcessPlayerInput ( float DeltaTime );
		CPlayerController * Controller = nullptr;
		CInputComponent * m_InputComponent = nullptr;
		// Input state
		bool bInputEnabled = false;


	};

REGISTER_CLASS_FACTORY ( CPawn );