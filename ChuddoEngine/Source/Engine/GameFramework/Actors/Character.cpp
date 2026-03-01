#include "Actors/Character.h"
#include "Components/Meshes/StaticMeshComponent.h"
#include "Components/Collisions/CapsuleComponent.h"
#include "Components/Collisions/BoxComponent.h"
#include "Components/Collisions/ConeComponent.h"
#include "Components/Collisions/CylinderComponent.h"
#include "Components/Collisions/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/GravityComponent.h"
#include "Actors/TerrainActor.h"
#include "World/World.h"
#include "World/Level.h"

#include "Core/Engine.h"
#include "Render/Renderer.h"
#include "Utils/Math/CE_MathHelpers.h"
#include <cmath>

CCharacter::CCharacter ( CObject * inOwner, const std::string & DisplayName )
	: Super ( inOwner, DisplayName )
	{
	Capsule = AddDefaultSubObject<CCapsuleComponent> ( "Capsule" );

	
	Capsule->SetHalfHeight ( 9.f );
	Capsule->SetRadius ( 1.8f );
	SetRootComponent ( Capsule );
	SetDrawCollisions ( true ); 
	Mesh = AddDefaultSubObject<CStaticMeshComponent> ( "Mesh" );
	Mesh->AttachTo ( Capsule );
	Mesh->SetRelativeLocation ( 0.f,0.f,0.f );
	// ВАЖНО: Генерируем геометрию для меша
	Mesh->CreateFallBackCube ();	
	Mesh->SetScale ( 0.5f );
	Camera = AddDefaultSubObject<CCameraComponent> ( "Camera" );
	Camera->AttachTo ( Capsule );
	Camera->SetRelativeLocation ( 0.f, 18.f, -20.f ); // Поднимаем камеру на уровень глаз
	Camera->SetRelativeRotation ( 45.f, 0.f, 0.f );

	// Настройки движения
	m_GroundSpeed = 600.0f;
	m_MaxAirSpeed = 400.0f;
	m_AirControl = 0.8f; // 80% контроля в воздухе

	// Настройки прыжка
	bIsJumping = false;
	TargetJumpHeight = 0.f;
	
	JumpHeight = 10.0f;  
	SetActorLocation ( { 500.f, 500.3322f, 500.f } );
	}

void CCharacter::BeginPlay ()
	{
	Super::BeginPlay ();	
	GetActorLocation ();
	}

bool CCharacter::CheckTargetJump ()
	{
	float CurrentHeight = GetActorLocation ().y;
	// Добавляем небольшую погрешность
	return CurrentHeight >= TargetJumpHeight - 1.2f;
	}


void CCharacter::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );

	// Логика прыжка - только если мы в воздухе и прыжок активен
	if (bIsJumping && m_Gravity && !m_Gravity->IsGrounded ())
		{
		float CurrentHeight = GetActorLocation ().y;
		float CurrentVel = m_Gravity->GetVerticalVelocity ();

		// Проверяем, достигли ли пика (скорость стала <= 0)
		if (CurrentVel <= 0)
			{
			if (CheckTargetJump ())
				{
				bIsJumping = false;
				LOG_DEBUG ( "[JUMP PEAK] Достигнута высота: ", CurrentHeight,
							" Цель: ", TargetJumpHeight );
				}
			}
		}
 
	if (m_Gravity && m_Gravity->IsGrounded () && bIsJumping&& CheckTargetJump ())
		{
		bIsJumping = false;
		LOG_DEBUG ( " ON GROUND - ready to jump again" );
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
		GetInputComponent ()->BindAction ( "Spawn", GLFW_KEY_F,EInputEvent::IE_Pressed,
										   [ this ] () { SpawnCube (); } );

		}
	}

void CCharacter::MoveRight ( float value )
	{
	if (value != 0.f)
		{
		FVector Direction = GetActorRotationQuat () * FVector::Right ();
		AddMovementInput ( Direction, value );
		}
	}

void CCharacter::MoveForward ( float Value )
	{
	if (Value != 0.f)
		{
		FVector Direction = GetActorRotationQuat () * FVector::Forward ();
		AddMovementInput ( Direction, Value );
		}
	}

void CCharacter::MoveUp ( float Value )
	{
	if (Value != 0.f)
		{
		FVector Direction = GetActorRotationQuat () * FVector::Up ();
		AddMovementInput ( Direction, Value );
		}
	}

void CCharacter::StartJump ()
	{
	if (m_Gravity && m_Gravity->IsGrounded ())
		{
		// Получаем силу гравитации
		float gravity = m_Gravity->GetGravityStrength () * m_Gravity->GetGravityScale ();

		// Вычисляем нужную скорость для достижения JumpHeight
		// v = sqrt(2 * g * h)
		float neededVelocity = std::sqrt ( 2.0f * gravity * JumpHeight );

		// Устанавливаем целевую высоту
		TargetJumpHeight = GetActorLocation ().y + JumpHeight;

		// Даем вычисленную силу
		m_Gravity->SetVerticalVelocity ( neededVelocity );

		bIsJumping = true;

		LOG_DEBUG ( "[JUMP START] Цель: ", TargetJumpHeight,
					" Текущая: ", GetActorLocation ().y,
					" Сила: ", neededVelocity,
					" Гравитация: ", gravity,
					" Высота: ", JumpHeight );
		}
	}

void CCharacter::EndJump ()
	{
	// При отпускании клавиши ничего не делаем
	// Прыжок уже в процессе
	LOG_DEBUG ( "[JUMP END]" );
	}

void CCharacter::Jump ()
	{
	if (!bIsJumping && m_Gravity && m_Gravity->IsGrounded ())
		{
		StartJump ();
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

	// Вычисляем позицию для спавна
	FVector SpawnOffset = GetActorForwardVector()*-200.f;
	FVector spawnLocation = GetActorLocation () + SpawnOffset*2.f;

	// Спавним актор
	auto cubeActor = level->SpawnActor <CActor> ( "TestActor", spawnLocation );
	if (!cubeActor)
		{
		LOG_ERROR ( "[CHARACTER] Failed to spawn cube actor!" );
		return;
		}

	// Создаём меш и устанавливаем как RootComponent
	auto cubemesh = cubeActor->AddDefaultSubObject<CStaticMeshComponent> ( "Testmesh" );
	if (!cubemesh)
		{
		LOG_ERROR ( "[CHARACTER] Failed to create mesh for cube!" );
		return;
		}

	// ВАЖНО: Сначала устанавливаем RootComponent
	cubeActor->SetRootComponent ( cubemesh );

	
	
	
	
	

	// ТЕПЕРЬ устанавливаем позицию, когда RootComponent уже есть
	cubeActor->SetActorLocation ( spawnLocation, true );

	// Запускаем актор
	if(GetWorld()->GetGameMode()->IsGameStarted())
	cubeActor->BeginPlay ();
	
	LOG_DEBUG ( "[CHARACTER] Player position: ", GetActorLocation () );
	LOG_DEBUG ( "[CHARACTER] Test cube spawned at: ", cubeActor->GetActorLocation () );
	LOG_DEBUG ( "[CHARACTER] Test cube spawned successfully!" );
	}