#pragma once

#include "Engine/Application/Application.h"

class GameApp : public CE::Application
{
 public:
  GameApp(CE::AppInfo* info);
  ~GameApp() {};
  virtual void Initialize() override;

 protected:
  void InitWorld();
};