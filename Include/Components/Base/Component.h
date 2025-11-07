#pragma once
#include "Core/CEObject.h"

namespace CE
{
  class CEComponent : public CEObject
  {
   public:
    CEComponent(CEObject* Owner = nullptr, FString NewName = "Component");
    virtual ~CEComponent() = default;
    virtual void Update(float DeltaTime) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
  };
}  // namespace CE