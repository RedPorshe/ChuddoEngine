#include "Actors/Actor.h"
#include "World/Level.h"
#include "World/World.h"
#include "GameInstance.h"
#include "Render/RenderInfo.h"
#include "Render/Vulkan/Managers/WireframePipeline.h"
#include "Components/Collisions/TerrainComponent.h"
#include "GameFramework/Components/BaseComponent.h"
#include "Components/Meshes/BaseMeshComponent.h"
#include "Components/Meshes/TerrainMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/Collisions/BaseCollisionComponent.h"
#include "Components/Collisions/BoxComponent.h"
#include "Components/Collisions/CapsuleComponent.h"
#include "Components/Collisions/ConeComponent.h"
#include "Components/Collisions/CylinderComponent.h"
#include "Components/Collisions/SphereComponent.h"
#include "Components/GravityComponent.h"


CActor::CActor ( CObject * owner, const std::string & inName ) : CObject ( owner, inName )
	{
	RootComponent = AddDefaultSubObject<CTransformComponent> ( inName + "_Transform" );
	m_Gravity = AddDefaultSubObject<CGravityComponent> ( GetName () + "_Gravity" );
	if (RootComponent)
		{
		RootComponent->SetCollisionEnabled ( bIsCollisionEnabled );
		}


	}

CActor::~CActor ()
	{
	ActorComponents.clear ();
	}

void CActor::BeginPlay ()
	{
	LOG_DEBUG ( "[ACTOR] BeginPlay: ", GetName () );
	for (auto comp : ActorComponents)
		{
		if (CTransformComponent * transform = dynamic_cast< CTransformComponent * >( comp ))
			{
			transform->UpdateTransform ();
			}
		comp->OnBeginPlay ();

		}
	if (RootComponent)
		{
		RootComponent->SetCollisionEnabled ( bIsCollisionEnabled );
		RootComponent->UpdateTransform ();
		}
	}

void CActor::Tick ( float deltaTime )
	{
	if (IsPendingToDestroy ()) return;

	UpdatePhysics ( deltaTime );

	// Сначала обновляем интерполяцию
	if (bIsLerpingLocation && RootComponent)
		{


		LocationLerpAlpha += deltaTime * LerpSpeed;

		if (LocationLerpAlpha >= 1.0f)
			{
				// Завершаем интерполяцию
			LocationLerpAlpha = 1.0f;
			bIsLerpingLocation = false;
			bIsMoving = false;
			RootComponent->SetLocation ( TargetLocation );

			}
		else
			{
				// Продолжаем интерполяцию
			FVector currentLocation = FVector::Lerp (
				LerpStartLocation,
				TargetLocation,
				LocationLerpAlpha
			);
			RootComponent->SetLocation ( currentLocation );

			}
		}

		// Затем обновляем вращение
	if (bIsLerpingRotation && RootComponent)
		{
		RotationLerpAlpha += deltaTime * LerpSpeed;

		if (RotationLerpAlpha >= 1.0f)
			{
			RotationLerpAlpha = 1.0f;
			bIsLerpingRotation = false;
			RootComponent->SetRotation ( TargetRotation );
			}
		else
			{
			FQuat currentRotation = FQuat::Slerp (
				LerpStartRotation,
				TargetRotation,
				RotationLerpAlpha
			);
			currentRotation.Normalize ();
			RootComponent->SetRotation ( currentRotation );
			}
		}

		// ПОТОМ обновляем компоненты
	for (auto comp : ActorComponents)
		{
		if (comp && comp->GetOwner () == this)
			{
			comp->Tick ( deltaTime );
			}
		}
	this->DebugInfo ( deltaTime );
	}


void CActor::EndPlay ()
	{
	LOG_DEBUG ( "[ACTOR] EndPlay: ", GetName () );
	}

void CActor::DebugInfo ( float deltaTime )
	{

	}

