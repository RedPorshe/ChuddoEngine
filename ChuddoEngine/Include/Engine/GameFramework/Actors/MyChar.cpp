#include "MyChar.h"

MyCharact::MyCharact ( CObject * inOwner, const std::string & DisplayName ) : Super(inOwner,DisplayName)
    {}

void MyCharact::BeginPlay ()
    {
    Super::BeginPlay ();
    }

void MyCharact::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }

void MyCharact::EndPlay ()
    {
    Super::EndPlay ();
    }

void MyCharact::SetupPlayerInputComponent ( CInputComponent * InputComponent )
    {
    Super::SetupPlayerInputComponent ( InputComponent );
    }
