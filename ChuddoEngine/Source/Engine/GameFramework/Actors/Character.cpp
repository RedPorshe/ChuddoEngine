#include "Actors/Character.h"
#include "Components/Meshes/StaticMeshComponent.h"
#include "Components/Collisions/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/GravityComponent.h"
#include "Actors/TerrainActor.h"
#include "World/World.h"
#include "World/Level.h"

#include "Core/Engine.h"
#include "Render/Renderer.h"

CCharacter::CCharacter ( CObject * inOwner, const std::string & DisplayName )
	: Super ( inOwner, DisplayName )
	{
	Capsule = AddDefaultSubObject<CCapsuleComponent> ( "Capsule" );
	Capsule->SetRadius ( 18.f );
	Capsule->SetHalfHeight ( 9.f );
	SetRootComponent ( Capsule );

	Mesh = AddDefaultSubObject<CStaticMeshComponent> ( "Mesh" );
	Mesh->AttachTo ( Capsule );
	
	// ВАЖНО: Генерируем геометрию для меша
	Mesh->CreateFallBackCube (); // Вызываем метод создания куба

	Camera = AddDefaultSubObject<CCameraComponent> ( "Camera" );
	Camera->AttachTo ( Capsule );
	Camera->SetRelativeLocation ( 0.f, 0.f, -10.f ); // Поднимаем камеру на уровень глаз
	Camera->SetRelativeRotation ( 0.f, 0.f, 0.f );
	
	


	}

void CCharacter::BeginPlay ()
	{
	Super::BeginPlay ();
	
	 // Убеждаемся, что меш создан
	if (Mesh && !Mesh->HasRenderResources ())
		{
		if (CEngine::Get ().GetRenderer ())
			{
			auto  bufferManager = CEngine::Get ().GetRenderer ()->GetBufferManager ();
			if (bufferManager)
				{
				LOG_DEBUG ( "[CHARACTER] Creating mesh resources..." );
				Mesh->CreateRenderResources ( bufferManager );
				if (!Mesh->IsReadyForRender ())
					{
					Mesh->SetVisible ( true );
					Mesh->SetPipelineName ( "StaticMesh" );
					}
				}
			}
		}	
	
	}

void CCharacter::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );
	static float timer = 0.f;
	timer += DeltaTime;
	if (timer >= 1.f )
		{
		/*auto rot = RootComponent->GetRotation ();
		rot.z += 5.f;
		RootComponent->SetRotation ( rot );*/
		
		 


		}
	}

void CCharacter::EndPlay ()
	{
	Super::EndPlay ();
	}

void CCharacter::SetupPlayerInputComponent ( CInputComponent * InputComponent )
	{
	Super::SetupPlayerInputComponent ( InputComponent );
	if (GetInputComponent ())
		{
		GetInputComponent ()->BindAxis ( "MoveForward", GLFW_KEY_W, GLFW_KEY_S,
										 [ this ] ( float value ) { MoveForward ( value ); } );
		GetInputComponent ()->BindAxis ( "MoveRight", GLFW_KEY_D, GLFW_KEY_A,
										 [ this ] ( float value ) { MoveRight ( value ); } );
		GetInputComponent ()->BindAxis ( "MoveUp", GLFW_KEY_E, GLFW_KEY_Q,
										 [ this ] ( float value ) { MoveUp ( value ); } );
		GetInputComponent ()->BindAction ( "Jump", GLFW_KEY_SPACE, EInputEvent::IE_Pressed,
										   [ this ] () { Jump (); } );
		GetInputComponent ()->BindAction ( "SpawnCube", GLFW_KEY_TAB, EInputEvent::IE_Pressed,
										   [ this ] () { SpawnCube (); } );

		}
	}

void CCharacter::MoveRight ( float value )
	{
	if ( value != 0.f)
		{
		FVector Direction = GetActorRotationQuat ()*FVector::Right();
		float scale = MoveSpeed * value;
		AddMovementInput ( Direction, scale );
		}
	}

void CCharacter::MoveForward ( float Value )
	{
	if (Value != 0.f)
		{
		
		FVector Direction = GetActorRotationQuat () * FVector::Forward ();
		
		float scale = MoveSpeed * Value;
		
		AddMovementInput ( Direction, scale );
		}
	}

void CCharacter::MoveUp ( float Value )
	{
	if (Value != 0.f)
		{

		FVector Direction = GetActorRotationQuat () * FVector::Up ();

		float scale = MoveSpeed * Value;

		AddMovementInput ( Direction, scale );
		}
	}

void CCharacter::Jump ()
	{
	if (m_Gravity && m_Gravity->IsGrounded ())
		{
		LOG_DEBUG ( "JUMP!" );
		// Даем импульс вверх
		AddMovementInput ( FVector::Up(), JumpForce);
		m_Gravity->IsGrounded ();
		}
	}

void CCharacter::SpawnCube ()
	{
	if (!GetWorld () || !GetWorld ()->GetCurrentLevel ())
		{
		LOG_ERROR ( "[CHARACTER] Cannot spawn cube - no world or level!" );
		return;
		}

	auto level = GetWorld ()->GetCurrentLevel ();

	// Вычисляем позицию для спавна (немного впереди и справа от персонажа)
	FVector right = Camera->GetRotationQuat ()*FVector::Right();
	FVector forward = Camera->GetRotationQuat () * FVector::Forward ();
	FVector spawnDirection = ( right + forward ).Normalize ();
	FVector spawnlocation = GetActorLocation () + spawnDirection * 5.0f; // 200 единиц от персонажа

	// Поднимаем куб над землёй
	spawnlocation.y += 50.0f;

	LOG_DEBUG ( "[CHARACTER] Spawning test cube at: (",
				spawnlocation.x, ", ", spawnlocation.y, ", ", spawnlocation.z, ")" );
	LOG_DEBUG ( "[CHARACTER] Player position: (",
				GetActorLocation ().x, ", ", GetActorLocation ().y, ", ", GetActorLocation ().z, ")" );

	  // Спавним актор
	auto cubeActor = level->SpawnActorAtLocation ( "CActor", "TestActor", spawnlocation );
	if (!cubeActor)
		{
		LOG_ERROR ( "[CHARACTER] Failed to spawn cube actor!" );
		return;
		}

		// Создаём меш
	auto cubemesh = cubeActor->AddDefaultSubObject<CStaticMeshComponent> ( "Testmesh" );
	if (!cubemesh)
		{
		LOG_ERROR ( "[CHARACTER] Failed to create mesh for cube!" );
		return;
		}

		// Настраиваем меш
	cubeActor->SetRootComponent ( cubemesh );
	cubemesh->CreateFallBackCube ();
	cubemesh->SetPipelineName ( "StaticMesh" ); // Убеждаемся, что используем правильный пайплайн
	
	

	// Важно: устанавливаем видимость
	cubemesh->SetVisible ( true );

	// Запускаем актор
	cubeActor->BeginPlay ();

	LOG_DEBUG ( "[CHARACTER] Test cube spawned successfully!" );
	}
