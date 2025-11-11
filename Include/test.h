#pragma once
#include "GamePlay/Actors/SunActor.h"
#include "GamePlay/Components/Camera/CameraComponent.h"
#include "GamePlay/Components/Meshes/MeshComponent.h"
#include "GamePlay/Controllers/PlayerController.h"
#include "GamePlay/GameInstance/CEGameInstance.h"
#include "GamePlay/Pawns/Pawn.h"
#include "GamePlay/World/CEWorld.h"
#include "GamePlay/World/Levels/CELevel.h"

class TestLevel : public CE::CELevel
{
 public:
  TestLevel();
  virtual void Update(float DeltaTime) override;

  CE::SunActor* DirectLighter;
  CE::PlayerController* playerController;
  CE::CEPawn* playerPawn;
};

class TestGameInstance : public CE::CEGameInstance
{
 public:
  TestGameInstance();

 protected:
  virtual void SetupWorlds() override;
};

// Реализация TestLevel
TestLevel::TestLevel() : CE::CELevel(nullptr, "TestWorld")
{
  // === СОЗДАЕМ УПРАВЛЯЕМОГО ИГРОКА ===

  // 1. Создаем PlayerController
  playerController = SpawnActor<CE::PlayerController>(this, "PlayerController");

  // 2. Создаем Pawn (игрока с камерой)
  playerPawn = SpawnActor<CE::CEPawn>(this, "PlayerPawn");
  playerPawn->SetActorLocation(glm::vec3(0.0f, 0.0f, 5.0f));
  playerPawn->SetActorRotation(glm::vec3(0.0f, 0.0f, 0.0f));

  // 3. Запассесть Pawn контроллером
  playerController->Possess(playerPawn);

  // === СТАРЫЙ КОД ДЛЯ ОБРАТНОЙ СОВМЕСТИМОСТИ ===

  DirectLighter = SpawnActor<CE::SunActor>(this, "DirectLight");
  DirectLighter->SetIntensity(10.f);
  DirectLighter->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
  DirectLighter->SetActorLocation(glm::vec3(0.0f, 0.0f, 0.0f));

  // Спавним меш-акторы
  auto* enemy = SpawnActor<CE::CEActor>(this, "Enemy");
  enemy->SetRootComponent(enemy->AddSubObject<CE::MeshComponent>("EnemyMesh", enemy, "EnemyMesh"));
  enemy->SetActorLocation(glm::vec3(7.0f, 1.5f, 0.0f));
  enemy->SetActorScale(glm::vec3(1.f, 1.f, 1.f));

  // Создаем несколько кубов в разных позициях
  std::vector<glm::vec3> positions = {
      glm::vec3(-3.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(3.0f, 0.0f, 0.0f),
      glm::vec3(-1.5f, 2.0f, 0.0f),
      glm::vec3(1.5f, 2.0f, 0.0f)};

  std::vector<glm::vec3> colors = {
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 1.0f),
      glm::vec3(1.0f, 1.0f, 0.0f),
      glm::vec3(1.0f, 0.0f, 1.0f)};

  for (int i = 0; i < (int)positions.size(); i++)
  {
    auto* actor = SpawnActor<CE::CEActor>(this, "Cube_" + std::to_string(i));
    actor->SetRootComponent(actor->AddSubObject<CE::MeshComponent>("Mesh", actor, "Mesh"));
    actor->SetActorLocation(positions[i]);

    auto* mesh = dynamic_cast<CE::MeshComponent*>(actor->GetRootComponent());
    if (mesh)
    {
      mesh->SetColor(colors[i]);
    }
  }

  auto* enemyMesh = dynamic_cast<CE::MeshComponent*>(enemy->GetRootComponent());
  if (enemyMesh)
    enemyMesh->SetColor(glm::vec3(0.8f, 0.2f, 0.2f));

  // Spawn a SunActor to control world's directional/positional "sun" light
  auto* sun = SpawnActor<CE::SunActor>(this, "SunActor");
  if (sun)
  {
    sun->SetColor(glm::vec3(1.0f, 0.95f, 0.85f));
    sun->SetIntensity(2.0f);
    sun->SetRadius(15.0f);
    sun->SetAngularSpeed(0.25f);
  }

  CE_CORE_DEBUG("TestWorld created with player controller and pawn");
}

void TestLevel::Update(float DeltaTime)
{
  CE::CELevel::Update(DeltaTime);

  static float totalTime = 0.0f;
  totalTime += DeltaTime;

  float rotationSpeed = 180.0f;

  int idx = 0;
  for (const auto& actorPtr : GetActors())
  {
    CE::CEActor* actor = actorPtr.get();
    if (!actor)
      continue;

    // Пропускаем управляемые объекты
    if (actor == playerController || actor == playerPawn)
    {
      ++idx;
      continue;
    }

    // Derive a unique speed/axis for this actor using its index
    float speed = rotationSpeed + (idx * 7.5f);
    glm::vec3 rot = glm::vec3(totalTime * glm::radians(speed * 1.5f),
                              totalTime * glm::radians(speed * 1.5f),
                              totalTime * glm::radians(speed * 1.3f));

    actor->SetActorRotation(rot);
    ++idx;
  }
}

// Реализация TestGameInstance
TestGameInstance::TestGameInstance()
    : CEGameInstance()
{
}

void TestGameInstance::SetupWorlds()
{
  // Создаем основной мир
  auto* mainWorld = CreateWorld("MainGameWorld");

  // Настраиваем мир
  auto testLevel = std::make_unique<TestLevel>();
  mainWorld->AddLevel(std::move(testLevel));
  mainWorld->SetCurrentLevel("TestWorld");

  // Настраиваем освещение мира
  CE::LightingUBO worldLighting{};
  worldLighting.lightCount = 3;

  // Key light (warm)
  worldLighting.lightPositions[0] = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
  worldLighting.lightColors[0] = glm::vec4(1.0f, 0.95f, 0.9f, 2.0f);

  // Fill light (cool)
  worldLighting.lightPositions[1] = glm::vec4(-4.0f, 3.0f, 2.0f, 1.0f);
  worldLighting.lightColors[1] = glm::vec4(0.4f, 0.6f, 1.0f, 1.2f);

  // Rim/back light (magenta-ish)
  worldLighting.lightPositions[2] = glm::vec4(0.0f, 4.0f, -6.0f, 1.0f);
  worldLighting.lightColors[2] = glm::vec4(1.0f, 0.4f, 0.6f, 1.0f);

  worldLighting.ambientColor = glm::vec4(0.05f, 0.05f, 0.06f, 0.25f);

  mainWorld->SetDefaultLighting(worldLighting);
  CE_CORE_DEBUG("TestGameInstance set world default lighting with ", worldLighting.lightCount, " lights");
}