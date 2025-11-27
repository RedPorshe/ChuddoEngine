#include "Game/World/MainLevel.h"

#include "Engine/GamePlay/Actors/SunActor.h"
#include "Engine/GamePlay/Actors/TerrainActor.h"
#include "Engine/Utils/Math/AllMath.h"
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
    terrain->SetActorLocation(CE::Math::Vector3f(0.0f, -2.0f, 0.0f));
  }

 

  // Спавним меш-акторы
  enemy = SpawnActor<CE::CActor>(this, "Enemy");
  enemy->SetRootComponent(enemy->AddSubObject<CE::CStaticMeshComponent>("EnemyMesh", enemy, "EnemyMesh"));
  enemy->SetActorLocation(CE::Math::Vector3f(.0f, 0.f, -5.0f));
  enemy->SetActorScale(5.f);
  enemy->SetActorRotation(CE::Math::Vector3f(-90.0f, -90.0f, 0.0f));

 
  std::vector<CE::Math::Vector3f> positions = {
      CE::Math::Vector3f(-3.0f, 0.5f, 0.0f),  
      CE::Math::Vector3f(0.0f, 0.5f, 0.0f),   
      CE::Math::Vector3f(3.0f, 0.5f, 0.0f),   
      CE::Math::Vector3f(-1.5f, 2.0f, 0.0f),
      CE::Math::Vector3f(1.5f, 2.0f, 0.0f)};

  std::vector<CE::Math::Vector3f> colors = {
      CE::Math::Vector3f(1.0f, 0.0f, 0.0f),
      CE::Math::Vector3f(0.0f, 1.0f, 0.0f),
      CE::Math::Vector3f(0.0f, 0.0f, 1.0f),
      CE::Math::Vector3f(1.0f, 1.0f, 0.0f),
      CE::Math::Vector3f(1.0f, 0.0f, 1.0f)};

  for (int i = 0; i < (int)positions.size(); i++)
  {
    auto* actor = SpawnActor<CE::CActor>(this, "Sphere_" + std::to_string(i));
    actor->SetRootComponent(actor->AddSubObject<CE::CStaticMeshComponent>("Mesh", actor, "Mesh"));
    actor->SetActorLocation(positions[i]);

    auto* mesh = dynamic_cast<CE::CStaticMeshComponent*>(actor->GetRootComponent());
    if (mesh)
    {
      mesh->CreateCubeMesh();
      mesh->SetColor(colors[i]);
    }
    // actor->SetIsStatic(false);
    
     actor->SetUseGravity(false);
     if (i == 1) 
    {
      mesh->SetMesh("Assets/Meshes/test_cube.obj");
      mesh->SetColor(colors[i]);
    }
    if (i == 2) 
    {
      mesh->SetMesh("Assets/Meshes/Sphere.obj");
       mesh->SetColor(colors[i]);
    }
   
    if ( i ==4 )
    {
      mesh->SetMesh("Assets/Meshes/Icosahedron.obj");
       mesh->SetColor(colors[i]);
    }
  }

  for (int i = 0; i < (int)positions.size(); i++)
  {
    auto* actor = SpawnActor<CE::CActor>(this, "Sphere_" + std::to_string(i));
    actor->SetRootComponent(actor->AddSubObject<CE::CStaticMeshComponent>("Mesh", actor, "Mesh"));
    actor->SetActorLocation(positions[i]*static_cast<float>(i));

    auto* mesh = dynamic_cast<CE::CStaticMeshComponent*>(actor->GetRootComponent());
    if (mesh)
    {
      mesh->CreateCubeMesh();
      mesh->SetColor(colors[i]);
    }
    // actor->SetIsStatic(false);
    actor->SetUseGravity(false);
     if (i == 1) 
    {
      mesh->SetMesh("Assets/Meshes/test_cube.obj");
    }
    if (i == 2) 
    {
      mesh->SetMesh("Assets/Meshes/Sphere.obj");
    }
   
    if ( i ==4 )
    {
      mesh->SetMesh("Assets/Meshes/Icosahedron.obj");
    }
  }

  auto* enemyMesh = dynamic_cast<CE::CStaticMeshComponent*>(enemy->GetRootComponent());
  if (enemyMesh)
  {
    enemyMesh->SetMesh("Assets/Meshes/VikingRoom.obj");
    enemyMesh->SetColor(CE::Math::Vector3f(0.8f, 0.2f, 0.2f));
  }

  // Spawn a SunActor to control world's directional/positional "sun" light
  auto* sun = SpawnActor<CE::SunActor>(this, "SunActor");
  if (sun)
  {
    sun->SetColor(CE::Math::Vector3f(1.0f, 1.f, 1.f));
    sun->SetIntensity(2.0f);
    sun->SetRadius(100.0f);
    sun->SetAngularSpeed(0.025f);
    sun->SetActorLocation(CE::Math::Vector3f(0.0f, 1000.0f, 0.0f));
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
    CE::Math::Vector3f rot = CE::Math::Vector3f(
        totalTime * CE::Math::ToRadians(speed * 1.5f) * random, totalTime * CE::Math::ToRadians(speed * 1.5f) * random, totalTime * CE::Math::ToRadians(speed * 1.5f) * random);

    actor->SetActorRotation(rot);
    ++idx;
  }
}