#include "Actors/Pawn.h"
#include "Actors/PlayerController.h"
#include "Components/TransformComponent.h"
#include "Components/InputComponent.h"
#include "Utils/Math/Quaternion.h"

CPawn::CPawn ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {
    m_InputComponent = AddDefaultSubObject<CInputComponent> ( "InputComponent" );
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
    FVector Movement = WorldDirection * ScaleValue;
    MoveActor(Movement);
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
        return;
    //stub implementation need check buttons
    static int warncount = 0;
    if (warncount < 1)
        {
        LOG_WARN ( "stub implementation ProcessPlayerInput ( float DeltaTime ) need check buttons" );
        warncount++;
        }
    
    }
 
void CPawn::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    ProcessPlayerInput ( DeltaTime );
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
            m_InputComponent->onEndPlay ();
            }
        }
    }

void CPawn::OnPossessed ( CPlayerController * NewController )
    {
    if (NewController == nullptr) return;
    SetController ( NewController );
    SetupPlayerInputComponent (m_InputComponent);
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
    if(m_InputComponent)
    SetupPlayerInputComponent ( m_InputComponent );
    }

void CPawn::testfuncinput ( float axis )
    {
    LOG_DEBUG ( "testfuncinput called with axis value: ", axis );
    }

#include <GLFW/glfw3.h>
void CPawn::SetupPlayerInputComponent ( const CInputComponent & InputComponent )
    {
    m_InputComponent = const_cast< CInputComponent * >( &InputComponent );
    LOG_DEBUG ( "[PAWN] SetupPlayerInputComponent for: ", GetName () );
    if (m_InputComponent)
        {
        m_InputComponent->BindAction ( "MoveForward", GLFW_KEY_W, [ this ] ( float value )
                                       {
                                       testfuncinput(value);
                                       } );
        }
    }