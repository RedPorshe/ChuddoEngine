#include "Engine/Core/CollisionProfileManager.h"

namespace CE
{
    FString CCollisionProfileManager::NoCollision = "NoCollision";
    FString CCollisionProfileManager::BlockAll = "BlockAll";
    FString CCollisionProfileManager::OverlapAll = "OverlapAll";
    FString CCollisionProfileManager::BlockAllDynamic = "BlockAllDynamic";
    FString CCollisionProfileManager::OverlapAllDynamic = "OverlapAllDynamic";
    FString CCollisionProfileManager::IgnoreOnlyPawn = "IgnoreOnlyPawn";
    FString CCollisionProfileManager::OverlapOnlyPawn = "OverlapOnlyPawn";
    FString CCollisionProfileManager::Pawn = "Pawn";
    FString CCollisionProfileManager::Spectator = "Spectator";
    FString CCollisionProfileManager::Character = "Character";
    FString CCollisionProfileManager::PhysicsActor = "PhysicsActor";
    FString CCollisionProfileManager::Destructible = "Destructible";
    FString CCollisionProfileManager::Trigger = "Trigger";
    FString CCollisionProfileManager::Ragdoll = "Ragdoll";
    FString CCollisionProfileManager::Vehicle = "Vehicle";
    FString CCollisionProfileManager::UI = "UI";

    CCollisionProfileManager& CCollisionProfileManager::Get()
    {
        static CCollisionProfileManager instance;
        return instance;
    }

    void CCollisionProfileManager::InitializeDefaultProfiles()
    {
        // NoCollision - ничего не коллизирует
        FCollisionProfile noCollision(NoCollision, ECollisionChannel::WorldStatic);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            noCollision.Responses[channel] = ECollisionResponse::Ignore;
        }
        AddProfile(noCollision);

