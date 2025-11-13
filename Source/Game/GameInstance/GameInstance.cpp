#include "Game/GameInstance/GameInstance.h"

#include "Engine/GamePlay/Input/InputSystem.h"
#include "Game/World/MainLevel.h"

MainGameInstance::MainGameInstance() : CE::CEGameInstance()
{
}
void MainGameInstance::SetupWorlds()
{
  auto* MainWorld = CreateWorld("World");
  auto Level = std::make_unique<MainLevel>(MainWorld, "Main Level");
  MainWorld->AddLevel(std::move(Level));
  MainWorld->SetCurrentLevel("Main Level");
}

void MainGameInstance::Update(float DeltaTime)
{
  CE::CEGameInstance::Update(DeltaTime);
  CE::InputSystem::Get().Update(DeltaTime);
}