FRenderCollection CActor::GetRenderInfo () const
	{
	FRenderCollection Collection;

	// Сбор мешей и террейнов для рендера
	for (auto * comp : ActorComponents)
		{
		if (CBaseMeshComponent * mesh = dynamic_cast< CBaseMeshComponent * >( comp ))
			{
			if (mesh->IsReadyForRender ())
				{
				if (CTerrainMeshComponent * terrain = dynamic_cast< CTerrainMeshComponent * >( mesh ))
					{
					FTerrainRenderInfo terrainInfo = terrain->GetTerrainInfo ();
					if (terrainInfo.IsValid ())
						{
						Collection.Terrains.push_back ( terrainInfo );
						}
					}
				else
					{
					FMeshInfo meshInfo = mesh->GetMeshInfo ();
					if (meshInfo.IsValid ())
						{
						Collection.Meshes.push_back ( meshInfo );
						}
					}
				}
			}
		}

		// Сбор отладочной информации о коллизиях
	if (m_bDrawCollisions)
		{
			// Функция для обработки коллизионного компонента
		auto ProcessCollisionComponent = [ & ] ( const CBaseCollisionComponent * collision )
			{
			if (!collision || !collision->IsCollisionEnabled ())
				return;

			FVector worldLoc = collision->GetWorldLocation ();
			FQuat worldRot = GetActorRotationQuat ();

			// Выбираем цвет в зависимости от типа взаимодействия
			FVector debugColor = FVector ( 0.0f, 1.0f, 0.0f ); // Зеленый по умолчанию

			if (collision->ShouldBlockWith ( collision ))
				debugColor = FVector ( 1.0f, 0.0f, 0.0f ); // Красный для Block
			else if (collision->ShouldOverlapWith ( collision ))
				debugColor = FVector ( 0.0f, 0.0f, 1.0f ); // Синий для Overlap

			switch (collision->GetShapeType ())
				{
					case ECollisionShape::SPHERE:
						{
						if (auto * sphere = dynamic_cast< const CSphereComponent * >( collision ))
							{
							Collection.DebugCollisions.push_back (
								FCollisionDebugInfo::CreateSphere (
									worldLoc,
									sphere->GetRadius (),
									debugColor
								)
							);
							}
						break;
						}

					case ECollisionShape::BOX:
						{
						if (auto * box = dynamic_cast< const CBoxComponent * >( collision ))
							{
							Collection.DebugCollisions.push_back (
								FCollisionDebugInfo::CreateBox (
									worldLoc,
									worldRot,
									box->GetHalfExtents (),
									debugColor
								)
							);
							}
						break;
						}

					case ECollisionShape::CAPSULE:
						{
						if (auto * capsule = dynamic_cast< const CCapsuleComponent * >( collision ))
							{
							Collection.DebugCollisions.push_back (
								FCollisionDebugInfo::CreateCapsule (
									worldLoc,
									worldRot,
									capsule->GetRadius (),
									capsule->GetHalfHeight (),
									debugColor
								)
							);
							}
						break;
						}

					case ECollisionShape::CYLINDER:
						{
						if (auto * cylinder = dynamic_cast< const CCylinderComponent * >( collision ))
							{
							Collection.DebugCollisions.push_back (
								FCollisionDebugInfo::CreateCylinder (
									worldLoc,
									worldRot,
									cylinder->GetRadius (),
									cylinder->GetHeight (),
									debugColor
								)
							);
							}
						break;
						}

					case ECollisionShape::CONE:
						{
						if (auto * cone = dynamic_cast< const CConeComponent * >( collision ))
							{
							Collection.DebugCollisions.push_back (
								FCollisionDebugInfo::CreateCone (
									worldLoc,
									worldRot,
									cone->GetRadius (),
									cone->GetHeight (),
									debugColor
								)
							);
							}
						break;
						}

					case ECollisionShape::TERRAIN:
						{
						if (auto * terrain = dynamic_cast< const CTerrainComponent * >( collision ))
							{
								// Используем специальную структуру для террейна
							FTerrainDebugInfo debugInfo;

							// Генерируем wireframe для террейна
							CWireframeGenerator::GenerateTerrainWireframe (
								debugInfo.WireframeVertices,
								terrain,
								FVector ( 0.7f, 0.7f, 0.7f ),  // Серый цвет
								false  // Не рисовать диагонали
							);

							// Заполняем дополнительную информацию
							debugInfo.Width = terrain->GetTerrainData ().Width;
							debugInfo.Height = terrain->GetTerrainData ().Height;
							debugInfo.CellSize = terrain->GetTerrainData ().CellSize;

							// ВАЖНО: Добавляем в коллекцию!
							if (debugInfo.IsValid ())
								{
								Collection.TerrainWireframes.push_back ( debugInfo );
								}
							}
						break;
						}

					default:
						break;
				}
			};

			// Обрабатываем прямые коллизионные компоненты
		for (auto * comp : ActorComponents)
			{
			if (auto * collision = dynamic_cast< CBaseCollisionComponent * >( comp ))
				{
				ProcessCollisionComponent ( collision );
				}
			}

			// Обрабатываем коллизионные компоненты в дочерних трансформ-компонентах
		for (auto * comp : ActorComponents)
			{
			if (auto * transform = dynamic_cast< CTransformComponent * >( comp ))
				{
				for (auto * childComp : transform->GetChildTransformComponents ())
					{
					if (auto * collision = dynamic_cast< CBaseCollisionComponent * >( childComp ))
						{
						ProcessCollisionComponent ( collision );
						}
					}
				}
			}
		}

	return Collection;
	}

