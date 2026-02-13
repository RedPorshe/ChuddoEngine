#include "Components/TransformComponent.h"
#include "Actors/Actor.h"
#include "Components/Collisions/BaseCollisionComponent.h"

CTransformComponent::CTransformComponent ( CObject * inOwner, const std::string & inDisplayName )
	: Super ( inOwner, inDisplayName ), bIsTransformDirty ( false )
	{

	}

CTransformComponent::~CTransformComponent ()
	{
	DetachFromParent ();

	for (auto child : ChildTransformComponents)
		{
		if (child && child->ParentTransform == this)
			{
			child->ParentTransform = nullptr;
			}
		}
	ChildTransformComponents.clear ();
	}

void CTransformComponent::InitComponent ()
	{
	Super::InitComponent ();
	}

void CTransformComponent::Tick ( float DeltaTime )
	{
	Super::Tick ( DeltaTime );
	if (bIsTransformDirty)
		{
		UpdateTransform ();
		}
	}

void CTransformComponent::OnBeginPlay ()
	{
	Super::OnBeginPlay ();
	if (bIsAutoGenerateCollision)
		{
		SetCollisionComponent ( AddSubObject<CBaseCollisionComponent> ( GetName () + "_collision" ) );
		}
	}

void CTransformComponent::UpdateTransform ()
	{
	if (!bIsTransformDirty)
		return;

	if (ParentTransform) // Есть родитель
		{
		FTransform parentTransform = ParentTransform->GetTransform ();

		// ПРАВИЛЬНОЕ вычисление мировой трансформации:
		// 1. Сначала вращение, потом позиция (стандартный порядок в игровых движках)
		m_WorldTransform.Rotation = parentTransform.Rotation * m_RelativeTransform.Rotation;
		m_WorldTransform.Rotation.Normalize (); // Важно нормализовать

		// 2. Позиция: сначала применяем вращение родителя к локальной позиции,
		//    потом добавляем позицию родителя
		FVector rotatedLocalPos = parentTransform.Rotation * m_RelativeTransform.Location;
		m_WorldTransform.Location = parentTransform.Location + rotatedLocalPos;

		// 3. Масштаб: перемножаем
		m_WorldTransform.Scale = parentTransform.Scale * m_RelativeTransform.Scale;
		}
	else // Нет родителя
		{
			// Нет родителя - мировая трансформация равна относительной
		m_WorldTransform = m_RelativeTransform;
		m_WorldTransform.Rotation.Normalize (); // Всегда нормализуем
		}

		// Обновляем кэш
	CachedWorldTransform = m_WorldTransform;
	CachedRelativeTransform = m_RelativeTransform;

	// Обновляем матрицу трансформации для рендеринга
	UpdateTransformMatrix ();

	// Обновляем детей
	for (auto child : ChildTransformComponents)
		{
		if (child)
			{
			child->MarkTransformDirty ();
			}
		}

	bIsTransformDirty = false;
	}

void CTransformComponent::UpdateTransformMatrix ()
	{
		// Получаем компоненты трансформации
	const FVector & location = m_WorldTransform.Location;
	const FQuat & rotation = m_WorldTransform.Rotation;
	const FVector & scale = m_WorldTransform.Scale;

	// Создаем матрицы для каждой компоненты
	FMat4 translationMatrix = FMat4::Translation ( location.x, location.y, location.z );

	// Конвертируем кватернион в матрицу вращения
	FMat4 rotationMatrix = rotation.ToMatrix ();

	// Создаем матрицу масштабирования
	FMat4 scaleMatrix = FMat4::Scaling ( scale.x, scale.y, scale.z );

	// Комбинируем матрицы в правильном порядке: Scale * Rotation * Translation
	// Для большинства графических API порядок: сначала масштаб, потом вращение, потом перемещение
	// Вектор умножается на матрицу справа: v' = M * v
	m_TransformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

	// Опционально: можно сохранить отдельно матрицу для нормалей (без трансляции)
	// m_NormalMatrix = (rotationMatrix * scaleMatrix).Inversed().Transposed();
	}

