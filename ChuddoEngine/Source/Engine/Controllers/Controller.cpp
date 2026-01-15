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
	if (ControlledPawn != NewPawn)
	{
		UnPossess();
		if (NewPawn && NewPawn->CanBeControlled())
		{
			ControlledPawn = NewPawn;
			NewPawn->SetController(this);
			NewPawn->OnPossessed();
		}
	}
}

bool CController::Possess(CPawn* PawnToPossess)
{
	if (!PawnToPossess || !PawnToPossess->CanBeControlled())
		return false;

	// Отписываемся от текущего павна
	UnPossess();

	// Устанавливаем двустороннюю связь
	ControlledPawn = PawnToPossess;
	PawnToPossess->SetController(this);

	// Вызываем соответствующие события
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
