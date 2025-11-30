#include "Game/World/MainLevel.h"

#include "Engine/GamePlay/Actors/SunActor.h"
#include "Engine/GamePlay/Actors/TerrainActor.h"
#include "Engine/Utils/Math/AllMath.h"
#include "Engine/GamePlay/Components/StaticMeshComponent.h"


MainLevel::MainLevel(CObject* Owner,
                     FString NewName) : CLevel(Owner, NewName)
{
  GAMEPLAY_DEBUG(this->GetName(), " Created ");
  playerController = SpawnActor<CPlayerController>(this, "Player Controller");
  playerCharacter = SpawnActor<CCharacter>(this, "Player Character");
  playerController->Possess(playerCharacter);

  playerCharacter->SetActorLocation(-2.f, 0.f, -20.f);
 
 

  // === СОЗДАЕМ ЛАНДШАФТ ===
  terrain = SpawnActor<TerrainActor>(this, "Terrain");
  if (terrain)
  {
    terrain->SetActorLocation(CEMath::Vector3f(-2.0f, -11.0f, -25.0f));
  }

 

  // Спавним меш-акторы
  enemy = SpawnActor<CActor>(this, "Enemy");
  enemy->SetRootComponent(enemy->AddSubObject<CStaticMeshComponent>("EnemyMesh", enemy, "EnemyMesh"));
  enemy->SetActorLocation(CEMath::Vector3f(.0f, 0.f, -5.0f));
  enemy->SetActorScale(5.f);
  enemy->SetActorRotation(CEMath::Vector3f(-90.0f, -90.0f, 0.0f));

 
  std::vector<CEMath::Vector3f> positions = {
      CEMath::Vector3f(-3.0f, 15.5f, 0.0f),  
      CEMath::Vector3f(0.0f, 15.5f, 0.0f),   
      CEMath::Vector3f(3.0f, 15.5f, 0.0f),   
      CEMath::Vector3f(-1.5f, 15.0f, 0.0f),
      CEMath::Vector3f(1.5f, 15.0f, 0.0f)};

  std::vector<CEMath::Vector3f> colors = {
      CEMath::Vector3f(1.0f, 0.0f, 0.0f),
      CEMath::Vector3f(0.0f, 1.0f, 0.0f),
      CEMath::Vector3f(0.0f, 0.0f, 1.0f),
      CEMath::Vector3f(1.0f, 1.0f, 0.0f),
      CEMath::Vector3f(1.0f, 0.0f, 1.0f)};

  for (int i = 0; i < (int)positions.size(); i++)
  {
    auto* actor = SpawnActor<CActor>(this, "Cube_" + std::to_string(i));
    actor->SetRootComponent(actor->AddSubObject<CStaticMeshComponent>("Mesh", actor, "Mesh"));
    actor->SetActorLocation(positions[i]);

    auto* mesh = dynamic_cast<CStaticMeshComponent*>(actor->GetRootComponent());
    if (mesh)
    {
      mesh->CreateCubeMesh();
      mesh->SetColor(colors[i]);
    }



    

    if (i == 1)
    {
      mesh->SetMesh("Assets/Meshes/test_cube.obj");
    }
    if (i == 2)
    {
      mesh->SetMesh("Assets/Meshes/Sphere.obj");
    }
    if (i == 4)
    {
      mesh->SetMesh("Assets/Meshes/Icosahedron.obj");
    }
  }

  for (int i = 0; i < (int)positions.size(); i++)
  {
    auto* actor = SpawnActor<CActor>(this, "Sphere_" + std::to_string(i));
    actor->SetRootComponent(actor->AddSubObject<CStaticMeshComponent>("Mesh", actor, "Mesh"));
    actor->SetActorLocation(positions[i]*static_cast<float>(i));

    auto* mesh = dynamic_cast<CStaticMeshComponent*>(actor->GetRootComponent());
    if (mesh)
    {
      mesh->CreateCubeMesh();
      mesh->SetColor(colors[i]);
    }



    if (i == 1)
    {
      mesh->SetMesh("Assets/Meshes/test_cube.obj");
    }
    if (i == 2)
    {
      mesh->SetMesh("Assets/Meshes/Sphere.obj");
    }
    if (i == 4)
    {
      mesh->SetMesh("Assets/Meshes/Icosahedron.obj");
    }
  }

  auto* enemyMesh = dynamic_cast<CStaticMeshComponent*>(enemy->GetRootComponent());
  if (enemyMesh)
  {
    enemyMesh->SetMesh("Assets/Meshes/VikingRoom.obj");
    enemyMesh->SetColor(CEMath::Vector3f(0.8f, 0.2f, 0.2f));
  }

  // Spawn a SunActor to control world's directional/positional "sun" light
  auto* sun = SpawnActor<SunActor>(this, "SunActor");
  if (sun)
  {
    sun->SetColor(CEMath::Vector3f(1.0f, 1.f, 1.f));
    sun->SetIntensity(2.0f);
    sun->SetRadius(100.0f);
    sun->SetAngularSpeed(0.025f);
    sun->SetActorLocation(CEMath::Vector3f(0.0f, 1000.0f, 0.0f));
  }

 
 
}
void MainLevel::BeginPlay()
{
  CLevel::BeginPlay();
}

void MainLevel::Tick(float DeltaTime)
{
  CLevel::Tick(DeltaTime);
}
void MainLevel::Update(float DeltaTime)
{
  CLevel::Update(DeltaTime);  
}