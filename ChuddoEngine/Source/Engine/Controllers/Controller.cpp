#include "Controllers/Controller.h"
#include "Pawns/Pawn.h"

CController::CController(const CObject* Owner, const std::string& inName) : CActor(Owner, inName)
{
}

CController::CController(const std::string& inName) : CActor(inName)
{
}

CController::~CController()
{
	UnPossess();
}

void CController::BeginPlay()
{
	CActor::BeginPlay();
}

void CController::Tick(float DeltaTime)
{
	CActor::Tick(DeltaTime);
}

CPawn* CController::GetControlledPawn() const
{
	if (ControlledPawn)
	{
		return ControlledPawn;
	}
	return nullptr;
}

void CController::SetControlledPawn(CPawn* NewPawn)
{
	if (NewPawn == nullptr)
	{
		UnPossess();
		return;
	}
	if (ControlledPawn == NewPawn)
	{
		return;
	}
	if (!NewPawn->CanBeControlled())
	{
		return;
	}
	if (ControlledPawn != nullptr)
	{
		CPawn* OldPawn = ControlledPawn;

		UnPossess();

		if (NewPawn->GetController() != nullptr)
		{
			if (NewPawn->GetController() != this)
			{
				NewPawn->GetController()->UnPossess();
			}
			else
			{
				ControlledPawn = OldPawn;
				OldPawn->SetController(this);
				OldPawn->OnPossessed();
				return;
			}
		}
		ControlledPawn = NewPawn;
		NewPawn->SetController(this);
		NewPawn->OnPossessed();

		if (ControlledPawn == nullptr)
		{

			if (OldPawn && OldPawn->CanBeControlled())
			{
				ControlledPawn = OldPawn;

				CController* oldController = OldPawn->GetController();
				if (oldController && oldController != this)
				{
					oldController->UnPossess();
				}

				OldPawn->SetController(this);
				OldPawn->OnPossessed();
			}
		}
	}
	else
	{
		ControlledPawn = NewPawn;
		NewPawn->SetController(this);
		NewPawn->OnPossessed();
	}
}

bool CController::Possess(CPawn* PawnToPossess)
{
	if (!PawnToPossess || !PawnToPossess->CanBeControlled())
		return false;	
	UnPossess();	
	ControlledPawn = PawnToPossess;
	PawnToPossess->SetController(this);	
	PawnToPossess->OnPossessed();
	return true;
}

void CController::UnPossess()
{
	if (ControlledPawn)
	{
		CPawn* OldPawn = ControlledPawn;
		ControlledPawn = nullptr;
		OldPawn->OnUnPossessed();
		OldPawn->SetController(nullptr);
	}
}
