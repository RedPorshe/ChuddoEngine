#include "Actors/Pawn.h"
#include "Actors/PlayerController.h"
#include "Components/TransformComponent.h"
#include "Components/InputComponent.h"
#include "Utils/Math/Quaternion.h"

CPawn::CPawn ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {
    m_InputComponent = AddDefaultSubObject<CInputComponent> ( "InputComponent_"+GetName() );
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
   
    MoveActor( Movement,false );
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

void CPawn::MoveForward ( float axis )
    {
    LOG_DEBUG ( "Current location : ", GetActorLocation () );
    if (axis != 0.0f)
        {           
        FVector Forward = GetActorForwardVector ();

        FVector NewLocation = GetActorLocation () + Forward * 100.0f; // 100.0f - скорость движения
        
        AddMovementInput ( NewLocation, axis );
        }
    }

#include <GLFW/glfw3.h>
void CPawn::SetupPlayerInputComponent ( CInputComponent * InputComponent )
    {
    LOG_DEBUG ( "[PAWN] SetupPlayerInputComponent for: ", GetName () );

    if (InputComponent)
        {
       
                                     // Оси движения
        InputComponent->BindAxis ( "MoveForward", GLFW_KEY_W, GLFW_KEY_S,
                                   [ this ] ( float value )
                                   {
                                   if (value != 0.0f)
                                       {
                                       MoveForward ( value );
                                       }
                                   } );

        m_InputComponent = InputComponent;
        }
    }