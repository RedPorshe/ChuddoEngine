#include "Game/World/MainLevel.h"

#include "Engine/GamePlay/Actors/SunActor.h"
#include "Engine/GamePlay/Actors/TerrainActor.h"

#include "Engine/GamePlay/Components/CEStaticMeshComponent.h"


MainLevel::MainLevel(CE::CEObject* Owner,
                     CE::FString NewName) : CE::CELevel(Owner, NewName)
{
  CE_GAMEPLAY_DEBUG(this->GetName(), " Created ");
  playerController = SpawnActor<CE::PlayerController>(this, "Player Controller");
  playerCharacter = SpawnActor<CE::CEPawn>(this, "Player Character");
  playerController->Possess(playerCharacter);

  playerCharacter->SetActorLocation(1.f, 1.f, 1.f);

  // === СОЗДАЕМ ЛАНДШАФТ ===
  terrain = SpawnActor<CE::TerrainActor>(this, "Terrain");
  if (terrain)
  {
    terrain->SetActorLocation(glm::vec3(0.0f, -2.0f, 0.0f));
  }

 

  // Спавним меш-акторы
  enemy = SpawnActor<CE::CEActor>(this, "Enemy");
  enemy->SetRootComponent(enemy->AddSubObject<CE::CEStaticMeshComponent>("EnemyMesh", enemy, "EnemyMesh"));
  enemy->SetActorLocation(glm::vec3(.0f, 0.f, -5.0f));
  enemy->SetActorScale(5.f);
  enemy->SetActorRotation(glm::vec3(-90.0f, -90.0f, 0.0f));

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
      mesh->CreateCubeMesh();
    }
    // actor->SetIsStatic(false);
    actor->SetUseGravity(true);
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
    sun->SetColor(glm::vec3(1.0f, 1.f, 1.f));
    sun->SetIntensity(2.0f);
    sun->SetRadius(100.0f);
    sun->SetAngularSpeed(0.025f);
    sun->SetActorLocation(glm::vec3(0.0f, 1000.0f, 0.0f));
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
  static float random = 1.0f;
  totalTime += DeltaTime;

  // // Вращение кубов (как было)
  float rotationSpeed = 180.0f;
  int idx = 0;
  for (const auto& actorPtr : GetActors())
  {
    CE::CEActor* actor = actorPtr.get();
    if (!actor)
      continue;

    if (actor == playerController || actor == playerCharacter || actor->GetName() == "Terrain" || actor->GetName() == "Enemy" || actor->GetName() == "SunActor")
    {
      ++idx;
      continue;
    }
    random *= -1.f;
    float speed = rotationSpeed + (idx * 7.5f);
    glm::vec3 rot = glm::vec3(
        totalTime * glm::radians(speed * 1.5f) * random, totalTime * glm::radians(speed * 1.5f) * random, totalTime * glm::radians(speed * 1.5f) * random);

    actor->SetActorRotation(rot);
    ++idx;
  }
}