#include "Components/InputComponent.h"
#include "Actors/Pawn.h"

CInputComponent::CInputComponent ( CObject * inOwner, const std::string & inDysplayName ) : Super(inOwner,inDysplayName)
	{}

CInputComponent::~CInputComponent ()
	{}

void CInputComponent::InitComponent ()
	{
	Super::InitComponent ();
	}

void CInputComponent::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );
	}

void CInputComponent::OnBeginPlay ()
	{
	Super::OnBeginPlay ();
	}
