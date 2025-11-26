#pragma once
#include "Engine/Core/Object.h"

namespace CE
{
  class CComponent : public CObject
  {
   public:
    CComponent(CObject* Owner = nullptr, FString NewName = "Component");
    virtual ~CComponent() = default;
    virtual void Update(float DeltaTime) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
  };
}  // namespace CE