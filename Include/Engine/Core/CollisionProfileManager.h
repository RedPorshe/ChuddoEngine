#pragma once

#include "Engine/GamePlay/CollisionSystem/CollisionTypes.h"

namespace CE
{
    class CollisionProfileManager
    {
public:
static CollisionProfileManager& Get();
void InitializeDefaultProfiles();

const FCollisionProfile* FindProfile(const FString& ProfileName) const;
void AddProfile(const FCollisionProfile& Profile);

   
        static FString NoCollision;
        static FString BlockAll;
        static FString OverlapAll;
        static FString BlockAllDynamic;
        static FString OverlapAllDynamic;
        static FString IgnoreOnlyPawn;
        static FString OverlapOnlyPawn;
        static FString Pawn;
        static FString Spectator;
        static FString Character;
        static FString PhysicsActor;
        static FString Destructible;
        static FString Trigger;
        static FString Ragdoll;
        static FString Vehicle;
        static FString UI;

         private:
        std::unordered_map<FString, FCollisionProfile> m_Profiles;
    };
}