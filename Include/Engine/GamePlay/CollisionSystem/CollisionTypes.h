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
    Venicle,
    Destructible,
    Projectile,
    Camera,
    Trigger,
    UI,

    MAX
};

enum class ECollisionResponce

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
std::unordered_map<ECollisionChannel, ECollisionResponce> Responses;

FCollisionProfile(const FString & Name,ECollisionChannel Channel)
: ProfileName(Name), CollisionChannel(Channel) {}
};


}