        // BlockAll - блокирует всё
        FCollisionProfile blockAll(BlockAll, ECollisionChannel::WorldDynamic);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            blockAll.Responses[channel] = ECollisionResponse::Block;
        }
        AddProfile(blockAll);

        // OverlapAll - пересекает всё
        FCollisionProfile overlapAll(OverlapAll, ECollisionChannel::WorldDynamic);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            overlapAll.Responses[channel] = ECollisionResponse::Overlap;
        }
        AddProfile(overlapAll);

        // BlockAllDynamic - блокирует всё динамическое
        FCollisionProfile blockAllDynamic(BlockAllDynamic, ECollisionChannel::WorldDynamic);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            if (channel == ECollisionChannel::WorldStatic)
            {
                blockAllDynamic.Responses[channel] = ECollisionResponse::Ignore;
            }
            else
            {
                blockAllDynamic.Responses[channel] = ECollisionResponse::Block;
            }
        }
        AddProfile(blockAllDynamic);

        // OverlapAllDynamic - пересекает всё динамическое
        FCollisionProfile overlapAllDynamic(OverlapAllDynamic, ECollisionChannel::WorldDynamic);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            if (channel == ECollisionChannel::WorldStatic)
            {
                overlapAllDynamic.Responses[channel] = ECollisionResponse::Ignore;
            }
            else
            {
                overlapAllDynamic.Responses[channel] = ECollisionResponse::Overlap;
            }
        }
        AddProfile(overlapAllDynamic);

        // Trigger - пересекает всё, но сам WorldStatic
        FCollisionProfile trigger(Trigger, ECollisionChannel::Trigger);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            if (channel == ECollisionChannel::Trigger)
            {
                trigger.Responses[channel] = ECollisionResponse::Ignore;
            }
            else
            {
                trigger.Responses[channel] = ECollisionResponse::Overlap;
            }
        }
        AddProfile(trigger);

        // Pawn - для персонажей
        FCollisionProfile pawn(Pawn, ECollisionChannel::Pawn);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            pawn.Responses[channel] = ECollisionResponse::Ignore;
        }
        pawn.Responses[ECollisionChannel::WorldStatic] = ECollisionResponse::Block;
        pawn.Responses[ECollisionChannel::WorldDynamic] = ECollisionResponse::Block;
        pawn.Responses[ECollisionChannel::Pawn] = ECollisionResponse::Block;
        pawn.Responses[ECollisionChannel::PhysicsBody] = ECollisionResponse::Block;
        pawn.Responses[ECollisionChannel::Vehicle] = ECollisionResponse::Block;
        pawn.Responses[ECollisionChannel::Trigger] = ECollisionResponse::Overlap;
        pawn.Responses[ECollisionChannel::Camera] = ECollisionResponse::Ignore;
        AddProfile(pawn);

        // Character - для игровых персонажей
        FCollisionProfile character(Character, ECollisionChannel::Pawn);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            character.Responses[channel] = ECollisionResponse::Ignore;
        }
        character.Responses[ECollisionChannel::WorldStatic] = ECollisionResponse::Block;
        character.Responses[ECollisionChannel::WorldDynamic] = ECollisionResponse::Block;
        character.Responses[ECollisionChannel::Pawn] = ECollisionResponse::Block;
        character.Responses[ECollisionChannel::PhysicsBody] = ECollisionResponse::Block;
        character.Responses[ECollisionChannel::Vehicle] = ECollisionResponse::Block;
        character.Responses[ECollisionChannel::Destructible] = ECollisionResponse::Block;
        character.Responses[ECollisionChannel::Trigger] = ECollisionResponse::Overlap;
        character.Responses[ECollisionChannel::Projectile] = ECollisionResponse::Overlap;
        character.Responses[ECollisionChannel::Camera] = ECollisionResponse::Ignore;
        AddProfile(character);

        // PhysicsActor - для физических объектов
        FCollisionProfile physicsActor(PhysicsActor, ECollisionChannel::PhysicsBody);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            physicsActor.Responses[channel] = ECollisionResponse::Block;
        }
        physicsActor.Responses[ECollisionChannel::Trigger] = ECollisionResponse::Overlap;
        physicsActor.Responses[ECollisionChannel::UI] = ECollisionResponse::Ignore;
        AddProfile(physicsActor);

        // Destructible - для разрушаемых объектов
        FCollisionProfile destructible(Destructible, ECollisionChannel::Destructible);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            destructible.Responses[channel] = ECollisionResponse::Block;
        }
        destructible.Responses[ECollisionChannel::Trigger] = ECollisionResponse::Overlap;
        destructible.Responses[ECollisionChannel::Projectile] = ECollisionResponse::Overlap;
        AddProfile(destructible);

        // Vehicle - для транспортных средств
        FCollisionProfile vehicle(Vehicle, ECollisionChannel::Vehicle);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            vehicle.Responses[channel] = ECollisionResponse::Block;
        }
        vehicle.Responses[ECollisionChannel::Trigger] = ECollisionResponse::Overlap;
        vehicle.Responses[ECollisionChannel::Pawn] = ECollisionResponse::Overlap;
        AddProfile(vehicle);

        // Ragdoll - для ragdoll физики
        FCollisionProfile ragdoll(Ragdoll, ECollisionChannel::PhysicsBody);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            ragdoll.Responses[channel] = ECollisionResponse::Block;
        }
        ragdoll.Responses[ECollisionChannel::Ragdoll] = ECollisionResponse::Ignore;
        ragdoll.Responses[ECollisionChannel::Trigger] = ECollisionResponse::Overlap;
        AddProfile(ragdoll);

        // UI - для элементов интерфейса
        FCollisionProfile ui(UI, ECollisionChannel::UI);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            ui.Responses[channel] = ECollisionResponse::Ignore;
        }
        ui.Responses[ECollisionChannel::UI] = ECollisionResponse::Block;
        AddProfile(ui);

        // Spectator - для спектаторских камер
        FCollisionProfile spectator(Spectator, ECollisionChannel::Camera);
        for (int i = 0; i < static_cast<int>(ECollisionChannel::Max); ++i)
        {
            ECollisionChannel channel = static_cast<ECollisionChannel>(i);
            spectator.Responses[channel] = ECollisionResponse::Ignore;
        }
        spectator.Responses[ECollisionChannel::WorldStatic] = ECollisionResponse::Overlap;
        spectator.Responses[ECollisionChannel::WorldDynamic] = ECollisionResponse::Overlap;
        AddProfile(spectator);
    }

    const FCollisionProfile* CCollisionProfileManager::FindProfile(const FString& ProfileName) const
    {
        auto it = m_Profiles.find(ProfileName);
        if (it != m_Profiles.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    void CCollisionProfileManager::AddProfile(const FCollisionProfile& Profile)
    {
        m_Profiles[Profile.ProfileName] = Profile;
    }

    bool CCollisionProfileManager::CreateProfile(const FString& ProfileName, ECollisionChannel Channel, 
                                                const std::unordered_map<ECollisionChannel, ECollisionResponse>& Responses)
    {
        if (m_Profiles.find(ProfileName) != m_Profiles.end())
        {
            return false;
        }

        FCollisionProfile newProfile(ProfileName, Channel);
        newProfile.Responses = Responses;
        AddProfile(newProfile);
        return true;
    }

    std::vector<FString> CCollisionProfileManager::GetAllProfileNames() const
    {
        std::vector<FString> names;
        names.reserve(m_Profiles.size());
        
        for (const auto& pair : m_Profiles)
        {
            names.push_back(pair.first);
        }
        
        return names;
    }

    bool CCollisionProfileManager::RemoveProfile(const FString& ProfileName)
    {
        auto it = m_Profiles.find(ProfileName);
        if (it != m_Profiles.end())
        {
            m_Profiles.erase(it);
            return true;
        }
        return false;
    }
}