#pragma once

#include "Actors/Actor.h"

class CPlayerController;
class CInputComponent;
class CStaticMeshComponent;
class CMovementComponent;

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
		CMovementComponent * GetMovementComponent () const { return MovementComponent; }
		void SetMovementComponent ( CMovementComponent * NewMovementComponent ) { MovementComponent = NewMovementComponent; }
// Input API (как в UE)
		void AddMovementInput ( const FVector & WorldDirection, float ScaleValue );
		void AddMovementInput ( const FVector & WorldDirection, float ScaleValue, bool bForce = false );
		void AddControllerYawInput ( float Val );
		void AddControllerPitchInput ( float Val );
		void AddControllerRollInput ( float Val );
		bool IsInputEnabled () const;

		// ========== MOVEMENT SETTINGS ==========
		void SetAirControl ( float value ) { m_AirControl = value; }
		float GetAirControl () const { return m_AirControl; }

		void SetMaxAirSpeed ( float value ) { m_MaxAirSpeed = value; }
		float GetMaxAirSpeed () const { return m_MaxAirSpeed; }

		void SetGroundSpeed ( float value ) { m_GroundSpeed = value; }
		float GetGroundSpeed () const { return m_GroundSpeed; }

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
		CMovementComponent * MovementComponent = nullptr;
		CPlayerController * Controller = nullptr;
		CInputComponent * m_InputComponent = nullptr;

		// Input state
		bool bInputEnabled = false;

		// Movement settings
		float m_AirControl = 0.3f;      // Коэффициент управления в воздухе (30% от наземного)
		float m_MaxAirSpeed = 300.0f;    // Максимальная скорость в воздухе
		float m_GroundSpeed = 600.0f;     // Скорость на земле
		FVector m_Velocity = FVector::Zero (); // Текущая скорость персонажа
	};

REGISTER_CLASS_FACTORY ( CPawn );