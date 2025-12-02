#pragma once
#include <array>
#include <string>

struct AppInfo
{
  std::string AppName = "Unnamed App";
  std::array<int, 3> AppVerion = {0, 0, 1};
  std::string EngineName = "Chuddo Engine";
  std::array<int, 3> EngineVersion = {0, 0, 1};

  
  int Width = 1024;
  int Height = 768;
  bool Fullscreen = false;
  bool VSync = true;

  
  int MSAA = 4;
  int MaxFPS = 120;

  void LoadFromConfig();
};
