#include "Actors/Pawn.h"
#include "Actors/PlayerController.h"
#include "Components/TransformComponent.h"
#include "Components/GravityComponent.h"
#include "Components/MovementComponent.h"
#include "Components/InputComponent.h"
#include "World/World.h"

CPawn::CPawn ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {
    m_InputComponent = AddDefaultSubObject<CInputComponent> ( "InputComponent_" + GetName () );
   
    SetMovableState ( EMovableState::DYNAMIC );
    }

CPawn::~CPawn ()
    {
    if (Controller)
        {
        Controller->Unpossess ();
        Controller = nullptr;
        }
    }

void CPawn::SetController ( CController * NewController )
    {
    Controller = NewController;
    }

void CPawn::AddMovementInput ( const FVector & WorldDirection, float ScaleValue, bool bForce )
    {
    if (MovementComponent && IsInputEnabled ())
        {
        MovementComponent->AddInputVector ( WorldDirection, ScaleValue, bForce );
        }
    }

void CPawn::AddControllerYawInput ( float Val )
    {
    if (MovementComponent && IsInputEnabled ())
        {
        MovementComponent->AddYawInput ( Val );
        }
    }

void CPawn::AddControllerPitchInput ( float Val )
    {
    if (MovementComponent && IsInputEnabled ())
        {
        MovementComponent->AddPitchInput ( Val );
        }
    }

void CPawn::AddControllerRollInput ( float Val )
    {
    if (MovementComponent && IsInputEnabled ())
        {
        MovementComponent->AddRollInput ( Val );
        }
    }

bool CPawn::IsInputEnabled () const
    {
    return bInputEnabled && Controller && Controller->GetPawn () == this;
    }

void CPawn::ProcessPlayerInput ( float DeltaTime )
    {
    if (!Controller || !bInputEnabled) return;
    Controller->ProcessPlayerInput ( DeltaTime );
    }

void CPawn::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );

    if (Controller)
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

    if (m_InputComponent)
        {
        m_InputComponent->OnEndPlay ();
        }
    }

void CPawn::OnPossessed ( CController * NewController )
    {
    if (!NewController) return;
    SetController ( NewController );
    }

void CPawn::OnUnpossessed ( CController * OldController )
    {
    if (OldController && Controller == OldController)
        {
        SetController ( nullptr );
        }
    }

void CPawn::OnPossess ()
    {
    if (m_InputComponent)
        {
        SetupPlayerInputComponent ( m_InputComponent );
        }
    }

void CPawn::SetupPlayerInputComponent ( CInputComponent * InputComponent )
    {
    LOG_DEBUG ( "[PAWN] SetupPlayerInputComponent for: ", GetName () );
    m_InputComponent = InputComponent;
    }

void CPawn::SetAirControl ( float value )
    {
    if (MovementComponent)
        {
        MovementComponent->SetAirControl ( value );
        }
    }

float CPawn::GetAirControl () const
    {
    return MovementComponent ? MovementComponent->GetAirControl () : 0.0f;
    }

void CPawn::SetMaxAirSpeed ( float value )
    {
    if (MovementComponent)
        {
        MovementComponent->SetMaxAirSpeed ( value );
        }
    }

float CPawn::GetMaxAirSpeed () const
    {
    return MovementComponent ? MovementComponent->GetMaxAirSpeed () : 0.0f;
    }

void CPawn::SetGroundSpeed ( float value )
    {
    if (MovementComponent)
        {
        MovementComponent->SetMaxWalkSpeed ( value );
        }
    }

float CPawn::GetGroundSpeed () const
    {
    return MovementComponent ? MovementComponent->GetMaxWalkSpeed () : 0.0f;
    }