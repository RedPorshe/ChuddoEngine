#pragma once
#include "CoreMinimal.h"
namespace CE
{
  struct AppInfo
  {
    FString AppName;
    FString EngineName;
    int EngineVersion[3];
    int AppVerion[3];
    int Width{800};
    int Height{600};
  };
}  // namespace CE
