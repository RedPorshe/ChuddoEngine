#include "Test.h"
#include "Camera/CameraComponent.h"
#include "Components/Collisions/CapsuleComponent.h"
#include "Components/Meshes/StaticMeshComponent.h"

CFirstPersonCharacter::CFirstPersonCharacter ( CObject * inOWner, const std::string & inName ) :Super ( inOWner, inName )
    {
    Camera = AddDefaultSubObject<CCameraComponent> ();
    Camera->AttachTo ( Capsule );
    Mesh->ResizeCube ( 1.5f );
    Camera->SetRelativeLocation ( 0.f, 15.f, 25.f );
    }

void CFirstPersonCharacter::BeginPlay ()
    {
    Super::BeginPlay ();
    }

void CFirstPersonCharacter::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }

void CFirstPersonCharacter::EndPlay ()
    {
    Super::EndPlay ();
    }

void CFirstPersonCharacter::OnComponentBeginOverlap ( CBaseCollisionComponent * other )
    {
    
    }

void CFirstPersonCharacter::OnComponentEndOverlap ( CBaseCollisionComponent * other )
    {
    }

void CFirstPersonCharacter::OnComponentHit ( CBaseCollisionComponent * other )
    {
    Super::OnComponentHit ( other );
    }

void CFirstPersonCharacter::SetupPlayerInputComponent ( CInputComponent * InputComponent )
    {
    Super::SetupPlayerInputComponent ( InputComponent );
    auto input = GetInputComponent ();
    if (input)
        {
        input->BindAxis ( "MoveForward", EKeys::W, EKeys::S, [ this ] ( float val ) { MoveForward ( val ); } );
        input->BindAxis ( "MoveRight", EKeys::D, EKeys::A, [ this ] ( float val ) { MoveRight ( val ); } );
        input->BindAction ( "Jump", EKeys::Space, EInputEvent::IE_Pressed, [ this ] () { Jump(); } );
        }
    }

void CFirstPersonCharacter::DebugInfo ( float dt )
    {
    }

void CFirstPersonCharacter::MoveForward ( float val )
    {
    auto forward = GetActorForwardVector ();
    AddMovementInput ( forward, val );
    }

void CFirstPersonCharacter::MoveRight ( float val )
    {
    auto right = GetActorRightVector ();
    AddMovementInput ( right, val );
    }

void CFirstPersonCharacter::Jump ()
    {
    StartJump ();
    }
