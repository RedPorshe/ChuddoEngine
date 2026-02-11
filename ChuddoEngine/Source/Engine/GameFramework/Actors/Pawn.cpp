#include "Actors/Pawn.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "World/World.h"

CPawn::CPawn ( CObject * inOwner, const std::string & inDisplayName ) :Super(inOwner,inDisplayName)
	{
	CreateInputComponent ();
	}

CPawn::~CPawn ()
	{}

void CPawn::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );
	}

void CPawn::BeginPlay ()
	{
	Super::BeginPlay ();
	}

void CPawn::EndPlay ()
	{
	Super::EndPlay ();
	}

void CPawn::SetupInputComponent ( CInputComponent * InputComponent )
	{
	/* No bindings by default.*/
	}

void CPawn::AddMovementInput ( FVector WorldDirection, float ScaleValue, bool bForce )
	{
	 // Проверяем, включен ли ввод
	if (!bInputEnabled && !bForce)
		return;

	// Проверяем валидность направления
	if (WorldDirection.IsZero () || ScaleValue == 0.0f)
		return;

	// Нормализуем направление
	FVector NormalizedDirection = WorldDirection.Normalized ();

	// Вычисляем дельту перемещения
	//float DeltaTime = GetWorld ()->GetDeltaSeconds ();
	FVector DeltaMovement = NormalizedDirection * m_MoveSpeed * ScaleValue /**/;

	// Используем MoveActor из CActor для интерполированного движения
	MoveActor ( DeltaMovement, true );

	LOG_DEBUG ( "[CPawn] AddMovementInput: ", GetName (),
				" Direction=(", WorldDirection.x, ", ", WorldDirection.y, ", ", WorldDirection.z, ")",
				" Scale=", ScaleValue, " Delta=(", DeltaMovement.x, ", ", DeltaMovement.y, ", ", DeltaMovement.z, ")" );
	}

void CPawn::AddControllerYawInput ( float Val )
	{
	// Проверяем, включен ли ввод
	if (!bInputEnabled || Val == 0.0f)
		return;

	// Получаем дельту времени
	float DeltaTime = GetWorld ()->GetDeltaSeconds ();

	// Вычисляем угол поворота за этот кадр
	float YawDelta = Val * m_TurnSpeed * DeltaTime;

	if (bUseControlRotation)
		{
			// Вариант 1: Поворачиваем всего Pawn'а
		RotateActor ( FVector ( 0.0f, YawDelta, 0.0f ), true );
		}
	else
		{
			// Вариант 2: Только камера/контроллер (если нужно)
			// Здесь можно поворачивать только контроллер или камеру
		if (m_CameraComponent)
			{
			m_CameraComponent->AddLocalRotation ( FVector ( 0.0f, YawDelta, 0.0f ) );
			}
		}

	LOG_DEBUG ( "[CPawn] AddControllerYawInput: ", GetName (),
				" Val=", Val, " YawDelta=", YawDelta, " degrees" );
	}

void CPawn::AddControllerRollInput ( float Val )
	{
	// Проверяем, включен ли ввод
	if (!bInputEnabled || Val == 0.0f)
		return;

	// Получаем дельту времени
	float DeltaTime = GetWorld ()->GetDeltaSeconds ();

	// Вычисляем угол поворота за этот кадр
	float RollDelta = Val * m_TurnSpeed * DeltaTime;

	if (bUseControlRotation)
		{
			// Поворачиваем всего Pawn'а
		RotateActor ( FVector ( 0.0f, 0.0f, RollDelta ), true );
		}
	else
		{
			// Только камера
		if (m_CameraComponent)
			{
			m_CameraComponent->AddLocalRotation ( FVector ( 0.0f, 0.0f, RollDelta ) );
			}
		}

	LOG_DEBUG ( "[CPawn] AddControllerRollInput: ", GetName (),
				" Val=", Val, " RollDelta=", RollDelta, " degrees" );
	}

void CPawn::AddControllerPitchInput ( float Val )
	{
	// Проверяем, включен ли ввод
	if (!bInputEnabled || Val == 0.0f)
		return;

	// Получаем дельту времени
	float DeltaTime = GetWorld ()->GetDeltaSeconds ();

	// Вычисляем угол поворота за этот кадр
	float PitchDelta = -Val * m_LookUpSpeed * DeltaTime; // Минус для интуитивного управления

	if (bUseControlRotation)
		{
			// Вариант 1: Поворачиваем всего Pawn'а
		RotateActor ( FVector ( PitchDelta, 0.0f, 0.0f ), true );
		}
	else
		{
			// Вариант 2: Только камера (обычно так и делают)
		if (m_CameraComponent)
			{
			m_CameraComponent->AddLocalRotation ( FVector ( PitchDelta, 0.0f, 0.0f ) );

			// Ограничиваем наклон камеры (чтобы не перевернуться)
			FVector CameraRotation = m_CameraComponent->GetRelativeRotation ();
			CameraRotation.x = std::clamp ( CameraRotation.x, -80.0f, 80.0f );
			m_CameraComponent->SetRelativeRotation ( CameraRotation );
			}
		}

	LOG_DEBUG ( "[CPawn] AddControllerPitchInput: ", GetName (),
				" Val=", Val, " PitchDelta=", PitchDelta, " degrees" );
	}

void CPawn::OnPosses ()
	{
	if (m_InputComponent)
		{
		SetupInputComponent ( m_InputComponent );
		}
	}

void CPawn::PossesedBy (/*controller*/ )
	{
	/*check controller and set as Pawn controller*/
	OnPosses ();
	}

void CPawn::OnUnPosses ()
	{}

void CPawn::UnPosses (/*controller*/ )
	{
	//checks then call onunposses
	OnUnPosses ();
	}

void CPawn::CreateInputComponent ()
	{
	m_InputComponent = AddDefaultSubObject<CInputComponent> ( GetName () + "_InputComponent" );
	}
