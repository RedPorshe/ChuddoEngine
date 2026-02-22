#include "Actors/Pawn.h"
#include "Actors/PlayerController.h"
#include "Components/TransformComponent.h"
#include "Components/GravityComponent.h"
#include "Components/InputComponent.h"
#include "Utils/Math/Quaternion.h"

CPawn::CPawn ( CObject * inOwner, const std::string & inDisplayName )
	: Super ( inOwner, inDisplayName )
	{
	m_InputComponent = AddDefaultSubObject<CInputComponent> ( "InputComponent_" + GetName () );
	LOG_DEBUG ( "[PAWN] Created: ", GetName () );
	}

CPawn::~CPawn ()
	{
	if (Controller)
		{
		Controller->Unpossess ();
		Controller = nullptr;
		}
	}

void CPawn::SetController ( CPlayerController * NewController )
	{
	Controller = NewController;
	}



void CPawn::AddMovementInput ( const FVector & WorldDirection, float ScaleValue )
	{
	if (!bInputEnabled) return;


	 // Опционально: нормализуем направление, если это ещё не сделано
	FVector NormalizedDirection = WorldDirection;
	if (!NormalizedDirection.IsZero ())
		{
		NormalizedDirection.Normalize ();
		}

	FVector Movement = NormalizedDirection * ScaleValue;

	CActor::MoveActor ( Movement );
	}

bool CPawn::IsInputEnabled () const
	{
	if (Controller)
		{
		if (Controller->GetPawn () == this)
			{
			return bInputEnabled;
			}
		}
	return false;
	}

void CPawn::ProcessPlayerInput ( float DeltaTime )
	{
	if (!Controller || !bInputEnabled)
		{
		LOG_WARN ( "NO CONTROLLER OR INPUT DISABLED" );
		return;
		}
	Controller->ProcessPlayerInput ( DeltaTime );
	}

void CPawn::Jump ( float val )
	{
	( void ) val;
	LOG_DEBUG ( " Jump value =", val );
	LOG_DEBUG ( "Check Pawn location." );
	LOG_DEBUG ( this->GetActorLocation (), " for actor" );
	LOG_DEBUG ( this->GetActorRotation (), " actor rotation FVector" );
	LOG_DEBUG ( this->GetActorRotationQuat (), " actor rotation FQuat" );
	}

void CPawn::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );

	if (GetController () != nullptr)
		{
		ProcessPlayerInput ( DeltaTime );
		}
	}

void CPawn::BeginPlay ()
	{
	Super::BeginPlay ();
	LOG_DEBUG ( "[PAWN] BeginPlay: ", GetName () );
	}

void CPawn::EndPlay ()
	{
	Super::EndPlay ();
	LOG_DEBUG ( "[PAWN] EndPlay: ", GetName () );
	for (auto comp : ActorComponents)
		{
		if (comp == m_InputComponent)
			{
			m_InputComponent->OnEndPlay ();
			}
		}
	}

void CPawn::OnPossessed ( CPlayerController * NewController )
	{
	if (NewController == nullptr) return;
	SetController ( NewController );

	}

void CPawn::OnUnpossessed ( CPlayerController * OldController )
	{
	if (OldController == nullptr) return;
	if (Controller == OldController)
		{
		SetController ( nullptr );
		}
	}

void CPawn::OnPossess ()
	{
	if (m_InputComponent)
		SetupPlayerInputComponent ( m_InputComponent );
	}

#include <GLFW/glfw3.h>
void CPawn::SetupPlayerInputComponent ( CInputComponent * InputComponent )
	{
	LOG_DEBUG ( "[PAWN] SetupPlayerInputComponent for: ", GetName () );

	if (InputComponent)
		{
		m_InputComponent = InputComponent;
		}
	}