void CTransformComponent::SetTransform ( const FTransform & InTransform )
	{
	m_WorldTransform = InTransform;
	m_WorldTransform.Rotation.Normalize ();
	MarkTransformDirty ();
	}

void CTransformComponent::SetRelativeTransform ( const FTransform & InTransform )
	{
	m_RelativeTransform = InTransform;
	m_RelativeTransform.Rotation.Normalize ();
	MarkTransformDirty ();
	}

void CTransformComponent::MarkTransformDirty ()
	{
	bIsTransformDirty = true;
	for (auto child : ChildTransformComponents)
		{
		if (child)
			{
			child->MarkTransformDirty ();
			}
		}
	}

void CTransformComponent::SetAutoGenerateCollisionComponent ( bool value, const FCollisionChannel & inChannel )
	{
	bIsAutoGenerateCollision = value;
	CollisionChannel = inChannel;
	}

void CTransformComponent::AddChild ( CTransformComponent * Child )
	{
	if (!Child || Child == this)
		{
		LOG_ERROR ( this->GetName (), " can't add invalid or self as child" );
		return;
		}

	if (Child == GetParent () || ( GetParent () && Child == GetParent ()->GetParent () ))
		{
		LOG_ERROR ( this->GetName (), " can't be parent for his parent" );
		return;
		}

	if (Child->ParentTransform && Child->ParentTransform != this)
		{
		Child->ParentTransform->RemoveChild ( Child );
		}

	ChildTransformComponents.push_back ( Child );
	Child->ParentTransform = this;
	Child->MarkTransformDirty ();

	LOG_DEBUG ( this->GetName (), " added child: ", Child->GetName () );
	}

void CTransformComponent::RemoveChild ( CTransformComponent * Child )
	{
	if (!Child)
		{
		LOG_ERROR ( "Can't remove null child" );
		return;
		}

	auto it = std::find ( ChildTransformComponents.begin (),
						  ChildTransformComponents.end (), Child );

	if (it != ChildTransformComponents.end ())
		{
		ChildTransformComponents.erase ( it );

		if (Child->ParentTransform == this)
			{
			Child->ParentTransform = nullptr;
			}

		LOG_DEBUG ( GetName (), " removed child: ", Child->GetName () );
		}
	else
		{
		LOG_ERROR ( "Child not found: ", Child->GetName () );
		}
	}

void CTransformComponent::AttachTo ( CTransformComponent * Parent )
	{
	if (GetParent () == Parent)
		return;

	if (!Parent)
		{
		LOG_ERROR ( "Can't attach to null parent" );
		return;
		}

	Parent->AddChild ( this );
	}

bool CTransformComponent::IsChildTransformComponent () const
	{
	return GetParent () != nullptr;
	}

void CTransformComponent::DetachFromParent ()
	{
	if (!ParentTransform)
		return;

	ParentTransform->RemoveChild ( this );
	}

FTransform CTransformComponent::GetTransform () const
	{
	return m_WorldTransform;
	}

FTransform CTransformComponent::GetRelativeTransform () const
	{
	return m_RelativeTransform;
	}

FVector CTransformComponent::GetLocation () const
	{
	return m_WorldTransform.Location;
	}

FVector CTransformComponent::GetRelativeLocation () const
	{
	return m_RelativeTransform.Location;
	}

FVector CTransformComponent::GetScale () const
	{
	return m_WorldTransform.Scale;
	}

FVector CTransformComponent::GetRelativeScale () const
	{
	return m_RelativeTransform.Scale;
	}

FQuat CTransformComponent::GetRotationQuat () const
	{
	return m_WorldTransform.Rotation;
	}

FQuat CTransformComponent::GetRelativeRotationQuat () const
	{
	return m_RelativeTransform.Rotation;
	}

FVector CTransformComponent::GetRotation () const
	{
	FVector RotVec = GetRotationQuat ().GetEulerAngles ();
	return FVector (
		CEMath::RadiansToDegrees ( RotVec.x ),
		CEMath::RadiansToDegrees ( RotVec.y ),
		CEMath::RadiansToDegrees ( RotVec.z )
	);
	}

