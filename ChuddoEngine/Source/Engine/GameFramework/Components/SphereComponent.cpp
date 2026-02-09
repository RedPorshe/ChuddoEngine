
#include "Components/SphereComponent.h"

CSphereComponent::CSphereComponent ( CObject * inOwner, const std::string & inDisplayName, float InRadius )
    :Super(inOwner,inDisplayName),m_Radius(InRadius)
    {
    SetShapeType ( ECollisionShape::SPHERE );
    }

CSphereComponent::~CSphereComponent ()
    {
    }

void CSphereComponent::InitComponent ()
    {
    Super::InitComponent ();
    }

void CSphereComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }

void CSphereComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();
    }

bool CSphereComponent::CheckCollision ( CBaseCollisionComponent * other ) const
    {
    return false;
    }
