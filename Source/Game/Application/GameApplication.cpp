#include "Game/Application/GameApplication.h"

#include "Game/GameInstance/GameInstance.h"

GameApp::GameApp(AppInfo* info) : Application(info)
{
}
void GameApp::Initialize()
{
  Application::Initialize();
  InitWorld();
}

void GameApp::InitWorld()
{
  m_GameInstance = std::make_unique<MainGameInstance>();
  m_GameInstance->Initialize();
}