CLevel * CActor::GetLevel () const
	{
	if (GetWorld () == nullptr) return nullptr;
	return GetWorld ()->GetCurrentLevel ();
	}

CWorld * CActor::GetWorld () const
	{
	return CGameInstance::Get ().GetWorld ();
	}

std::vector<FMeshInfo> CActor::GetRenderMeshes () const
	{
	std::vector<FMeshInfo> RenderMeshes;
	std::vector<CBaseComponent *> Components = ActorComponents;
	for (auto & comp : Components)
		{
		if (CBaseMeshComponent * mesh = dynamic_cast< CBaseMeshComponent * >( comp ))
			{
			if (mesh->IsReadyForRender ())
				{
				auto info = mesh->GetMeshInfo ();
				RenderMeshes.push_back ( info );
				static float timer = 0.f;
				timer += 0.016f;
				if (timer >= 1.f)
					{
					LOG_WARN ( "[", GetName (), "] Added mesh: ", mesh->GetName (), " to render list" );
					timer = 0.f;
					}
				}
			}
		}

	Components.clear ();
	static float tttt = 0.f;
	tttt += 0.016f;
	if (tttt >= 1.f)
		{
		LOG_WARN ( "[", GetName (), "] collected ", RenderMeshes.size (), " meshes" );
		tttt = 0.f;
		}
	return RenderMeshes;
	}


void CActor::SetRootComponent ( CTransformComponent * NewRoot )
	{
	if (!NewRoot)
		{
		LOG_WARN ( "Cannot set nullptr as RootComponent" );
		return;
		}

	if (NewRoot->GetOwner () != this)
		{
		LOG_WARN ( "RootComponent must belong to this actor" );
		return;
		}

	if (RootComponent == NewRoot)
		{
		return;
		}

	CTransformComponent * OldRoot = RootComponent;

	if (OldRoot)
		{
			// Сохраняем мировую позицию СТАРОГО корня
		FVector WorldLocation = OldRoot->GetLocation ();  // (500, 500.332, 500)
		FQuat WorldRotation = OldRoot->GetRotationQuat ();
		FVector WorldScale = OldRoot->GetScale ();

		// Прикрепляем старый корень к новому
		OldRoot->AttachTo ( NewRoot );  // Теперь Transform приаттачен к Capsule

		// НОВЫЙ корень получает мировую позицию СТАРОГО корня
		NewRoot->SetTransform ( FTransform ( WorldLocation, WorldRotation, WorldScale ) );

		// Старый корень теперь в относительных координатах
		OldRoot->SetRelativeLocation ( FVector::Zero () );
		OldRoot->SetRelativeRotation ( FQuat::Identity () );
		OldRoot->SetRelativeScale ( FVector::One () );

		RootComponent = NewRoot;
		}
	}

