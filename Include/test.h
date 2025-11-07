#pragma once
#include "Components/Camera/CameraComponent.h"
#include "Components/Meshes/MeshComponent.h"
#include "World/CEWorld.h"
#include "World/Levels/CELevel.h"

class TestWorld : public CE::CELevel
{
 public:
  TestWorld();
};

class GameWorld : public CE::CEWorld
{
 public:
  GameWorld();
};

// Реализация
TestWorld::TestWorld() : CE::CELevel(nullptr, "TestWorld")
{
  // Спавним камеру
  auto* cameraActor = SpawnActor<CE::CEActor>(this, "MainCamera");
  auto* cameraComp = cameraActor->AddDefaultSubObject<CE::CameraComponent>("Camera", cameraActor, "MainCamera");
  cameraActor->SetRootComponent(cameraComp);
  // Позиционируем камеру
  cameraActor->SetActorLocation(glm::vec3(0.0f, 0.0f, 10.0f));
  cameraActor->SetActorRotation(glm::vec3(0.0f, 0.0f, 0.0f));

  // Спавним меш-акторы
  auto* player = SpawnActor<CE::CEActor>(this, "Player");
  auto* enemy = SpawnActor<CE::CEActor>(this, "Enemy");

  player->SetRootComponent(player->AddDefaultSubObject<CE::MeshComponent>("PlayerMesh", player, "PlayerMesh"));
  enemy->SetRootComponent(enemy->AddDefaultSubObject<CE::MeshComponent>("EnemyMesh", enemy, "EnemyMesh"));

  player->SetActorLocation(glm::vec3(0.0f, 0.0f, 0.0f));
  enemy->SetActorLocation(glm::vec3(5.0f, 0.0f, 0.0f));

  auto* playerMesh = dynamic_cast<CE::MeshComponent*>(player->GetRootComponent());
  auto* enemyMesh = dynamic_cast<CE::MeshComponent*>(enemy->GetRootComponent());

  if (playerMesh)
    playerMesh->SetColor(glm::vec3(0.2f, 0.8f, 0.2f));
  if (enemyMesh)
    enemyMesh->SetColor(glm::vec3(0.8f, 0.2f, 0.2f));

  CE_CORE_DEBUG("TestWorld created with camera and mesh actors");
}

GameWorld::GameWorld() : CE::CEWorld(nullptr, "MainGameWorld")
{
  AddLevel(std::make_unique<TestWorld>());
  SetCurrentLevel("TestWorld");
}