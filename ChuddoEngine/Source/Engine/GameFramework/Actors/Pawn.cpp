#include "Actors/Pawn.h"
#include "Actors/PlayerController.h"
#include "Components/TransformComponent.h"
#include "Components/GravityComponent.h"
#include "Components/InputComponent.h"
#include "Components/Meshes/StaticMeshComponent.h"
#include "Utils/Math/Quaternion.h"
#include "World/World.h"
#include "World/Level.h"

CPawn::CPawn ( CObject * inOwner, const std::string & inDisplayName )
	: Super ( inOwner, inDisplayName )
	{
	m_InputComponent = AddDefaultSubObject<CInputComponent> ( "InputComponent_" + GetName () );
	LOG_DEBUG ( "[PAWN] Created: ", GetName () );
	Mesh = AddDefaultSubObject< CStaticMeshComponent> ( "PawnCube" );
	Mesh->AttachTo ( RootComponent );
	}

CPawn::~CPawn ()
	{
	if (Controller)
		{
		Controller->Unpossess ();
		Controller = nullptr;
		}
	}

void CPawn::SetController ( CPlayerController * NewController )
	{
	Controller = NewController;
	}



void CPawn::AddMovementInput ( const FVector & WorldDirection, float ScaleValue )
	{
	if (!bInputEnabled) return;


	 // Опционально: нормализуем направление, если это ещё не сделано
	FVector NormalizedDirection = WorldDirection;
	if (!NormalizedDirection.IsZero ())
		{
		NormalizedDirection.Normalize ();
		}

	FVector Movement = NormalizedDirection * ScaleValue;

	CActor::MoveActor ( Movement );
	}

bool CPawn::IsInputEnabled () const
	{
	if (Controller)
		{
		if (Controller->GetPawn () == this)
			{
			return bInputEnabled;
			}
		}
	return false;
	}

void CPawn::ProcessPlayerInput ( float DeltaTime )
	{
	if (!Controller || !bInputEnabled)
		{
		LOG_WARN ( "NO CONTROLLER OR INPUT DISABLED" );
		return;
		}
	Controller->ProcessPlayerInput ( DeltaTime );
	}

void CPawn::Jump (  )
	{
	LOG_DEBUG (
		"Spawning actor by pressing space bar! ^_^"
	);
	float dist = 25.f;
	FVector location = GetActorLocation ();
	FVector direction = FVector::Forward ()*dist;
	FVector spawnlocation = location + direction;
	auto spawned =GetWorld ()->GetCurrentLevel ()->SpawnActorAtLocation ("CActor","TEST", spawnlocation );
	CStaticMeshComponent* spawnedmesh = spawned->AddDefaultSubObject<CStaticMeshComponent> ( "SpawnedCube" );
	spawnedmesh->AttachTo ( spawned->GetRootComponent () );
	spawned->BeginPlay ();
	}

void CPawn::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );

	if (GetController () != nullptr)
		{
		ProcessPlayerInput ( DeltaTime );
		}
	}

void CPawn::BeginPlay ()
	{
	Super::BeginPlay ();
	LOG_DEBUG ( "[PAWN] BeginPlay: ", GetName () );
	}

void CPawn::EndPlay ()
	{
	Super::EndPlay ();
	LOG_DEBUG ( "[PAWN] EndPlay: ", GetName () );
	for (auto comp : ActorComponents)
		{
		if (comp == m_InputComponent)
			{
			m_InputComponent->OnEndPlay ();
			}
		}
	}

void CPawn::OnPossessed ( CPlayerController * NewController )
	{
	if (NewController == nullptr) return;
	SetController ( NewController );

	}

void CPawn::OnUnpossessed ( CPlayerController * OldController )
	{
	if (OldController == nullptr) return;
	if (Controller == OldController)
		{
		SetController ( nullptr );
		}
	}
#include "Camera/CameraComponent.h"
void CPawn::OnPossess ()
	{
	CCameraComponent* camera = AddDefaultSubObject<CCameraComponent> (  "Camera" );
	camera->AttachTo ( Mesh );
	camera->SetRelativeLocation ( 0.f, 0.f, -5.f );
	if (m_InputComponent)
		SetupPlayerInputComponent ( m_InputComponent );
	}

void CPawn::MoveForward ( float Value )
	{
	float scale = 150.f * Value* GetWorld()->GetDeltaSeconds();
	FVector Direction = GetActorRotationQuat () * FVector::Forward ();
	AddMovementInput ( Direction, scale );
	}

void CPawn::MoveRight ( float Value )
	{	
	float scale = 150 * Value * GetWorld ()->GetDeltaSeconds ();
	
	auto rightDirection = GetActorRotationQuat () * FVector::Right ();
	
	AddMovementInput ( rightDirection, scale );
	}

#include <GLFW/glfw3.h>
void CPawn::SetupPlayerInputComponent ( CInputComponent * InputComponent )
	{
	LOG_DEBUG ( "[PAWN] SetupPlayerInputComponent for: ", GetName () );

	if (InputComponent)
		{
		InputComponent->BindAxis ( "MoveForward", GLFW_KEY_W, GLFW_KEY_S, [ this ] ( float value ) { MoveForward ( value ); } );
		InputComponent->BindAxis ( "MoveRight", GLFW_KEY_A, GLFW_KEY_D, [ this ] ( float value ) { MoveRight ( value ); } );
		InputComponent->BindAction ( "SpawnCube", GLFW_KEY_SPACE, EInputEvent::IE_Pressed, [ this ] () { Jump (); } );
		m_InputComponent = InputComponent;
		
		}
	}