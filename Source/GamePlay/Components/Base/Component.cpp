#include "GamePlay/Components/Base/Component.h"

namespace CE
{
  CEComponent::CEComponent(CEObject* Owner, FString NewName)
      : CEObject(Owner, NewName)
  {
  }

  void CEComponent::Update(float DeltaTime)
  {
    CEObject::Update(DeltaTime);
  }

  void CEComponent::Tick(float DeltaTime)
  {
    CEObject::Tick(DeltaTime);
  }

  void CEComponent::BeginPlay()
  {
    CEObject::BeginPlay();
  }
}  // namespace CE