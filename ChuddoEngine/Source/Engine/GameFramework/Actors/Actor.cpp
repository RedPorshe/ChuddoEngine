#include "Actors/Actor.h"
#include "World/Level.h"
#include "GameInstance.h"



CActor::CActor ( CObject * owner, const std::string & inName ) : CObject ( owner, inName )
    {
    std::cout << "[ACTOR] Actor created: " << inName << "\n";
    }

CActor::~CActor ()
    {
    std::cout << "[ACTOR] Actor destroyed: " << GetName () << "\n";
    }

void CActor::BeginPlay ()
    {
    std::cout << "[ACTOR] BeginPlay: " << GetName () << "\n";
    }

void CActor::Tick ( float deltaTime )
    {
    std::cout << "[ACTOR] Tick: " << GetName () << " (delta: " << deltaTime << ")\n";
    }

void CActor::EndPlay ()

    {
    std::cout << "[ACTOR] EndPlay: " << GetName () << "\n";
    }

CLevel * CActor::GetLevel () const
    {
    return dynamic_cast< CLevel * >( GetOwner () );
    }

CWorld * CActor::GetWorld () const
    {
    return CGameInstance::Get().GetWorld();
    }