FVector CTransformComponent::GetRelativeRotation () const
	{
	FVector RotVec = GetRelativeRotationQuat ().GetEulerAngles ();
	return FVector (
		CEMath::RadiansToDegrees ( RotVec.x ),
		CEMath::RadiansToDegrees ( RotVec.y ),
		CEMath::RadiansToDegrees ( RotVec.z )
	);
	}

CBaseCollisionComponent * CTransformComponent::GetCollisionComponent ()
	{
	return CollisionComp;
	}

void CTransformComponent::SetCollisionComponent ( CBaseCollisionComponent * inComp )
	{
	if (inComp == nullptr)
		{
		LOG_ERROR ( GetName (),
					": Transform component must have collision component, can't change to nullptr" );
		return;
		}

	if (inComp == CollisionComp)
		{
		LOG_WARN ( GetName (),
				   ": Transform component already has '", inComp->GetName (),
				   "' as collision component" );
		return;
		}

	if (CollisionComp)
		{
		const std::string & oldName = CollisionComp->GetName ();

		LOG_DEBUG ( GetName (),
					": Replacing collision component '", oldName,
					"' with '", inComp->GetName (), "'" );

		if (CollisionComp->HasOwnedObjects ())
			{
			const auto & ownedObjects = CollisionComp->GetOwnedObjects ();

			for (auto it = ownedObjects.rbegin (); it != ownedObjects.rend (); ++it)
				{
				if (*it)
					{
					if (!CollisionComp->TransferOwnership ( it->get (), inComp ))
						{
						LOG_WARN ( GetName (),
								   ": Failed to transfer ownership of '",
								   ( *it )->GetName (), "'" );
						}
					}
				}
			}

		if (!RemoveOwnedObject ( oldName ))
			{
			LOG_ERROR ( GetName (),
						": Critical error - failed to remove collision component '",
						oldName, "'" );

#ifdef _DEBUG
			assert ( false && "Failed to remove collision component" );
#endif
			}

		CollisionComp = inComp;
		return;
		}

	LOG_DEBUG ( GetName (),
				": Setting collision component to '", inComp->GetName (), "'" );
	CollisionComp = inComp;
	}

void CTransformComponent::SetCollisionComponent ( const std::string & inCompClassName )
	{
	auto newColl = AddSubObjectByClass ( inCompClassName, GetName () + "_collision" );
	if (newColl)
		{
		if (CBaseCollisionComponent * newColll = dynamic_cast< CBaseCollisionComponent * >( newColl ))
			{
			SetCollisionComponent ( newColll );
			}
		}
	}

void CTransformComponent::SetCollisionEnabled ( bool value )
	{
	bIsCollisionEnabled = value;
	if (CollisionComp)
		{
		CollisionComp->SetCollisionEnabled ( bIsCollisionEnabled );
		}
	}

bool CTransformComponent::IsChildOf ( CTransformComponent * PotentialParent ) const
	{
	if (!PotentialParent)
		return false;

	if (ParentTransform == PotentialParent)
		return true;

	CTransformComponent * current = ParentTransform;
	while (current)
		{
		if (current == PotentialParent)
			return true;
		current = current->ParentTransform;
		}

	return false;
	}

CTransformComponent * CTransformComponent::GetRootTransformComponent () const
	{
	CTransformComponent * root = const_cast< CTransformComponent * >( this );

	while (root && root->ParentTransform)
		{
		root = root->ParentTransform;
		}

	return root;
	}

void CTransformComponent::SetRelativeLocation ( const FVector & inLocation )
	{
	m_RelativeTransform.Location = inLocation;
	MarkTransformDirty ();
	}

void CTransformComponent::SetRelativeLocation ( float inX, float inY, float inZ )
	{
	SetRelativeLocation ( FVector ( inX, inY, inZ ) );
	}

void CTransformComponent::SetScale ( const FVector & inScale )
	{
	if (ParentTransform)
		{
			// Конвертируем мировой масштаб в относительный
		FVector parentScale = ParentTransform->GetScale ();
		if (!parentScale.IsZero ())
			{
				// Безопасное деление с проверкой
			m_RelativeTransform.Scale = FVector (
				parentScale.x != 0.0f ? inScale.x / parentScale.x : inScale.x,
				parentScale.y != 0.0f ? inScale.y / parentScale.y : inScale.y,
				parentScale.z != 0.0f ? inScale.z / parentScale.z : inScale.z
			);
			}
		else
			{
			m_RelativeTransform.Scale = inScale;
			}
		}
	else
		{
		m_RelativeTransform.Scale = inScale;
		}

	MarkTransformDirty ();
	}

