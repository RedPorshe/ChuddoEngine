#pragma once
#include "Components/Camera/CameraComponent.h"
#include "Components/Meshes/MeshComponent.h"
#include "World/CEWorld.h"
#include "World/Levels/CELevel.h"
// Sun actor
#include "Actors/SunActor.h"

class TestWorld : public CE::CELevel
{
 public:
  TestWorld();
  virtual void Update(float DeltaTime) override;
  CE::CEActor* cameraActor;
  CE::CEActor* playerActor;
  CE::SunActor* DirectLighter;
};

void TestWorld::Update(float DeltaTime)
{
  CE::CELevel::Update(DeltaTime);

  static float totalTime = 0.0f;
  totalTime += DeltaTime;

  // Вращение вокруг оси Y (вертикальной) на 45 градусов в секунду
  float rotationSpeed = 180.0f;  // градусов в секунду
  float currentRotation = totalTime * (rotationSpeed / 20.f);

  cameraActor->SetActorRotation(glm::vec3(0.f, 0.f, currentRotation));

  int idx = 0;
  for (const auto& actorPtr : GetActors())
  {
    CE::CEActor* actor = actorPtr.get();
    if (!actor)
      continue;
    // Skip the camera actor
    if (actor == cameraActor)
    {
      ++idx;
      continue;
    }

    // Derive a unique speed/axis for this actor using its index
    float speed = rotationSpeed + (idx * 7.5f);
    // Make rotations vary per-axis for visual variety
    glm::vec3 rot = glm::vec3(totalTime * glm::radians(speed * 1.5f),
                              totalTime * glm::radians(speed * 1.5f),
                              totalTime * glm::radians(speed * 1.3f));

    actor->SetActorRotation(rot);
    ++idx;
  }
}

class GameWorld : public CE::CEWorld
{
 public:
  GameWorld();
};

// Реализация

TestWorld::TestWorld() : CE::CELevel(nullptr, "TestWorld")
{
  // Спавним камеру
  cameraActor = SpawnActor<CE::CEActor>(this, "MainCamera");
  auto* cameraComp = cameraActor->AddDefaultSubObject<CE::CameraComponent>("Camera", cameraActor, "MainCamera");
  cameraActor->SetRootComponent(cameraComp);
  // Позиционируем камеру
  cameraActor->SetActorLocation(glm::vec3(0.0f, 0.0f, 50.0f));
  cameraActor->SetActorRotation(glm::vec3(0.0f, 0.0f, 0.0f));

  DirectLighter = SpawnActor<CE::SunActor>(this, "DirectLight");
  DirectLighter->SetIntensity(10.f);
  DirectLighter->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
  DirectLighter->SetActorLocation(glm::vec3(0.0f, 0.0f, 0.0f));
  // Спавним меш-акторы
  playerActor = SpawnActor<CE::CEActor>(this, "Player");
  auto* enemy = SpawnActor<CE::CEActor>(this, "Enemy");

  playerActor->SetRootComponent(playerActor->AddDefaultSubObject<CE::MeshComponent>("PlayerMesh", playerActor, "PlayerMesh"));
  enemy->SetRootComponent(enemy->AddDefaultSubObject<CE::MeshComponent>("EnemyMesh", enemy, "EnemyMesh"));

  // РАЗНЫЕ позиции для мешей
  playerActor->SetActorLocation(glm::vec3(-2.0f, -2.0f, 0.0f));  // Слева на уровне земли
  enemy->SetActorLocation(glm::vec3(7.0f, 1.5f, 0.0f));

  // Если есть метод SetActorScale
  playerActor->SetActorScale(glm::vec3(2.0f, 2.0f, 2.0f));  // Нормальный размер
  enemy->SetActorScale(glm::vec3(1.f, 1.f, 1.f));

  // Создаем несколько кубов в разных позициях
  std::vector<glm::vec3> positions = {
      glm::vec3(-3.0f, 0.0f, 0.0f),  // Левый
      glm::vec3(0.0f, 0.0f, 0.0f),   // Центральный
      glm::vec3(3.0f, 0.0f, 0.0f),   // Правый
      glm::vec3(-1.5f, 2.0f, 0.0f),  // Верхний левый
      glm::vec3(1.5f, 2.0f, 0.0f)    // Верхний правый
  };

  std::vector<glm::vec3> colors = {
      glm::vec3(1.0f, 0.0f, 0.0f),  // Красный
      glm::vec3(0.0f, 1.0f, 0.0f),  // Зеленый
      glm::vec3(0.0f, 0.0f, 1.0f),  // Синий
      glm::vec3(1.0f, 1.0f, 0.0f),  // Желтый
      glm::vec3(1.0f, 0.0f, 1.0f)   // Пурпурный
  };

  for (int i = 0; i < (int)positions.size(); i++)
  {
    auto* actor = SpawnActor<CE::CEActor>(this, "Cube_" + std::to_string(i));
    actor->SetRootComponent(actor->AddDefaultSubObject<CE::MeshComponent>("Mesh", actor, "Mesh"));
    actor->SetActorLocation(positions[i]);

    auto* mesh = dynamic_cast<CE::MeshComponent*>(actor->GetRootComponent());
    if (mesh)
    {
      mesh->SetColor(colors[i]);
    }
  }

  auto* playerMesh = dynamic_cast<CE::MeshComponent*>(playerActor->GetRootComponent());
  auto* enemyMesh = dynamic_cast<CE::MeshComponent*>(enemy->GetRootComponent());

  if (playerMesh)
    playerMesh->SetColor(glm::vec3(0.f, 0.8f, 0.f));  // Зеленый
  if (enemyMesh)
    enemyMesh->SetColor(glm::vec3(0.8f, 0.2f, 0.2f));  // Красный
  // Spawn a SunActor to control world's directional/positional "sun" light
  auto* sun = SpawnActor<CE::SunActor>(this, "SunActor");
  if (sun)
  {
    sun->SetColor(glm::vec3(1.0f, 0.95f, 0.85f));
    sun->SetIntensity(2.0f);
    sun->SetRadius(15.0f);
    sun->SetAngularSpeed(0.25f);
  }

  CE_CORE_DEBUG("TestWorld created with camera and mesh actors");
}

GameWorld::GameWorld() : CE::CEWorld(nullptr, "MainGameWorld")
{
  AddLevel(std::make_unique<TestWorld>());
  SetCurrentLevel("TestWorld");

  // Инициализируем базовое освещение при создании мира
  // Устанавливаем мировое (по-умолчанию) освещение, чтобы уровни могли его
  // унаследовать, если не заполняют свои собственные данные освещения.
  CE::LightingUBO worldLighting;
  worldLighting.lightCount = 1;
  worldLighting.lightPositions[0] = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
  worldLighting.lightColors[0] = glm::vec4(1.0f, 0.0f, 0.0f, 1.2f);

  worldLighting.ambientColor = glm::vec4(0.2f, 0.2f, 0.2f, 0.8f);

  SetDefaultLighting(worldLighting);

  // (SunActor is spawned per-level inside TestWorld::TestWorld)
}