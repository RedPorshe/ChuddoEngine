#pragma once

#include "Actors/Actor.h"

class CPlayerController;
class CInputComponent;
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
		virtual void SetupPlayerInputComponent ( const CInputComponent & InputComponent );
		virtual void ProcessPlayerInput ( float DeltaTime );



		void AddMovementInput ( const FVector & WorldDirection, float ScaleValue = 1.0f );
		bool IsInputEnabled () const;

		  // ========== ACTOR OVERRIDES ==========
		virtual void Tick ( float DeltaTime ) override;
		virtual void BeginPlay () override;
		virtual void EndPlay () override;

		virtual void OnPossessed ( CPlayerController * NewController );
		virtual void OnUnpossessed ( CPlayerController * OldController );
		void OnPossess ();
		CInputComponent* GetInputComponent () const { return m_InputComponent; }
		void SetInputComponent ( CInputComponent* NewInputComponent ) { m_InputComponent = NewInputComponent; }

	protected:
		CPlayerController * Controller = nullptr;
		CInputComponent * m_InputComponent = nullptr;
		// Input state
		bool bInputEnabled = true;

		void testfuncinput ( float axis );

	};

REGISTER_CLASS_FACTORY ( CPawn );