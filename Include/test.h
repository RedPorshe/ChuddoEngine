#pragma once
#include "Components/Camera/CameraComponent.h"
#include "Components/Meshes/MeshComponent.h"
#include "World/CEWorld.h"
#include "World/Levels/CELevel.h"

class TestWorld : public CE::CELevel
{
 public:
  TestWorld();
  virtual void Update(float DeltaTime) override;
  CE::CEActor* cameraActor;
};

void TestWorld::Update(float DeltaTime)
{
  CE::CELevel::Update(DeltaTime);

  static float totalTime = 0.0f;
  totalTime += DeltaTime;

  // Вращение вокруг оси Y (вертикальной) на 45 градусов в секунду
  float rotationSpeed = 45.0f;  // градусов в секунду
  float currentRotation = totalTime * rotationSpeed;

  // Постепенное отдаление камеры
  float initialDistance = 10.0f;  // начальная дистанция
  float moveSpeed = 2.0f;         // скорость отдаления (единиц в секунду)
  float currentDistance = initialDistance + (totalTime * moveSpeed);

  // Устанавливаем позицию камеры (вращение + отдаление)
  float x = 0.0f;
  float y = 0.0f;
  float z = currentDistance;

  cameraActor->SetActorLocation(glm::vec3(x, y, z));
  cameraActor->SetActorRotation(glm::vec3(0.f, 0.f, currentRotation));
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
  cameraActor->SetActorLocation(glm::vec3(0.0f, 0.0f, 10.0f));
  cameraActor->SetActorRotation(glm::vec3(0.0f, 0.0f, 0.0f));

  // Спавним меш-акторы
  auto* player = SpawnActor<CE::CEActor>(this, "Player");
  // auto* enemy = SpawnActor<CE::CEActor>(this, "Enemy");

  player->SetRootComponent(player->AddDefaultSubObject<CE::MeshComponent>("PlayerMesh", player, "PlayerMesh"));
  // enemy->SetRootComponent(enemy->AddDefaultSubObject<CE::MeshComponent>("EnemyMesh", enemy, "EnemyMesh"));

  // РАЗНЫЕ позиции для мешей
  player->SetActorLocation(glm::vec3(-2.0f, 0.0f, 0.0f));  // Слева на уровне земли
  // enemy->SetActorLocation(glm::vec3(2.0f, 1.5f, 0.0f));

  player->SetActorLocation(glm::vec3(-3.0f, 0.0f, 0.0f));
  // enemy->SetActorLocation(glm::vec3(3.0f, 0.0f, 0.0f));

  // Если есть метод SetActorScale
  player->SetActorScale(glm::vec3(1.0f, 1.0f, 1.0f));  // Нормальный размер
                                                       // enemy->SetActorScale(glm::vec3(1.5f, 1.5f, 1.5f));

  // Создаем несколько кубов в разных позициях
  // std::vector<glm::vec3> positions = {
  //     glm::vec3(-3.0f, 0.0f, 0.0f),  // Левый
  //     glm::vec3(0.0f, 0.0f, 0.0f),   // Центральный
  //     glm::vec3(3.0f, 0.0f, 0.0f),   // Правый
  //     glm::vec3(-1.5f, 2.0f, 0.0f),  // Верхний левый
  //     glm::vec3(1.5f, 2.0f, 0.0f)    // Верхний правый
  // };

  // std::vector<glm::vec3> colors = {
  //     glm::vec3(1.0f, 0.0f, 0.0f),  // Красный
  //     glm::vec3(0.0f, 1.0f, 0.0f),  // Зеленый
  //     glm::vec3(0.0f, 0.0f, 1.0f),  // Синий
  //     glm::vec3(1.0f, 1.0f, 0.0f),  // Желтый
  //     glm::vec3(1.0f, 0.0f, 1.0f)   // Пурпурный
  // };

  // for (int i = 0; i < (int)positions.size(); i++)
  // {
  //   auto* actor = SpawnActor<CE::CEActor>(this, "Cube_" + std::to_string(i));
  //   actor->SetRootComponent(actor->AddDefaultSubObject<CE::MeshComponent>("Mesh", actor, "Mesh"));
  //   actor->SetActorLocation(positions[i]);

  //    auto* mesh = dynamic_cast<CE::MeshComponent*>(actor->GetRootComponent());
  //    if (mesh)
  //    {
  //      mesh->SetColor(colors[i]);
  //    }
  //  }

  auto* playerMesh = dynamic_cast<CE::MeshComponent*>(player->GetRootComponent());
  //  auto* enemyMesh = dynamic_cast<CE::MeshComponent*>(enemy->GetRootComponent());

  if (playerMesh)
    playerMesh->SetColor(glm::vec3(0.2f, 0.8f, 0.2f));  // Зеленый
                                                        // if (enemyMesh)
                                                        //   enemyMesh->SetColor(glm::vec3(0.8f, 0.2f, 0.2f));  // Красный

  CE_CORE_DEBUG("TestWorld created with camera and mesh actors");
}

GameWorld::GameWorld() : CE::CEWorld(nullptr, "MainGameWorld")
{
  AddLevel(std::make_unique<TestWorld>());
  SetCurrentLevel("TestWorld");
}