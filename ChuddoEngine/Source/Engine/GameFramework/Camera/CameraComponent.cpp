#include "Camera/CameraComponent.h"

CCameraComponent::CCameraComponent ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {}

CCameraComponent::~CCameraComponent ()
    {}

void CCameraComponent::InitComponent ()
    {
    Super::InitComponent ();
    }

void CCameraComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }

void CCameraComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();
    }
