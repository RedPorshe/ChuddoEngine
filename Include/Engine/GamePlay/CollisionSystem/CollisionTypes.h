#pragma once
#include "CoreMinimal.h"
#include <unordered_map>

namespace CE
{

enum class ECollisionChannel

{
    WorldStatic,
    WorldDynamic,
    Pawn,
    PhysicsBody,
    Vehicle,
    Destructible,
    Ragdoll,
    Projectile,
    Camera,
    Trigger,
    UI,

   Max
};

   enum class ECollisionShape
    {
        Box,
        Sphere,
        Capsule,
        Mesh
    };

enum class ECollisionResponse

{
    Ignore,
    Overlap,
    Block
};

struct FCollisionChannel
{
 ECollisionChannel Channel;
 FString Name;
 FCollisionChannel(ECollisionChannel InChannel, const FString & InName) : Channel(InChannel), Name(InName) {}
};

struct FCollisionProfile
{
FString ProfileName;
ECollisionChannel CollisionChannel;
std::unordered_map<ECollisionChannel, ECollisionResponse> Responses;

    FCollisionProfile() 
            : ProfileName("Default")
            , CollisionChannel(ECollisionChannel::WorldDynamic)
        {}

FCollisionProfile(const FString & Name,ECollisionChannel Channel)
: ProfileName(Name), CollisionChannel(Channel) {}
};


}