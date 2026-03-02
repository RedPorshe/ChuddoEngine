#include "Actors/Pawn.h"
#include "Actors/PlayerController.h"
#include "Components/TransformComponent.h"
#include "Components/GravityComponent.h"
#include "Components/MovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/Meshes/StaticMeshComponent.h"
#include "Utils/Math/Quaternion.h"
#include "World/World.h"
#include "World/Level.h"

CPawn::CPawn ( CObject * inOwner, const std::string & inDisplayName )
	: Super ( inOwner, inDisplayName )
	{
	m_InputComponent = AddDefaultSubObject<CInputComponent> ( "InputComponent_" + GetName () );
	m_InputComponent->AttachComponentToComponent ( RootComponent );
	SetMovableState ( EMovableState::DYNAMIC );
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

	float DeltaTime = GetWorld ()->GetDeltaSeconds ();

	FVector NormalizedDirection = WorldDirection;
	if (!NormalizedDirection.IsZero ())
		{
		NormalizedDirection.Normalize ();
		}

	// Получаем текущую скорость и состояние гравитации
	bool bIsGrounded = m_Gravity ? m_Gravity->IsGrounded () : false;

	// Вычисляем желаемое ускорение
	float desiredSpeed = bIsGrounded ? m_GroundSpeed : m_MaxAirSpeed;
	float airControlFactor = bIsGrounded ? 1.0f : m_AirControl;

	FVector desiredVelocity = NormalizedDirection * desiredSpeed * ScaleValue * airControlFactor;

	// Плавно изменяем скорость (интерполяция)
	float smoothness = bIsGrounded ? 8.0f : 4.0f; // Быстрее на земле, медленнее в воздухе
	m_Velocity = FVector::Lerp ( m_Velocity, desiredVelocity, smoothness * DeltaTime );

	// Ограничиваем скорость
	float maxSpeed = bIsGrounded ? m_GroundSpeed : m_MaxAirSpeed;
	if (m_Velocity.Length () > maxSpeed)
		{
		m_Velocity = m_Velocity.Normalized () * maxSpeed;
		}

	// В воздухе сохраняем вертикальную скорость от гравитации
	if (!bIsGrounded && m_Gravity)
		{
		m_Velocity.y = m_Gravity->GetVerticalVelocity () * DeltaTime;
		}

	// Применяем движение
	MoveActor ( m_Velocity * DeltaTime );
	}

void CPawn::AddMovementInput ( const FVector & WorldDirection, float ScaleValue, bool bForce )
	{}

void CPawn::AddControllerYawInput ( float Val )
	{
	if (MovementComponent && IsInputEnabled ())
		{
		MovementComponent->AddYawInput ( Val );
		}
	}

void CPawn::AddControllerPitchInput ( float Val )
	{
	if (MovementComponent && IsInputEnabled ())
		{
		MovementComponent->AddPitchInput ( Val );
		}
	}

void CPawn::AddControllerRollInput ( float Val )
	{
	if (MovementComponent && IsInputEnabled ())
		{
		MovementComponent->AddRollInput ( Val );
		}
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
		return;
		}
	Controller->ProcessPlayerInput ( DeltaTime );
	}

void CPawn::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );

	if (GetController () != nullptr)
		{
		ProcessPlayerInput ( DeltaTime );
		}

	// Обновляем скорость на основе гравитации, если не на земле
	if (m_Gravity && !m_Gravity->IsGrounded ())
		{
		m_Velocity.y = m_Gravity->GetVerticalVelocity () * DeltaTime;
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

void CPawn::OnPossess ()
	{
	if (m_InputComponent)
		SetupPlayerInputComponent ( m_InputComponent );
	}

void CPawn::SetupPlayerInputComponent ( CInputComponent * InputComponent )
	{
	LOG_DEBUG ( "[PAWN] SetupPlayerInputComponent for: ", GetName () );

	if (InputComponent)
		{
		m_InputComponent = InputComponent;
		}
	}