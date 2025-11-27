#pragma once

#include "Engine/GamePlay/Components/SceneComponent.h"
#include "Engine/GamePlay/CollisionSystem/CollisionTypes.h"

namespace CE
{
    class CCollisionComponent : public CSceneComponent
    {
 public:
 CCollisionComponent(CObject* Owner = nullptr, FString Name = "Collision Component");




 virtual ~CCollisionComponent() =default;

 virtual ECollisionShape GetCollisionShape() const =0;
 virtual bool CheckCollision(const CCollisionComponent* other) const = 0;

 virtual glm::vec3 GetBoundingBoxMin() const = 0;
 virtual glm::vec3 GetBoundingBoxMax() const = 0;

 void SetCollisionEnabled(bool Enabled){bIsCollisionEnabled = Enabled;}
 bool IsCollisionEnabled() const {return bIsCollisionEnabled;}

 void SetGenerateOverlapEvents(bool Enable) {bIsGenerateOverlapEvents = Enable;}
 bool IsGenerateOverlapEvents() const {return bIsGenerateOverlapEvents;}

 void SetCollisionProfileName(const FString& ProfileName);
 FString GetCollisionProfileName() const { return m_ProfileName;}

 void SetCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse Response);
  ECollisionResponse GetCollisionResponseToChannel(ECollisionChannel Channel) const;

 virtual void OnOverlapBegin(const CCollisionComponent* Other) {}
        virtual void OnOverlapEnd(const CCollisionComponent* Other) {}
        virtual void OnHit(const CCollisionComponent* Other, const glm::vec3& ImpactPoint) {}
ECollisionChannel GetCollisionChannel() const {return m_CollisionChannel;}
 protected:
FString m_ProfileName {"BlockAll"};

ECollisionChannel m_CollisionChannel = ECollisionChannel::WorldDynamic;
        std::unordered_map<ECollisionChannel, ECollisionResponse> m_CollisionResponses;
 bool bIsCollisionEnabled{true};
 bool bIsGenerateOverlapEvents{true};
    };
}