void CActor::Destroy ()
	{
	if (bIsPendingToDestroy)
		{
		LOG_WARN ( "Actor: ", GetName (), " already marked to destroy" );
		return;
		}
	auto level = GetLevel ();
	if (level)
		{
		level->DestroyActor ( GetName () );
		}
	}

void CActor::SetPendingToDestroy ()
	{
	if (bIsPendingToDestroy)
		{
		LOG_WARN ( "Actor: ", GetName (), " already marked to destroy" );
		return;
		}
	LOG_DEBUG ( "Actor:", GetName (), " is marked to destroy" );
	bIsPendingToDestroy = true;
	}

CBaseComponent * CActor::AddDefaultSubObject ( const std::string & className, const std::string & desiredDisplayName )
	{
	auto CompToAdd = CObject::AddSubObjectByClass ( className, desiredDisplayName );
	if (CBaseComponent * compToReturn = dynamic_cast< CBaseComponent * >( CompToAdd ))
		{
		return compToReturn;
		}
	return nullptr;
	}

FVector CActor::GetActorLocation ()
	{
	FVector result {};

	if (GetRootComponent () != nullptr)
		{
		return  RootComponent->GetLocation ();
		}

	return result;
	}

FVector CActor::GetActorRotation ()
	{
	FVector RotVec = GetActorRotationQuat ().GetEulerAngles ();
	return FVector (
		CEMath::RadiansToDegrees ( RotVec.x ),
		CEMath::RadiansToDegrees ( RotVec.y ),
		CEMath::RadiansToDegrees ( RotVec.z )
	);
	}

FVector CActor::GetActorScale ()
	{
	FVector result {};
	if (GetRootComponent () != nullptr) result = RootComponent->GetScale ();
	return result;
	}

FQuat CActor::GetActorRotationQuat ()
	{
	FQuat result {};
	if (GetRootComponent () != nullptr) result = RootComponent->GetRotationQuat ();
	return result;
	}

	// ============================================================================
	// Direction vectors getters
	// ============================================================================

FVector CActor::GetActorForwardVector ()
	{
	if (!RootComponent) return FVector::Forward ();
	FQuat rotation = RootComponent->GetRotationQuat ();
	rotation.Normalize ();
	// Вперед по оси Z (0, 0, 1) в локальном пространстве
	return rotation * FVector::Forward ();
	}

FVector CActor::GetActorRightVector ()
	{
	if (!RootComponent) return FVector::Right ();
	FQuat rotation = RootComponent->GetRotationQuat ();
	rotation.Normalize ();
	// Вправо по оси X (1, 0, 0) в локальном пространстве
	return rotation * FVector::Right ();
	}

FVector CActor::GetActorUpVector ()
	{
	if (!RootComponent) return FVector::Up ();
	FQuat rotation = RootComponent->GetRotationQuat ();
	rotation.Normalize ();
	// Вверх по оси Y (0, 1, 0) в локальном пространстве
	return rotation * FVector::Up ();
	}

	// ============================================================================
	// Transform setters
	// ============================================================================

void CActor::SetActorLocation ( const FVector & InLocation, bool bTeleport )
	{
	if (!bTeleport)
		{
		if (RootComponent)
			{
			RootComponent->SetLocation ( InLocation );
			// Сбрасываем интерполяцию позиции
			bIsLerpingLocation = false;
			LocationLerpAlpha = 0.0f;
			RootComponent->MarkTransformDirty ();
			}
		}
	else
		{
		this->TeleportTo ( InLocation );
		}
	}

void CActor::SetActorLocation ( float inX, float inY, float inZ, bool bTeleport )
	{
	SetActorLocation ( FVector ( inX, inY, inZ ), bTeleport );
	}

void CActor::SetActorScale ( const FVector & InScale )
	{
	if (RootComponent)
		{
		RootComponent->SetScale ( InScale );
		}
	}

