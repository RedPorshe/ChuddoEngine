#include "Actors/Character.h"
#include "Actors/TerrainActor.h"
#include "Components/Meshes/StaticMeshComponent.h"
#include "Components/Meshes/TerrainMeshComponent.h"
#include "Components/Collisions/CapsuleComponent.h"
#include "Components/Collisions/BoxComponent.h"
#include "Components/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/GravityComponent.h"

#include "World/World.h"
#include "World/Level.h"

CCharacter::CCharacter ( CObject * inOwner, const std::string & DisplayName )
	: Super ( inOwner, DisplayName )
	{
	CreateCharacterMovementComponent ();
	SetMovableState ( EMovableState::DYNAMIC );

	Capsule = AddDefaultSubObject<CCapsuleComponent> ( "Capsule" );
	Capsule->SetRadius ( 4.f );
	Capsule->SetHalfHeight ( 9.f );
	Capsule->AttachTo ( RootComponent );

	

	Mesh = AddDefaultSubObject<CStaticMeshComponent> ( "Mesh" );
	Mesh->AttachTo ( Capsule );
	Mesh->ResizeCube ( 1.f );
	

	Camera = AddDefaultSubObject<CCameraComponent> ( "Camera" );
	Camera->AttachTo ( Capsule );

	Camera->SetRelativeLocation (
		0.f,
		Capsule->GetHalfHeight (),
		-Capsule->GetRadius () * 10.0f
	);
		
	}

void CCharacter::CreateCharacterMovementComponent ()
	{
	if (MovementComponent)
		{
		RemoveOwnedObject ( MovementComponent->GetName () );
		MovementComponent = nullptr;
		}

	MovementComponent = AddDefaultSubObject<CCharacterMovementComponent> ( "CharMov" );

	if (MovementComponent)
		{
		MovementComponent->SetOwnerPawn ( this );
		LOG_DEBUG ( "[CHARACTER] Movement component created: ", MovementComponent->GetName () );
		}
	else
		{
		LOG_ERROR ( "[CHARACTER] Failed to create movement component" );
		}
	if (CCharacterMovementComponent * CharMov = dynamic_cast< CCharacterMovementComponent * >( MovementComponent ))
		{
		CharMov->SetUseControllRotationYaw ( true );   // Разрешаем поворот по горизонтали
		CharMov->SetUseControllRotationPitch ( true ); // Разрешаем поворот по вертикали
		CharMov->SetUseControllRotationRoll ( false ); // Запрещаем крен
		}
	}

void CCharacter::BeginPlay ()
	{
	Super::BeginPlay ();
	for (auto & actor : GetWorld ()->GetCurrentLevel ()->GetActors ())
		{
		for (auto & comp : actor->GetActorComponents ())
			{
			if (CTerrainMeshComponent * terMesh = dynamic_cast< CTerrainMeshComponent * >( comp ))
				{
				terrainMesh = terMesh;
				break;
				}
			}
		}
	}

void CCharacter::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );
	DebugInfo ( DeltaTime );
	}

void CCharacter::EndPlay ()
	{
	Super::EndPlay ();
	}

void CCharacter::SetupPlayerInputComponent ( CInputComponent * InputComponent )
	{
	Super::SetupPlayerInputComponent ( InputComponent );

	if (auto * Input = GetInputComponent ())
		{
		Input->BindAxis ( "MoveForward", EKeys::W, EKeys::S,
						  [ this ] ( float value ) { MoveForward ( value ); } );
		Input->BindAxis ( "MoveRight", GLFW_KEY_D, GLFW_KEY_A,
						  [ this ] ( float value ) { MoveRight ( value ); } );
		Input->BindAxis ( "MoveUp", GLFW_KEY_E, GLFW_KEY_Q,
						  [ this ] ( float value ) { MoveUp ( value ); } );
		Input->BindAction ( "Jump", GLFW_KEY_SPACE, EInputEvent::IE_Pressed,
							[ this ] () { Jump (); } );
		Input->BindAction ( "Spawn", GLFW_KEY_F, EInputEvent::IE_Pressed,
							[ this ] () { SpawnCube (); } );
		Input->BindMouseAxis ( "LookUp", EMouseAxis::MouseY,
							   [ this ] ( float value ) { LookUp ( value ); } );
		Input->BindMouseAxis ( "Turn", EMouseAxis::MouseX,
							   [ this ] ( float value ) { Turn ( value ); } );
		}
	}

void CCharacter::MoveRight ( float value )
	{
	if (value != 0.f && MovementComponent)
		{
		FVector Direction = GetActorRotationQuat () * FVector::Right ();
		AddMovementInput ( Direction, value );
		}
	}

