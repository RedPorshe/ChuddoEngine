#include "Engine/Core/CollisionProfileManager.h"
#include "Engine/GamePlay/CollisionSystem/CollisionComponent.h"
#include "Engine/GamePlay/CollisionSystem/CollisionTypes.h"


namespace CE
{
 CCollisionComponent::CCollisionComponent(CObject* Owner, FString Name) : CSceneComponent(Owner,Name)
{
    SetCollisionProfileName("BlockAll");
}

void CCollisionComponent::SetCollisionProfileName(const FString& ProfileName)
   
{
    auto ProfManager = CCollisionProfileManager::Get();
    const FCollisionProfile* profile = ProfManager.FindProfile(ProfileName);

    if (profile)
    {
        m_ProfileName = ProfileName;
        m_CollisionChannel = profile->CollisionChannel;
        m_CollisionResponses = profile->Responses;
    }
    else
    {
        CE_CORE_WARN("Collision profile not found ", ProfileName);
    }
}
void CCollisionComponent::SetCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse Response)
{
    m_CollisionResponses[Channel] = Response;
}
ECollisionResponse CCollisionComponent::GetCollisionResponseToChannel(ECollisionChannel Channel) const
{
  auto it = m_CollisionResponses.find(Channel);
        if (it != m_CollisionResponses.end())
        {
            return it->second;
        }
        return ECollisionResponse::Ignore;
}
}