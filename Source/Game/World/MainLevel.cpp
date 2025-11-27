#include "Game/World/MainLevel.h"

#include "Engine/GamePlay/Actors/SunActor.h"
#include "Engine/GamePlay/Actors/TerrainActor.h"

#include "Engine/GamePlay/Components/StaticMeshComponent.h"


MainLevel::MainLevel(CE::CObject* Owner,
                     CE::FString NewName) : CE::CLevel(Owner, NewName)
{
  CE_GAMEPLAY_DEBUG(this->GetName(), " Created ");
  playerController = SpawnActor<CE::CPlayerController>(this, "Player Controller");
  playerCharacter = SpawnActor<CE::CPawn>(this, "Player Character");
  playerController->Possess(playerCharacter);

  playerCharacter->SetActorLocation(1.f, 1.f, 1.f);

  // === СОЗДАЕМ ЛАНДШАФТ ===
  terrain = SpawnActor<CE::TerrainActor>(this, "Terrain");
  if (terrain)
  {
    terrain->SetActorLocation(glm::vec3(0.0f, -2.0f, 0.0f));
  }

 

  // Спавним меш-акторы
  enemy = SpawnActor<CE::CActor>(this, "Enemy");
  enemy->SetRootComponent(enemy->AddSubObject<CE::CStaticMeshComponent>("EnemyMesh", enemy, "EnemyMesh"));
  enemy->SetActorLocation(glm::vec3(.0f, 0.f, -5.0f));
  enemy->SetActorScale(5.f);
  enemy->SetActorRotation(glm::vec3(-90.0f, -90.0f, 0.0f));

 
  std::vector<glm::vec3> positions = {
      glm::vec3(-3.0f, 0.5f, 0.0f),  
      glm::vec3(0.0f, 0.5f, 0.0f),   
      glm::vec3(3.0f, 0.5f, 0.0f),   
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
    auto* actor = SpawnActor<CE::CActor>(this, "Sphere_" + std::to_string(i));
    actor->SetRootComponent(actor->AddSubObject<CE::CStaticMeshComponent>("Mesh", actor, "Mesh"));
    actor->SetActorLocation(positions[i]);

    auto* mesh = dynamic_cast<CE::CStaticMeshComponent*>(actor->GetRootComponent());
    if (mesh)
    {
      mesh->SetColor(colors[i]);
      mesh->CreateCubeMesh();
    }
    // actor->SetIsStatic(false);
    actor->SetUseGravity(true);
  }

  for (int i = 0; i < (int)positions.size(); i++)
  {
    auto* actor = SpawnActor<CE::CActor>(this, "Sphere_" + std::to_string(i));
    actor->SetRootComponent(actor->AddSubObject<CE::CStaticMeshComponent>("Mesh", actor, "Mesh"));
    actor->SetActorLocation(colors[i]);

    auto* mesh = dynamic_cast<CE::CStaticMeshComponent*>(actor->GetRootComponent());
    if (mesh)
    {
      mesh->SetColor(colors[i]);
      mesh->CreateCubeMesh();
    }
    // actor->SetIsStatic(false);
    actor->SetUseGravity(false);
  }

  auto* enemyMesh = dynamic_cast<CE::CStaticMeshComponent*>(enemy->GetRootComponent());
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
  CE::CLevel::BeginPlay();
}

void MainLevel::Tick(float DeltaTime)
{
  CE::CLevel::Tick(DeltaTime);
}
void MainLevel::Update(float DeltaTime)
{
  CE::CLevel::Update(DeltaTime);

  static float totalTime = 0.0f;
  static float random = 1.0f;
  totalTime += DeltaTime;

  // // Вращение кубов (как было)
  float rotationSpeed = 180.0f;
  int idx = 0;
  for (const auto& actorPtr : GetActors())
  {
    CE::CActor* actor = actorPtr.get();
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