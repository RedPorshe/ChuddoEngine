#include "Engine/Core/AppInfo.h"

#include "Engine/Core/Config.h"

void AppInfo::LoadFromConfig()
{
  auto& config = Config::Get();

  Width = config.GetInt("WindowWidth", 1024);
  Height = config.GetInt("WindowHeight", 768);
  Fullscreen = config.GetBool("Fullscreen", false);
  VSync = config.GetBool("VSync", true);
  MSAA = config.GetInt("MSAASamples", 4);
  MaxFPS = config.GetInt("MaxFPS", 120);

  CE_CORE_DEBUG("AppInfo loaded from config: ", Width, "x", Height,
                " Fullscreen:", Fullscreen, " VSync:", VSync);
}