#pragma once
#include "Engine/Core/Object.h"

namespace CE
{
  class CEComponent : public CObject
  {
   public:
    CEComponent(CObject* Owner = nullptr, FString NewName = "Component");
    virtual ~CEComponent() = default;
    virtual void Update(float DeltaTime) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
  };
}  // namespace CE