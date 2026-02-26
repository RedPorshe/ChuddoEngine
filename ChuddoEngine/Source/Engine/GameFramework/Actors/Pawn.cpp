#include "Actors/Pawn.h"
#include "Actors/PlayerController.h"
#include "Components/TransformComponent.h"
#include "Components/GravityComponent.h"
#include "Components/InputComponent.h"
#include "Components/Meshes/StaticMeshComponent.h"
#include "Utils/Math/Quaternion.h"
#include "World/World.h"
#include "World/Level.h"

CPawn::CPawn ( CObject * inOwner, const std::string & inDisplayName )
	: Super ( inOwner, inDisplayName )
	{
	m_InputComponent = AddDefaultSubObject<CInputComponent> ( "InputComponent_" + GetName () );
	m_InputComponent->AttachComponentToComponent ( RootComponent );	
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

	float DeltaTime = GetWorld ()->GetDeltaSeconds ();
	LOG_DEBUG ( "AddMovementInput DeltaTime: ", DeltaTime );
	FVector NormalizedDirection = WorldDirection;
	if (!NormalizedDirection.IsZero ())
		{
		NormalizedDirection.Normalize ();
		}

	FVector Movement = NormalizedDirection * ScaleValue * DeltaTime;
	LOG_DEBUG ( "Movement delta:", Movement );
	MoveActor ( Movement );
	
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
#include "Camera/CameraComponent.h"
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