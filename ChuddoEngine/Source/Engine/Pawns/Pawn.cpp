#include "Pawns/Pawn.h"
#include "Controllers/Controller.h"

CPawn::CPawn(const CObject* Owner, const std::string& inName) : CActor(Owner, inName)
{
	std::cout << "Pawn '" << GetName() << "' created." << std::endl;
}

CPawn::CPawn(const std::string& inName) : CActor(inName)
{
	std::cout << "Pawn '" << GetName() << "' created without owner." << std::endl;
}

CPawn::~CPawn()
{
	std::cout << "Pawn '" << GetName() << "' destroyed." << std::endl;
}

void CPawn::BeginPlay()
{
	std::cout << "Begin Play on Pawn: " << GetName() << std::endl;
	
	// Call parent implementation
	CActor::BeginPlay();

	// Setup input if we have a controller
	if (HasController())
	{
		SetupPlayerInputComponent();
	}
}

void CPawn::Tick(float DeltaTime)
{
	std::cout << "Tick on Pawn: " << GetName() << " with DeltaTime: " << DeltaTime << std::endl;
	
	// Call parent implementation
	CActor::Tick(DeltaTime);

	// Process accumulated input
	if (HasController() && bCanBeControlled)
	{
		ProcessMovementInput(DeltaTime);
		ProcessRotationInput(DeltaTime);
	}

	// Reset input for next frame
	AccumulatedMovementInput = FVector::Zero();
	AccumulatedPitchInput = 0.0f;
	AccumulatedYawInput = 0.0f;
}



void CPawn::SetController(CController* NewController)
{
	if (Controller != NewController)
	{
		// Если у нас уже был контроллер, отписываемся
		if (Controller && Controller->GetControlledPawn() == this)
		{
			Controller->UnPossess();
		}

		Controller = NewController;

		if (Controller)
		{
			std::cout << "Pawn '" << GetName() << "' possessed by controller '"
				<< Controller->GetName() << "'." << std::endl;
			// Настраиваем ввод только если контроллер назначен
			if (bIsInitialized)
			{
				OnPossessed();
			}
		}
		else
		{
			std::cout << "Pawn '" << GetName() << "' unpossessed." << std::endl;
		}
	}
}

void CPawn::PossessedBy(CController* NewController)
{
	if (NewController)
	{
		NewController->Possess(this);
	}
}

void CPawn::UnPossessed()
{
	// Это вызывается из контроллера
	if (Controller)
	{
		Controller = nullptr;
	}
	OnUnPossessed();
}

void CPawn::OnPossessed()
{
	// Просто настраиваем ввод
	if (Controller && bIsInitialized)
	{
		SetupPlayerInputComponent();
	}
}

void CPawn::OnUnPossessed()
{
	// Очистка при необходимости
	std::cout << "Pawn '" << GetName() << "' was unpossessed." << std::endl;
}

void CPawn::SetupPlayerInputComponent()
{
	std::cout << "Setting up player input component for Pawn: " << GetName() << std::endl;
}

void CPawn::AddMovementInput(const FVector& InputDirection, float Scale)
{
	if (!bCanBeControlled || !HasController())
		return;

	AccumulatedMovementInput += InputDirection * Scale;
}

void CPawn::AddRotationInput(float Pitch, float Yaw)
{
	if (!bCanBeControlled || !HasController())
		return;

	AccumulatedPitchInput += Pitch;
	AccumulatedYawInput += Yaw;
}

void CPawn::ProcessMovementInput(float DeltaTime)
{
	if (!AccumulatedMovementInput.IsZero())
	{
		FVector Movement = AccumulatedMovementInput * DeltaTime;
		MoveActor(Movement);
		std::cout << "Pawn '" << GetName() << "' moved by " << Movement.x << ", " << Movement.y << ", " << Movement.z << std::endl;
	}
}

void CPawn::ProcessRotationInput(float DeltaTime)
{
	if (AccumulatedPitchInput != 0.0f || AccumulatedYawInput != 0.0f)
	{
		float PitchDelta = AccumulatedPitchInput * DeltaTime;
		float YawDelta = AccumulatedYawInput * DeltaTime;
		RotateActor(PitchDelta, YawDelta, 0.0f);
		std::cout << "Pawn '" << GetName() << "' rotated by Pitch: " << PitchDelta << ", Yaw: " << YawDelta << std::endl;
	}
}
