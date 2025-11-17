#pragma once
#include "Engine/GamePlay/Actors/Character.h"
#include "Engine/GamePlay/Actors/Pawn.h"
#include "Engine/GamePlay/Actors/SunActor.h"
#include "Engine/GamePlay/Actors/TerrainActor.h"
#include "Engine/GamePlay/Components/BoxComponent.h"
#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/GamePlay/Components/PlaneComponent.h"
#include "Engine/GamePlay/Components/SphereComponent.h"
#include "Engine/GamePlay/Controllers/PlayerController.h"
#include "Engine/GamePlay/GameInstance/CEGameInstance.h"
#include "Engine/GamePlay/World/CEWorld.h"
#include "Engine/GamePlay/World/Levels/CELevel.h"

class TestLevel : public CE::CELevel
{
 public:
  TestLevel();
  virtual void Update(float DeltaTime) override;

  CE::SunActor* DirectLighter;
  CE::PlayerController* playerController;
  CE::CECharacter* playerCharacter;

  // Добавляем компоненты коллизий для тестирования
  CE::CEActor* collisionTestBox;
  CE::CEActor* collisionTestSphere;
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
  // === СОЗДАЕМ ЛАНДШАФТ ===
  auto* terrain = SpawnActor<CE::TerrainActor>(this, "Terrain");
  if (terrain)
  {
    terrain->SetActorLocation(glm::vec3(0.0f, -2.0f, 0.0f));
  }

  // === СОЗДАЕМ ЗЕМЛЮ ПЕРВОЙ ===
  auto* ground = SpawnActor<CE::CEActor>(this, "GroundActor");
  auto* planeComponent = ground->AddDefaultSubObject<CE::PlaneComponent>("Ground", ground, "Ground");
  ground->SetRootComponent(planeComponent);
  ground->SetActorLocation(glm::vec3(0.0f, -2.0f, 0.0f));  // Земля на нулевой высоте

  // Настраиваем плоскость
  planeComponent->SetSize(50.0f, 50.0f);  // 50x50 метров
  planeComponent->SetSubdivisions(10);    // Подразделения для красивого вида
  planeComponent->SetCollisionEnabled(true);
  planeComponent->SetGenerateOverlapEvents(true);

  // === СОЗДАЕМ УПРАВЛЯЕМОГО ИГРОКА ===
  // 1. Создаем PlayerController
  playerController = SpawnActor<CE::PlayerController>(this, "PlayerController");

  // 2. Создаем Pawn (игрока с камерой)
  playerCharacter = SpawnActor<CE::CECharacter>(this, "PlayerPawn");
  playerCharacter->SetActorLocation(glm::vec3(0.0f, 3.5f, -5.0f));  // Опустили до 0.5м над землей
  playerCharacter->SetActorRotation(glm::vec3(0.0f, 0.0f, 0.5f));

  // 3. Запассесть Pawn контроллером
  playerController->Possess(playerCharacter);

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
      glm::vec3(-3.0f, 0.5f, 0.0f),  // Над землей
      glm::vec3(0.0f, 0.5f, 0.0f),   // Над землей
      glm::vec3(3.0f, 0.5f, 0.0f),   // Над землей
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

  // === ТЕСТОВЫЕ ОБЪЕКТЫ ДЛЯ КОЛЛИЗИЙ ===

  // 1. Бокс для коллизий
  collisionTestBox = SpawnActor<CE::CEActor>(this, "CollisionBox");
  auto* boxCollision = collisionTestBox->AddDefaultSubObject<CE::CEBoxComponent>("BoxCollision", collisionTestBox, "BoxCollision");
  collisionTestBox->SetRootComponent(boxCollision);
  collisionTestBox->SetActorLocation(glm::vec3(8.0f, 1.0f, 8.0f));  // Над землей
  boxCollision->SetExtents(0.5f, 0.5f, 0.5f);
  boxCollision->SetCollisionEnabled(true);
  boxCollision->SetGenerateOverlapEvents(true);

  // 2. Сфера для коллизий
  collisionTestSphere = SpawnActor<CE::CEActor>(this, "CollisionSphere");
  auto* sphereCollision = collisionTestSphere->AddDefaultSubObject<CE::SphereComponent>("SphereCollision", collisionTestSphere, "SphereCollision");
  collisionTestSphere->SetRootComponent(sphereCollision);
  collisionTestSphere->SetActorLocation(glm::vec3(-8.0f, 1.5f, 8.0f));  // Над землей
  sphereCollision->SetRadius(0.75f);
  sphereCollision->SetCollisionEnabled(true);
  sphereCollision->SetGenerateOverlapEvents(true);

  // 3. Добавляем коллизии к существующим кубам
  int cubeIndex = 0;
  for (const auto& actorPtr : GetActors())
  {
    CE::CEActor* actor = actorPtr.get();
    if (!actor || actor->GetName().find("Cube_") == std::string::npos)
      continue;

    // Добавляем BoxComponent к кубам
    auto* cubeCollision = actor->AddDefaultSubObject<CE::CEBoxComponent>("CubeCollision", actor, "CubeCollision_" + std::to_string(cubeIndex));
    cubeCollision->SetExtents(0.25f, 0.25f, 0.25f);  // Кубы 0.5x0.5x0.5 метра
    cubeCollision->SetCollisionEnabled(true);
    cubeCollision->SetGenerateOverlapEvents(true);
    cubeIndex++;
  }

  CE_CORE_DEBUG("TestWorld created with improved collision setup");
}

void TestLevel::Update(float DeltaTime)
{
  CE::CELevel::Update(DeltaTime);

  static float totalTime = 0.0f;
  totalTime += DeltaTime;

  // Вращение кубов (как было)
  float rotationSpeed = 180.0f;
  int idx = 0;
  for (const auto& actorPtr : GetActors())
  {
    CE::CEActor* actor = actorPtr.get();
    if (!actor)
      continue;

    if (actor == playerController || actor == playerCharacter)
    {
      ++idx;
      continue;
    }

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