#include "Engine/GamePlay/Components/Base/Component.h"

namespace CE
{
  CEComponent::CEComponent(CObject* Owner, FString NewName)
      : CObject(Owner, NewName)
  {
  }

  void CEComponent::Update(float DeltaTime)
  {
    CObject::Update(DeltaTime);
  }

  void CEComponent::Tick(float DeltaTime)
  {
    CObject::Tick(DeltaTime);
  }

  void CEComponent::BeginPlay()
  {
    CObject::BeginPlay();
  }
}  // namespace CE