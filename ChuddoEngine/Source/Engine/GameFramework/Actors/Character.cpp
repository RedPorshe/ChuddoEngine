#include "Actors/Character.h"
#include "Components/Meshes/StaticMeshComponent.h"
#include "Components/Collisions/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/GravityComponent.h"
#include "Actors/TerrainActor.h"
#include "World/World.h"
#include "World/Level.h"

CCharacter::CCharacter ( CObject * inOwner, const std::string & DisplayName )
	: Super ( inOwner, DisplayName )
	{

	Capsule = AddDefaultSubObject<CCapsuleComponent> ("Capsule");
	SetRootComponent ( Capsule );
	Mesh = AddDefaultSubObject<CStaticMeshComponent> ("Mesh");
	Mesh->AttachTo ( Capsule );
	Camera = AddDefaultSubObject<CCameraComponent> ("Camera" );
	Camera->AttachTo ( Capsule );
	Camera->SetRelativeLocation (0.f,0.f,-5.f);
	Camera->SetRelativeRotation (0.f,0.f,0.f);
	
	}

void CCharacter::BeginPlay ()
	{
	Super::BeginPlay ();
	}

void CCharacter::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );
	
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
