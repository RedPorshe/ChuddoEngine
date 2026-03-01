#include "Actors/PlayerStart.h"
#include "Components/GravityComponent.h"

CPlayerStart::CPlayerStart ( CObject * inOwner, const std::string & inName )
	: Super ( inOwner, inName )
	{
    if (!RootComponent) LOG_ERROR ("NO ROOT COMPONENT");
	LOG_DEBUG ( "[PLAYERSTART] Created: ", GetName () );
	DestroyGravity ();
	}

CPlayerStart::~CPlayerStart ()
	{}

void CPlayerStart::BeginPlay ()
	{
	Super::BeginPlay ();
	LOG_ERROR ( GetName (), " BeginPlay - Current location: ", GetActorLocation () );

	  // Проверяем был ли вызван MoveActor до BeginPlay
	if (GetActorLocation ().x == 0 && GetActorLocation ().y == 0 && GetActorLocation ().z == 0)
		{
		LOG_WARN ( GetName (), " Still at (0,0,0) in BeginPlay! MoveActor might have been called too early?" );
		}
	}

void CPlayerStart::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );
	}

void CPlayerStart::EndPlay ()
	{
	Super::EndPlay ();
	}
