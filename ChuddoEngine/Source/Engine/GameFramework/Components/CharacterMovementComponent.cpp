#include "Components/CharacterMovementComponent.h"
#include "Components/GravityComponent.h"
#include "Actors/Character.h"

CCharacterMovementComponent::CCharacterMovementComponent ( CObject * inOwner, const std::string & Name )
    : Super ( inOwner, Name )
    {}

void CCharacterMovementComponent::InitComponent ()
    {
    Super::InitComponent ();
    }

void CCharacterMovementComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );

    if (bIsGrounded)
        {
        CurrentJumpCount = 0;
        bIsJumping = false;
        }
    }

void CCharacterMovementComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();
    }

bool CCharacterMovementComponent::CanJump () const
    {
    if (!OwnerPawn || !OwnerPawn->IsInputEnabled ())
        return false;

    return bIsGrounded || CurrentJumpCount < MaxJumpCount;
    }

void CCharacterMovementComponent::Jump ()
    {
    if (!CanJump ()) return;

    if (auto * Gravity = OwnerPawn->GetGravityComponent ())
        {
        float neededVelocity = Gravity->GetJumpVelocity ( JumpHeight );

        if (!bIsGrounded)
            {
            neededVelocity *= AirJumpMultiplier;
            }

        Gravity->SetVerticalVelocity ( neededVelocity );
        CurrentJumpCount++;
        bIsJumping = true;
        }
    }

void CCharacterMovementComponent::StopJumping ()
    {
    bIsJumping = false;
    }