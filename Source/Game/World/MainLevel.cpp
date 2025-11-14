#include "Game/World/MainLevel.h"

#include "Engine/GamePlay/Actors/SunActor.h"
#include "Engine/GamePlay/Components/BoxComponent.h"
#include "Engine/GamePlay/Components/CEStaticMeshComponent.h"
#include "Engine/GamePlay/Components/PlaneComponent.h"
#include "Engine/GamePlay/Components/SphereComponent.h"
#include "Game/Actors/PlayerCharacter.h"

MainLevel::MainLevel(CE::CEObject* Owner,
                     CE::FString NewName) : CE::CELevel(Owner, NewName)
{
  CE_GAMEPLAY_DEBUG(this->GetName(), " Created ");
  playerController = SpawnActor<CE::PlayerController>(this, "Player Controller");
  playerCharacter = SpawnActor<PlayerCharacter>(this, "Player Character");
  playerController->Possess(playerCharacter);

  playerCharacter->SetActorLocation(1.f, 1.f, 1.f);

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

  auto* DirectLighter = SpawnActor<CE::SunActor>(this, "DirectLight");
  DirectLighter->SetIntensity(10.f);
  DirectLighter->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
  DirectLighter->SetActorLocation(glm::vec3(0.0f, 0.0f, 0.0f));

  // Спавним меш-акторы
  auto* enemy = SpawnActor<CE::CEActor>(this, "Enemy");
  enemy->SetRootComponent(enemy->AddSubObject<CE::CEStaticMeshComponent>("EnemyMesh", enemy, "EnemyMesh"));
  enemy->SetActorLocation(glm::vec3(3.0f, 0.f, 5.0f));
  enemy->SetActorScale(5.f);
  enemy->SetActorRotation(glm::vec3(90.0f, 180.0f, -90.0f));

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
    auto* actor = SpawnActor<CE::CEActor>(this, "Sphere_" + std::to_string(i));
    actor->SetRootComponent(actor->AddSubObject<CE::CEStaticMeshComponent>("Mesh", actor, "Mesh"));
    actor->SetActorLocation(positions[i]);

    auto* mesh = dynamic_cast<CE::CEStaticMeshComponent*>(actor->GetRootComponent());
    if (mesh)
    {
      mesh->SetColor(colors[i]);
      mesh->SetMesh("Assets/Meshes/Sphere.obj");
    }
  }

  auto* enemyMesh = dynamic_cast<CE::CEStaticMeshComponent*>(enemy->GetRootComponent());
  if (enemyMesh)
  {
    enemyMesh->SetMesh("Assets/Meshes/VikingRoom.obj");
    enemyMesh->SetColor(glm::vec3(0.8f, 0.2f, 0.2f));
  }

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
  auto* collisionTestBox = SpawnActor<CE::CEActor>(this, "CollisionBox");
  auto* boxCollision = collisionTestBox->AddDefaultSubObject<CE::CEBoxComponent>("BoxCollision", collisionTestBox, "BoxCollision");
  collisionTestBox->SetRootComponent(boxCollision);
  collisionTestBox->SetActorLocation(glm::vec3(8.0f, 1.0f, 8.0f));  // Над землей
  boxCollision->SetExtents(0.5f, 0.5f, 0.5f);
  boxCollision->SetCollisionEnabled(true);
  boxCollision->SetGenerateOverlapEvents(true);

  // 2. Сфера для коллизий
  auto* collisionTestSphere = SpawnActor<CE::CEActor>(this, "CollisionSphere");
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
}
void MainLevel::BeginPlay()
{
  CE::CELevel::BeginPlay();
}

void MainLevel::Tick(float DeltaTime)
{
  CE::CELevel::Tick(DeltaTime);
}
void MainLevel::Update(float DeltaTime)
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

    if (actor == playerController || actor == playerCharacter || actor->GetName() == "GroundActor" || actor->GetName() == "Enemy" || actor->GetName() == "SunActor")
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