void CCharacter::MoveForward ( float Value )
	{
	if (Value != 0.f && MovementComponent)
		{
		FVector Direction = GetActorRotationQuat () * FVector::Forward ();
		AddMovementInput ( Direction, Value );
		}
	}

void CCharacter::MoveUp ( float Value )
	{
	if (Value != 0.f && MovementComponent)
		{
		AddMovementInput ( FVector::Up (), Value );
		}
	}

void CCharacter::Jump ()
	{
	if (auto * CharMov = dynamic_cast< CCharacterMovementComponent * >( MovementComponent ))
		{
		CharMov->Jump ();
		}
	}

void CCharacter::LookUp ( float value )
	{
	if (value == 0.0f || !GetInputComponent () || !MovementComponent) return;

	   // Получаем чувствительность мыши
	float sensitivity = GetInputComponent ()->GetMouseSensevity ();
	float speed = GetInputComponent ()->GetLookSpeed ();
	// Вычисляем угол поворота в радианах
	float pitchValue = value * sensitivity * speed;
	AddControllerPitchInput ( pitchValue );
	}

void CCharacter::Turn ( float value )
	{
	if (value == 0.0f || !GetInputComponent () || !MovementComponent) return;

		// Получаем чувствительность мыши
	float sensitivity = GetInputComponent ()->GetMouseSensevity ();
	float speed = GetInputComponent ()->GetLookSpeed ();
	// Вычисляем угол поворота в радианах
	float yawValue = value * sensitivity * speed;
	AddControllerYawInput ( yawValue );

	}

bool CCharacter::IsJumping () const
	{
	if (auto * CharMov = dynamic_cast< CCharacterMovementComponent * >( MovementComponent ))
		{
		return CharMov->IsJumping ();
		}
	return false;
	}

void CCharacter::OnComponentBeginOverlap ( CBaseCollisionComponent * other )
	{
	Super::OnComponentBeginOverlap ( other );
	auto otherActor = other->GetOwnerActor ();
	LOG_DEBUG ( "[CHARACTER] Overlaped with ", otherActor->GetName () );
	}

void CCharacter::OnComponentEndOverlap ( CBaseCollisionComponent * other )
	{
	Super::OnComponentEndOverlap ( other );
	auto otherActor = other->GetOwnerActor ();
	LOG_DEBUG ( "[CHARACTER] End Overlap with ", otherActor->GetName () );
	}

void CCharacter::OnComponentHit ( CBaseCollisionComponent * other )
	{
	auto otherActor = other->GetOwnerActor ();
	if (CTerrainActor * terra = dynamic_cast< CTerrainActor * >( otherActor )) return;


	}

void CCharacter::SpawnCube ()
	{
	if (!GetWorld () || !GetWorld ()->GetCurrentLevel ()) return;

	static int spawnDirection = 0; // 0 = вправо, 1 = влево

	FVector forward = GetActorForwardVector ();
	FVector right = ( spawnDirection == 0 ) ? GetActorRightVector () : -GetActorRightVector ();
	FVector up = GetActorUpVector ();

	// Чередуем направление для следующего спавна
	spawnDirection = 1 - spawnDirection;

	FVector dir = up;
	dir.Normalize ();
	FVector offset = dir * 200.f;
	FVector spawnLocation = GetActorLocation () + offset;

	auto cubeActor = SpawnActor<CActor> ( "TestCube", spawnLocation );
	if (!cubeActor) return;

	auto cubemesh = cubeActor->AddDefaultSubObject<CStaticMeshComponent> ( "Testmesh" );
	auto box = cubeActor->AddDefaultSubObject<CBoxComponent> ( "CubeBox" );

	box->SetHalfExtents ( FVector ( 5.f, 5.f, 5.f ) );
	cubeActor->SetRootComponent ( cubemesh );
	cubemesh->SetCollisionComponent ( box );
	box->AttachTo ( cubemesh );
	box->SetChannelAsTrigger ();
	box->SetResponseToChannel ( ECollisionChannel::WorldDynamic, ECollisionResponse::IGNORE );
	cubeActor->SetCollisionEnabled ( true );
	cubeActor->SetActorLocation ( spawnLocation, true );
	cubeActor->SetMovableState ( EMovableState::DYNAMIC );
	cubeActor->InitializeAllComponents ();
	LOG_DEBUG ( "[CHARACTER] Test cube spawned at: ", cubeActor->GetActorLocation () );
	}

void CCharacter::DebugInfo ( float dt )
	{

	}