void CActor::SetActorScale ( float inX, float inY, float inZ )
	{
	SetActorScale ( FVector ( inX, inY, inZ ) );
	}

void CActor::SetActorScale ( float InScale )
	{
	SetActorScale ( InScale, InScale, InScale );
	}

void CActor::SetActorRotation ( const FVector & inRotation )
	{
	FQuat rotationQuat = FQuat::FromEulerAngles (
		CEMath::DegreesToRadians ( inRotation.x ),
		CEMath::DegreesToRadians ( inRotation.y ),
		CEMath::DegreesToRadians ( inRotation.z )
	);
	SetActorRotation ( rotationQuat );
	}

void CActor::SetActorRotation ( const FQuat & inRotation )
	{
	if (RootComponent)
		{
		RootComponent->SetRotation ( inRotation );
		// Сбрасываем интерполяцию вращения
		bIsLerpingRotation = false;
		RotationLerpAlpha = 0.0f;
		}
	}

void CActor::SetActorRotation ( float inX, float inY, float inZ )
	{
	SetActorRotation ( FVector ( inX, inY, inZ ) );
	}

	// ============================================================================
	// Teleport functions (immediate movement)
	// ============================================================================

void CActor::TeleportTo ( const FVector & NewLocation )
	{
	LOG_DEBUG ( "[ACTOR] TeleportTo: ", GetName (), " to (",
				NewLocation.x, ", ", NewLocation.y, ", ", NewLocation.z, ")" );
	if (RootComponent)
		{
		RootComponent->SetLocation ( NewLocation );
		RootComponent->UpdateTransform ();
		}
	}

void CActor::TeleportTo ( float NewX, float NewY, float NewZ )
	{
	TeleportTo ( FVector ( NewX, NewY, NewZ ) );
	}

void CActor::SetActorRotationImmediately ( const FQuat & NewRotation )
	{
	LOG_DEBUG ( "[ACTOR] SetActorRotationImmediately: ", GetName () );
	SetActorRotation ( NewRotation );
	}

void CActor::SetActorRotationImmediately ( const FVector & NewRotation )
	{
	LOG_DEBUG ( "[ACTOR] SetActorRotationImmediately: ", GetName (),
				" to (", NewRotation.x, ", ", NewRotation.y, ", ", NewRotation.z, ")" );
	SetActorRotation ( NewRotation );
	}

void CActor::SetActorRotationImmediately ( float inX, float inY, float inZ )
	{
	SetActorRotationImmediately ( FVector ( inX, inY, inZ ) );
	}

void CActor::DestroyGravity ()
	{
	auto it = std::find ( ActorComponents.begin (), ActorComponents.end (), m_Gravity );
	if (it != ActorComponents.end ())
		{
		ActorComponents.erase ( it );
		}
	}


void CActor::MoveActor ( const FVector & Delta, bool Interpolate )
	{
	if (!RootComponent)
		return;

	FVector currentLocation = RootComponent->GetLocation ();
	FVector newTarget = currentLocation + Delta;

	if (!Interpolate)
		{
		// Мгновенное перемещение
		RootComponent->SetLocation ( newTarget );
		bIsLerpingLocation = false;
		bIsMoving = false;
		}
	else
		{
		if (bIsLerpingLocation)
			{
			TargetLocation = newTarget;
			}
		else
			{
			TargetLocation = newTarget;
			LerpStartLocation = currentLocation;
			LocationLerpAlpha = 0.0f;
			bIsLerpingLocation = true;
			bIsMoving = true;
			}
		}

	// Важно: обновляем последнюю позицию для гравитации
	if (m_Gravity)
		{
		m_Gravity->UpdateLastPosition ( currentLocation );
		}
	}

void CActor::RotateActor ( const FVector & DeltaRotation, bool Interpolate )
	{
	if (!RootComponent)
		return;

	FQuat deltaQuat = FQuat::FromEulerAngles (
		CEMath::DegreesToRadians ( DeltaRotation.x ),
		CEMath::DegreesToRadians ( DeltaRotation.y ),
		CEMath::DegreesToRadians ( DeltaRotation.z )
	);

	RotateActor ( deltaQuat, Interpolate );
	}

