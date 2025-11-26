#include "Engine/GamePlay/Components/Base/Component.h"

namespace CE
{
  CComponent::CComponent(CObject* Owner, FString NewName)
      : CObject(Owner, NewName)
  {
  }

  void CComponent::Update(float DeltaTime)
  {
    CObject::Update(DeltaTime);
  }

  void CComponent::Tick(float DeltaTime)
  {
    CObject::Tick(DeltaTime);
  }

  void CComponent::BeginPlay()
  {
    CObject::BeginPlay();
  }
}  // namespace CE