void CTransformComponent::SetScale ( float inX, float inY, float inZ )
	{
	SetScale ( FVector ( inX, inY, inZ ) );
	}

void CTransformComponent::SetScale ( float scale )
	{
	SetScale ( scale, scale, scale );
	}

void CTransformComponent::SetRelativeScale ( const FVector & inScale )
	{
	m_RelativeTransform.Scale = inScale;
	MarkTransformDirty ();
	}

void CTransformComponent::SetRelativeScale ( float inX, float inY, float inZ )
	{
	SetRelativeScale ( FVector ( inX, inY, inZ ) );
	}

void CTransformComponent::SetRelativeScale ( float scale )
	{
	SetRelativeScale ( scale, scale, scale );
	}

void CTransformComponent::SetRotation ( const FQuat & inRotation )
	{
	FQuat normalizedRotation = inRotation;
	normalizedRotation.Normalize ();

	if (ParentTransform)
		{
			// Конвертируем мировое вращение в относительное
		FQuat parentRotation = ParentTransform->GetRotationQuat ();
		parentRotation.Normalize ();
		FQuat parentInverse = parentRotation.Conjugated ();
		m_RelativeTransform.Rotation = parentInverse * normalizedRotation;
		m_RelativeTransform.Rotation.Normalize ();
		}
	else
		{
		m_RelativeTransform.Rotation = normalizedRotation;
		}

	MarkTransformDirty ();
	}

void CTransformComponent::SetRotation ( const FVector & inRotation )
	{
	FQuat rotationQuat = FQuat::FromEulerAngles (
		CEMath::DegreesToRadians ( inRotation.x ),
		CEMath::DegreesToRadians ( inRotation.y ),
		CEMath::DegreesToRadians ( inRotation.z )
	);
	SetRotation ( rotationQuat );
	}

void CTransformComponent::SetRotation ( float inX, float inY, float inZ )
	{
	SetRotation ( FVector ( inX, inY, inZ ) );
	}

void CTransformComponent::SetRelativeRotation ( const FQuat & inRotation )
	{
	m_RelativeTransform.Rotation = inRotation;
	m_RelativeTransform.Rotation.Normalize ();
	MarkTransformDirty ();
	}

void CTransformComponent::SetRelativeRotation ( const FVector & inRotation )
	{
	FQuat rotationQuat = FQuat::FromEulerAngles (
		CEMath::DegreesToRadians ( inRotation.x ),
		CEMath::DegreesToRadians ( inRotation.y ),
		CEMath::DegreesToRadians ( inRotation.z )
	);
	SetRelativeRotation ( rotationQuat );
	}

void CTransformComponent::SetRelativeRotation ( float inX, float inY, float inZ )
	{
	SetRelativeRotation ( FVector ( inX, inY, inZ ) );
	}

void CTransformComponent::SetLocation ( const FVector & inLocation )
	{
	if (ParentTransform)
		{
			// Конвертируем мировую позицию в относительную
		FTransform parentTransform = ParentTransform->GetTransform ();
		FVector delta = inLocation - parentTransform.Location;

		// Убираем влияние вращения родителя
		FQuat inverseParentRot = parentTransform.Rotation.Conjugated ();
		inverseParentRot.Normalize ();
		delta = inverseParentRot * delta;

		// Убираем влияние масштаба родителя
		FVector parentScale = parentTransform.Scale;
		m_RelativeTransform.Location = FVector (
			parentScale.x != 0.0f ? delta.x / parentScale.x : delta.x,
			parentScale.y != 0.0f ? delta.y / parentScale.y : delta.y,
			parentScale.z != 0.0f ? delta.z / parentScale.z : delta.z
		);
		}
	else
		{
			// Нет родителя - относительная равна мировой
		m_RelativeTransform.Location = inLocation;
		}

	MarkTransformDirty ();
	}