void CActor::RotateActor ( const FQuat & DeltaRotation, bool Interpolate )
	{
	if (!RootComponent)
		return;




	FQuat currentQuat = RootComponent->GetRotationQuat ();


	FQuat targetQuat = currentQuat * DeltaRotation;
	targetQuat.Normalize ();

	if (!Interpolate)
		{

		RootComponent->SetRotation ( targetQuat );

		return;
		}

	TargetRotation = targetQuat;
	LerpStartRotation = currentQuat;
	RotationLerpAlpha = 0.0f;
	bIsLerpingRotation = true;


	}



void CActor::AddActorWorldOffset ( const FVector & DeltaLocation, bool Interpolate )
	{

	MoveActor ( DeltaLocation, Interpolate );
	}

void CActor::AddActorLocalOffset ( const FVector & DeltaLocation, bool Interpolate )
	{
	if (!RootComponent)
		return;

	FQuat rotation = RootComponent->GetRotationQuat ();
	rotation.Normalize ();

	FVector worldDelta = rotation * DeltaLocation;


	AddActorWorldOffset ( worldDelta, Interpolate );
	}

void CActor::AddActorWorldRotation ( const FQuat & DeltaRotation, bool Interpolate )
	{
	if (!RootComponent)
		return;


	FQuat currentQuat = RootComponent->GetRotationQuat ();


	FQuat newRotation = currentQuat * DeltaRotation;
	newRotation.Normalize ();

	if (!Interpolate)
		{
		RootComponent->SetRotation ( newRotation );

		return;
		}

		// Интерполированное вращение
	TargetRotation = newRotation;
	LerpStartRotation = currentQuat;
	RotationLerpAlpha = 0.0f;
	bIsLerpingRotation = true;

	}

void CActor::AddActorLocalRotation ( const FQuat & DeltaRotation, bool Interpolate )
	{
	if (!RootComponent)
		return;

	// Получаем текущее вращение
	FQuat currentQuat = RootComponent->GetRotationQuat ();

	// ЛОКАЛЬНОЕ вращение: умножаем СЛЕВА (q' = Δq * q)
	FQuat newRotation = DeltaRotation * currentQuat;
	newRotation.Normalize ();

	if (!Interpolate)
		{
		RootComponent->SetRotation ( newRotation );

		return;
		}


	TargetRotation = newRotation;
	LerpStartRotation = currentQuat;
	RotationLerpAlpha = 0.0f;
	bIsLerpingRotation = true;


	}

	// ============================================================================
	// Helper movement methods
	// ============================================================================

void CActor::MoveActorInDirection ( const FVector & Direction, float Distance, bool Interpolate )
	{
	if (!RootComponent || Direction.IsZero ())
		return;



	  // Нормализуем направление и умножаем на расстояние
	FVector normalizedDir = Direction.Normalized ();
	FVector delta = normalizedDir * Distance;

	MoveActor ( delta, Interpolate );
	}

void CActor::RotateAroundAxis ( const FVector & Axis, float AngleDegrees, bool Interpolate )
	{
	if (!RootComponent || Axis.IsZero ())
		return;

	FQuat rotationQuat ( Axis.Normalized (), CEMath::DegreesToRadians ( AngleDegrees ) );

	AddActorLocalRotation ( rotationQuat, Interpolate );
	}

	// ============================================================================
	// Collision methods
	// ============================================================================

void CActor::SetCollisionEnabled ( bool value )
	{
	bIsCollisionEnabled = value;
	if (RootComponent)
		{
		RootComponent->SetCollisionEnabled ( bIsCollisionEnabled );
		}
	}


FVector CActor::GetActorLocation () const
	{
	FVector result {};
	if (GetRootComponent () != nullptr)
		{
		result = RootComponent->GetLocation ();
		}
	return result;
	}

