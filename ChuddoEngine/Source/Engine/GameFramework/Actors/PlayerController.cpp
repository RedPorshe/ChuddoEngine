#include "Actors/PlayerController.h"
#include "Actors/Pawn.h"
#include "Actors/HUD.h"
#include "GameFramework/GameMode.h"
#include "Components/TransformComponent.h"
#include "Core/InputSystem.h"
#include "Components/Collisions/BaseCollisionComponent.h"
#include "Components/GravityComponent.h"

CPlayerController::CPlayerController ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {
    LOG_DEBUG ( "[PLAYERCONTROLLER] Created: ", GetName () );
    if (m_Gravity)
        {

        auto it = std::find ( ActorComponents.begin (), ActorComponents.end (), m_Gravity );
        if (it != ActorComponents.end ())
            {
            ActorComponents.erase ( it );
            }


        RemoveOwnedObject ( m_Gravity->GetName () );


        m_Gravity = nullptr;
        }
    }

CPlayerController::~CPlayerController ()
    {
    if (ControlledPawn)
        {
        Unpossess ();
        }
    m_HUD = nullptr;
    OwningGameMode = nullptr;
    ViewTarget = nullptr;
    }

void CPlayerController::Possess ( CPawn * PawnToPossess )
    {
    if (!PawnToPossess)
        {
        LOG_ERROR ( "[PLAYERCONTROLLER] Cannot possess null pawn!" );
        return;
        }

    if (ControlledPawn == PawnToPossess)
        {
        LOG_WARN ( "[PLAYERCONTROLLER] Already possessing pawn: ", PawnToPossess->GetName () );
        return;
        }

    if (ControlledPawn)
        {
        Unpossess ();
        }

    ControlledPawn = PawnToPossess;
    ControlledPawn->SetController ( this );
    ControlledPawn->OnPossessed ( this );
    ViewTarget = ControlledPawn;
    ControlledPawn->OnPossess ();
    LOG_DEBUG ( "[PLAYERCONTROLLER] Possessed pawn: ", ControlledPawn->GetName (),
                " for controller: ", GetName () );
    }

void CPlayerController::Unpossess ()
    {
    if (!ControlledPawn)
        return;

    LOG_DEBUG ( "[PLAYERCONTROLLER] Unpossessing pawn: ", ControlledPawn->GetName (),
                " from controller: ", GetName () );

    ControlledPawn->SetController ( nullptr );
    ControlledPawn = nullptr;
    ViewTarget = nullptr;
    }

CHUD * CPlayerController::SpawnHUD ( CHUD * InHUD )
    {
    if (!InHUD)
        {
        LOG_ERROR ( "[PLAYERCONTROLLER] Cannot spawn null HUD!" );
        return nullptr;
        }

    if (m_HUD)
        {
        LOG_WARN ( "[PLAYERCONTROLLER] Controller already has HUD: ", m_HUD->GetName (),
                   " - replacing with: ", InHUD->GetName () );
        }

    SetHUD ( InHUD );

    if (m_HUD)
        {
        m_HUD->SetOwnerController ( this );
        LOG_DEBUG ( "[PLAYERCONTROLLER] Spawned HUD: ", m_HUD->GetName () );
        }

    return m_HUD;
    }

void CPlayerController::SetHUD ( CHUD * inHUD )
    {
    if (inHUD == nullptr)
        {
        LOG_ERROR ( "[PLAYERCONTROLLER] Cannot set null HUD!" );
        return;
        }

    m_HUD = inHUD;
    m_HUD->SetOwnerController ( this );
    }

void CPlayerController::SetOwningGameMode ( CGameMode * inGameMode )
    {
    OwningGameMode = inGameMode;
    }

void CPlayerController::ProcessPlayerInput ( float DeltaTime )
    {
    if (!bInputEnabled || !ControlledPawn)
        return;

    auto * InputSystem = CInputSystem::GetInstance ();
    if (InputSystem)
        {
        InputSystem->ProcessControllerInput ( this, DeltaTime );
        }
    }

FVector CPlayerController::GetViewLocation () const
    {
    if (ViewTarget)
        {
        return ViewTarget->GetActorLocation ();
        }
    else if (ControlledPawn)
        {
        return ControlledPawn->GetActorLocation ();
        }

    return const_cast< CPlayerController * >( this )->GetActorLocation ();
    }

FQuat CPlayerController::GetViewRotation () const
    {
    if (ViewTarget)
        {
        return ViewTarget->GetActorRotationQuat ();
        }
    else if (ControlledPawn)
        {
        return ControlledPawn->GetActorRotationQuat ();
        }

        // ИСПРАВЛЕНО: const версия метода
    return const_cast< CPlayerController * >( this )->GetActorRotationQuat ();
    }

void CPlayerController::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    ProcessPlayerInput ( DeltaTime );
    }

void CPlayerController::BeginPlay ()
    {
    Super::BeginPlay ();
    LOG_DEBUG ( "[PLAYERCONTROLLER] BeginPlay: ", GetName () );
    }

void CPlayerController::EndPlay ()
    {
    Super::EndPlay ();
    LOG_DEBUG ( "[PLAYERCONTROLLER] EndPlay: ", GetName () );

    if (ControlledPawn)
        {
        Unpossess ();
        }
    }