void CTransformComponent::SetLocation ( float inX, float inY, float inZ )
	{
	SetLocation ( FVector ( inX, inY, inZ ) );
	}

FTransform CTransformComponent::GetParentTransform ()
	{
	if (GetParent () != nullptr)
		{
		return GetParent ()->GetTransform ();
		}
	return FTransform::Identity ();
	}

	// ============================================================================
	// AddLocalRotation - добавляет локальное вращение (относительно текущего)
	// ============================================================================
void CTransformComponent::AddLocalRotation ( const FQuat & DeltaRotation )
	{
		// Получаем текущее относительное вращение
	FQuat currentRotation = m_RelativeTransform.Rotation;
	currentRotation.Normalize ();

	// Нормализуем дельту
	FQuat delta = DeltaRotation;
	delta.Normalize ();

	// ПРАВИЛЬНЫЙ ПОРЯДОК: новое = дельта * текущее (локальное вращение)
	// Это добавляет вращение в локальном пространстве компонента
	FQuat newRotation = delta * currentRotation;
	newRotation.Normalize ();

	// Устанавливаем новое вращение
	SetRelativeRotation ( newRotation );

	LOG_DEBUG ( "[CTransformComponent] AddLocalRotation (quat): ", GetName () );
	}

void CTransformComponent::AddLocalRotation ( const FVector & DeltaRotationDegrees )
	{
		// Конвертируем градусы в радианы и создаем кватернион
	FQuat deltaQuat = FQuat::FromEulerAngles (
		CEMath::DegreesToRadians ( DeltaRotationDegrees.x ),
		CEMath::DegreesToRadians ( DeltaRotationDegrees.y ),
		CEMath::DegreesToRadians ( DeltaRotationDegrees.z )
	);

	AddLocalRotation ( deltaQuat );

	LOG_DEBUG ( "[CTransformComponent] AddLocalRotation (degrees): ", GetName (),
				" Pitch=", DeltaRotationDegrees.x,
				" Yaw=", DeltaRotationDegrees.y,
				" Roll=", DeltaRotationDegrees.z );
	}

void CTransformComponent::AddLocalRotation ( float PitchDegrees, float YawDegrees, float RollDegrees )
	{
	AddLocalRotation ( FVector ( PitchDegrees, YawDegrees, RollDegrees ) );
	}

	// ============================================================================
	// AddWorldRotation - добавляет мировое вращение (относительно мировых осей)
	// ============================================================================
void CTransformComponent::AddWorldRotation ( const FQuat & DeltaRotation )
	{
	if (ParentTransform)
		{
			// Если есть родитель, нужно преобразовать мировое вращение в локальное

			// Получаем вращение родителя
		FQuat parentRotation = ParentTransform->GetRotationQuat ();
		parentRotation.Normalize ();

		// Конвертируем мировую дельту в локальное пространство родителя
		FQuat parentInverse = parentRotation.Conjugated ();
		FQuat localDelta = parentInverse * DeltaRotation * parentRotation;
		localDelta.Normalize ();

		// Добавляем локальное вращение
		AddLocalRotation ( localDelta );
		}
	else
		{
			// Нет родителя - мировое = локальное
		AddLocalRotation ( DeltaRotation );
		}

	LOG_DEBUG ( "[CTransformComponent] AddWorldRotation (quat): ", GetName () );
	}

void CTransformComponent::AddWorldRotation ( const FVector & DeltaRotationDegrees )
	{
	FQuat deltaQuat = FQuat::FromEulerAngles (
		CEMath::DegreesToRadians ( DeltaRotationDegrees.x ),
		CEMath::DegreesToRadians ( DeltaRotationDegrees.y ),
		CEMath::DegreesToRadians ( DeltaRotationDegrees.z )
	);

	AddWorldRotation ( deltaQuat );
	}

void CTransformComponent::AddWorldRotation ( float PitchDegrees, float YawDegrees, float RollDegrees )
	{
	AddWorldRotation ( FVector ( PitchDegrees, YawDegrees, RollDegrees ) );
	}