FVector CActor::GetActorRotation () const
	{
	FVector RotVec = GetActorRotationQuat ().GetEulerAngles ();
	return FVector (
		CEMath::RadiansToDegrees ( RotVec.x ),
		CEMath::RadiansToDegrees ( RotVec.y ),
		CEMath::RadiansToDegrees ( RotVec.z )
	);
	}

FVector CActor::GetActorScale () const
	{
	FVector result {};
	if (GetRootComponent () != nullptr)
		{
		result = RootComponent->GetScale ();
		}
	return result;
	}

FQuat CActor::GetActorRotationQuat () const
	{
	FQuat result {};
	if (GetRootComponent () != nullptr)
		{

		result = RootComponent->GetRotationQuat ();
		}
	return result;
	}

FVector CActor::GetActorForwardVector () const
	{
	if (!RootComponent) return FVector::Forward ();
	FQuat rotation = GetActorRotationQuat ();
	rotation.Normalize ();
	return rotation * FVector::Forward ();
	}

FVector CActor::GetActorRightVector () const
	{
	if (!RootComponent) return FVector::Right ();
	FQuat rotation = GetActorRotationQuat ();
	rotation.Normalize ();
	return rotation * FVector::Right ();
	}

FVector CActor::GetActorUpVector () const
	{
	if (!RootComponent) return FVector::Up ();
	FQuat rotation = GetActorRotationQuat ();
	rotation.Normalize ();
	return rotation * FVector::Up ();
	}


void CActor::OnComponentBeginOverlap ( CBaseCollisionComponent * other )
	{
	if (other == nullptr) return;

	}

void CActor::OnComponentEndOverlap ( CBaseCollisionComponent * other )
	{
	if (other == nullptr) return;
	LOG_ERROR ( "OnComponentEndOverlap with : ", other->GetOwnerActor ()->GetName (), " for ", GetName () );

	}

void CActor::SetActorName ( const std::string & newName )
	{
	this->Rename ( newName );
	}

void CActor::OnComponentHit ( CBaseCollisionComponent * other )
	{
	if (!other || !m_Gravity) return;

	// Проверяем, нужно ли блокировать движение
	CBaseCollisionComponent * myCollision = FindComponent<CBaseCollisionComponent> ();
	if (myCollision && ( myCollision->ShouldBlockWith ( other ) || other->ShouldBlockWith ( myCollision ) ))
		{
			// Останавливаем вертикальную скорость
		m_Gravity->SetVerticalVelocity ( 0.0f );

		LOG_DEBUG ( "[ACTOR] ", GetName (), " stopped by ",
					other->GetOwnerActor () ? other->GetOwnerActor ()->GetName () : "unknown" );
		}
	}

void CActor::SetMovableState ( const EMovableState & state )
	{
	MovableState = state;
	switch (MovableState)
		{
			case EMovableState::STATIC:
				{
				m_Gravity->SetEnableGravity(false); 
				Velocity = FVector::Zero ();
				SetCollisionEnabled ( true );
				break;
				}
			case EMovableState::MOVABLE:
				{
				m_Gravity->SetEnableGravity(true);
				SetCollisionEnabled ( true );
				break;
				}
			case EMovableState::DYNAMIC:
				{				
				m_Gravity->SetEnableGravity(true);
				SetCollisionEnabled ( true );
				break;
				}
			default:
				break;
		}
	}

void CActor::AddImpulse ( const FVector & Impulse )
	{}

void CActor::SetVelocity ( const FVector & NewVelocity )
	{
	Velocity = NewVelocity;
	}

void CActor::UpdatePhysics ( float DeltaTime )
	{
	if (MovableState == EMovableState::STATIC) return;
	if (m_Gravity && m_Gravity->IsGravityEnabled ())
		{
		m_Gravity->ApplyGravity ( DeltaTime );
		Velocity.y += m_Gravity->GetVerticalVelocity () * DeltaTime;
		}
	if (!Velocity.IsZero ())
		{
		MoveActor ( Velocity * DeltaTime );
		}
	Velocity.y *= 0.98f;
	}
