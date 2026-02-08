#include "Components/TransformComponent.h"
#include "Actors/Actor.h"

CTransformComponent::CTransformComponent ( CObject * inOwner, const std::string & inDisplayName ) : Super ( inOwner, inDisplayName ), bIsTransformDirty { false }
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
	}

void CTransformComponent::UpdateTransform ()
	{
	if (!bIsTransformDirty)
		return;

	if (ParentTransform)  // Есть родитель
		{
		FTransform parentTransform = ParentTransform->GetTransform ();

		// Вычисляем мировую трансформацию из относительной и родительской
		m_WorldTransform.Scale = parentTransform.Scale * m_RelativeTransform.Scale;
		m_WorldTransform.Rotation = parentTransform.Rotation * m_RelativeTransform.Rotation;

		// Позиция: позиция родителя + (вращение родителя * (масштаб родителя * относительная позиция))
		FVector scaledRelativeLocation = m_RelativeTransform.Location * parentTransform.Scale;
		FVector rotatedRelativeLocation = parentTransform.Rotation * scaledRelativeLocation;
		m_WorldTransform.Location = parentTransform.Location + rotatedRelativeLocation;
		}
	else  // Нет родителя
		{
		// Нет родителя - мировая трансформация равна относительной
		m_WorldTransform = m_RelativeTransform;
		}

	// Обновляем кэш
	CachedWordTransform = m_WorldTransform;
	CachedRelativeTransform = m_RelativeTransform;

	// Обновляем детей
	for (auto child : ChildTransformComponents)
		{
		if (child)
			{
			child->MarkTransformDirty ();
			}
		}
	UpdateTransformMatix ();
	bIsTransformDirty = false;
	}

void CTransformComponent::SetTransform ( const FTransform & InTransform )
	{
	m_WorldTransform = InTransform;
	MarkTransformDirty ();
	}

void CTransformComponent::SetRelativeTransform ( const FTransform & InTransform )
	{
	m_RelativeTransform = InTransform;
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

FTransform  CTransformComponent::GetTransform () const
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
			m_RelativeTransform.Scale = inScale / parentScale;
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

	//m_WorldTransform.Scale = inScale;
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
	if (ParentTransform)
		{
		// Конвертируем мировое вращение в относительное
		FQuat parentRotation = ParentTransform->GetRotationQuat ();
		FQuat parentInverse = parentRotation.Conjugated ();
		m_RelativeTransform.Rotation = parentInverse * inRotation;
		}
	else
		{
		m_RelativeTransform.Rotation = inRotation;
		}

	//m_WorldTransform.Rotation = inRotation;
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
		delta = inverseParentRot * delta;

		// Убираем влияние масштаба родителя
		if (!parentTransform.Scale.IsZero ())
			{
			m_RelativeTransform.Location = delta / parentTransform.Scale;
			}
		else
			{
			m_RelativeTransform.Location = delta;
			}
		}
	else
		{
		// Нет родителя - относительная равна мировой
		m_RelativeTransform.Location = inLocation;
		}

	//m_WorldTransform.Location = inLocation;
	MarkTransformDirty ();
	}

void CTransformComponent::SetLocation ( float inX, float inY, float inZ )
	{
	SetLocation ( FVector ( inX, inY, inZ ) );
	}

FTransform  CTransformComponent::GetParentTransform ()
	{
	if (GetParent () != nullptr)
		{
		return GetParent ()->GetTransform ();
		}
	return FTransform::Identity ();
	}

void CTransformComponent::UpdateAllTransforms ()
	{
	}

void CTransformComponent::UpdateTransformMatix ()
	{
	
	}
