#pragma once

#include "Engine/Application/Application.h"

class GameApp : public Application
{
 public:
  GameApp(AppInfo* info);
  ~GameApp() {};
  virtual void Initialize() override;

 protected:
  void InitWorld();
};