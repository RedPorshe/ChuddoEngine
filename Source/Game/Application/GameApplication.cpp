#include "Game/Application/GameApplication.h"

#include "Game/GameInstance/GameInstance.h"

GameApp::GameApp(CE::AppInfo* info) : CE::Application(info)
{
}
void GameApp::Initialize()
{
  CE::Application::Initialize();
  InitWorld();
}

void GameApp::InitWorld()
{
  m_GameInstance = std::make_unique<MainGameInstance>();
  m_GameInstance->Initialize();
}