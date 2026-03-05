#include "Test.h"
#include "Camera/CameraComponent.h"

CFirstPersonCharacter::CFirstPersonCharacter ( CObject * inOWner, const std::string & inName ) :Super ( inOWner, inName )
    {
    Camera = AddDefaultSubObject<CCameraComponent> ();
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
    }

void CFirstPersonCharacter::DebugInfo ( float dt )
    {
    }
