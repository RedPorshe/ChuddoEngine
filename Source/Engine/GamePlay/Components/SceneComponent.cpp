#include "Engine/GamePlay/Components/SceneComponent.h"
#include "Engine/Core/CoreTypes.h"
#include "Engine/Utils/Math/MathConstants.hpp"


CSceneComponent::CSceneComponent(CObject* Owner, FString NewName)
    : CComponent(Owner, NewName)
{
  m_TransformMatrix= m_TransformMatrix.Identity ;
}

void CSceneComponent::SetPosition(const FVector& Position)
{
  m_RelativeLocation = Position;
  UpdateTransformMatrix();
}

void CSceneComponent::SetPosition(float X, float Y, float Z)
{
  SetPosition(FVector(X, Y, Z));
}

void CSceneComponent::SetRelativePosition(const FVector& Position)
{
  m_RelativeLocation = Position;
  UpdateTransformMatrix();
}

void CSceneComponent::SetRelativePosition(float X, float Y, float Z)
{
  SetRelativePosition(FVector(X, Y, Z));
}

void CSceneComponent::SetRelativePosition(float val)
{
  SetRelativePosition(val, val, val);
}
void CSceneComponent::SetRotation(const FVector& Rotation)
{
  m_WorldRotation = Rotation;
  m_RotationQuat = FQuat::FromEuler(
      CEMath::DEG_TO_RAD * Rotation.x,
      CEMath::DEG_TO_RAD *Rotation.y,
      CEMath::DEG_TO_RAD * Rotation.z
  );
  UpdateTransformMatrix();
}

void CSceneComponent::SetRotation(float Pitch, float Yaw, float Roll)
{
  SetRotation(FVector(Pitch, Yaw, Roll));
}

void CSceneComponent::SetRelativeRotation(const FVector& Rotation)
{
  m_RelativeRotation = Rotation;
  m_RotationQuat = FQuat::FromEuler(
      CEMath::DEG_TO_RAD * Rotation.x,
      CEMath::DEG_TO_RAD *Rotation.y,
      CEMath::DEG_TO_RAD * Rotation.z
  );
  UpdateTransformMatrix();
}

void CSceneComponent::SetRelativeRotation(float Pitch, float Yaw, float Roll)
{
  SetRelativeRotation(FVector(Pitch, Yaw, Roll));
}

void CSceneComponent::SetRotation(const FQuat& Rotation)
{
  m_RotationQuat = Rotation;
  UpdateRotationFromQuat();
  UpdateTransformMatrix();
}

void CSceneComponent::SetRelativeRotation(const FQuat& Rotation)
{
  m_RotationQuat = Rotation;
  UpdateRotationFromQuat();
  UpdateTransformMatrix();
}

void CSceneComponent::SetScale(const FVector& Scale)
{
  m_WorldScale = Scale;
  UpdateTransformMatrix();
}

void CSceneComponent::SetScale(float X, float Y, float Z)
{
  SetScale(FVector(X, Y, Z));
}

void CSceneComponent::SetScale(float value)
{
  SetScale(FVector(value, value, value));
}

void CSceneComponent::SetRelativeScale(const FVector& Scale)
{
  m_RelativeScale = Scale;
  UpdateTransformMatrix();
}

void CSceneComponent::SetRelativeScale(float X, float Y, float Z)
{
  SetRelativeScale(FVector(X, Y, Z));
}

void CSceneComponent::SetRelativeScale(float value)
{
  SetRelativeScale(FVector(value, value, value));
}

const FVector& CSceneComponent::GetPosition() const
{
  return m_WorldLocation;
}

const FVector& CSceneComponent::GetRelativePosition() const
{
  return m_RelativeLocation;
}

const FVector& CSceneComponent::GetRotation() const
{
  return m_WorldRotation;
}

const FVector& CSceneComponent::GetRelativeRotation() const
{
  return m_RelativeRotation;
}

const FVector& CSceneComponent::GetScale() const
{
  return m_WorldScale;
}

const FVector& CSceneComponent::GetRelativeScale() const
{
  return m_RelativeScale;
}

const FMatrix& CSceneComponent::GetTransformMatrix() const
{
  return m_TransformMatrix;
}

FMatrix CSceneComponent::GetWorldTransform() const
{
  // Защита от рекурсии
  if (m_IsComputingWorldTransform)
  {
    CORE_ERROR("Recursive world transform computation detected for component: {}", GetName());
    return m_TransformMatrix;
  }

  m_IsComputingWorldTransform = true;

  FMatrix result;

  try
  {
    if (m_Parent && m_Parent != this)
    {
      result = m_Parent->GetWorldTransform() * m_TransformMatrix;
    }
    else
    {
      result = m_TransformMatrix;
    }
  }
  catch (...)
  {
    CORE_ERROR("Exception in GetWorldTransform for component: {}", GetName());
    result = m_TransformMatrix;
  }

  m_IsComputingWorldTransform = false;
  return result;
}

FVector CSceneComponent::GetWorldLocation() const
{
  if (m_Parent)
  {
    FMatrix parentTransform = m_Parent->GetWorldTransform();
    return (parentTransform * FVector4(m_RelativeLocation, 1.0f)).ToVector3D();
  }
  return m_WorldLocation;
}

FVector CSceneComponent::GetForwardVector() const
{
  FVector euler = m_RotationQuat.ToEuler();
  float pitch = euler.x;
  float yaw = euler.y;

  return FVector(
      std::sin(yaw) * std::cos(pitch),
      -std::sin(pitch),
      std::cos(yaw) * std::cos(pitch)
  ).Normalized();
}

FVector CSceneComponent::GetRightVector() const
{
  FVector forward = GetForwardVector();
  FVector worldUp(0.0f, 1.0f, 0.0f);
  return forward.Cross(worldUp).Normalized();
}

FVector CSceneComponent::GetUpVector() const
{
  FVector forward = GetForwardVector();
  FVector right = GetRightVector();
  return right.Cross(forward).Normalized();
}

void CSceneComponent::AddYawInput(float Value)
{
  FQuat yawRot = FQuat::FromAxisAngle(FVector::UnitY, CEMath::DEG_TO_RAD * (Value));
  m_RotationQuat = yawRot * m_RotationQuat;
  UpdateRotationFromQuat();
  UpdateTransformMatrix();
}

void CSceneComponent::AddPitchInput(float Value)
{
  float newPitch = m_RelativeRotation.x + Value;

  if (m_UsePitchLimits)
  {
    newPitch = std::clamp(newPitch, m_MinPitch, m_MaxPitch);
    Value = newPitch - m_RelativeRotation.x;
  }

  FQuat pitchRot = FQuat::FromAxisAngle(GetRightVector(), CEMath::DEG_TO_RAD * (Value));
  m_RotationQuat = m_RotationQuat * pitchRot;
  UpdateRotationFromQuat();
  UpdateTransformMatrix();
}

void CSceneComponent::SetPitchLimits(float MinPitch, float MaxPitch)
{
  m_MinPitch = MinPitch;
  m_MaxPitch = MaxPitch;
  m_UsePitchLimits = true;
  ClampPitchRotation();
}

void CSceneComponent::AttachToComponent(CSceneComponent* Parent)
{
  if (m_Parent == Parent)
    return;

  // Проверка на циклические ссылки
  if (Parent && WouldCreateCycle(Parent))
  {
    CORE_ERROR("Cannot attach component: would create cyclic dependency");
    return;
  }

  if (m_Parent)
  {
    m_Parent->RemoveChild(this);
  }

  m_Parent = Parent;

  if (m_Parent)
  {
    m_Parent->AddChild(this);
  }

  UpdateTransformMatrix();
}

void CSceneComponent::DetachFromParent()
{
  AttachToComponent(nullptr);
}

void CSceneComponent::Move(const FVector& Delta)
{
  m_RelativeLocation += Delta;
  UpdateTransformMatrix();
}

void CSceneComponent::Rotate(const FVector& Delta)
{
  m_RelativeRotation += Delta;
  ClampPitchRotation();
  m_RotationQuat = FQuat::FromEuler(
      CEMath::DEG_TO_RAD *(m_RelativeRotation.x),
      CEMath::DEG_TO_RAD *(m_RelativeRotation.y),
      CEMath::DEG_TO_RAD *(m_RelativeRotation.z)
  );
  UpdateTransformMatrix();
}

void CSceneComponent::Rotate(const FQuat& Delta)
{
  m_RotationQuat = Delta * m_RotationQuat;
  UpdateRotationFromQuat();
  UpdateTransformMatrix();
}

void CSceneComponent::Update(float DeltaTime)
{
  UpdateTransformMatrix();
  for (size_t i = 0; i < m_Children.size(); i++)
  {
    auto child = m_Children[i];


    if (child && child != this)
    {
      child->Update(DeltaTime);
    }
  }
}

void CSceneComponent::UpdateTransformMatrix()
{
 
  if (m_Parent && m_Parent != this)
  {
    try
    {
      FMatrix parentTransform = m_Parent->GetWorldTransform();

      m_WorldLocation = (parentTransform * FVector4(m_RelativeLocation, 1.0f)).ToVector3D();
      
      FVector parentScale = m_Parent->GetScale();
      m_WorldScale = FVector(
          m_RelativeScale.x * parentScale.x,
          m_RelativeScale.y * parentScale.y,
          m_RelativeScale.z * parentScale.z
      );

      // Вычисляем мировое вращение
      FQuat parentRot = m_Parent->GetRotationQuat();
      FQuat worldRot = parentRot * m_RotationQuat;
      m_WorldRotation = worldRot.ToEuler() * CEMath::RAD_TO_DEG    ;
    }
    catch (...)
    {
      CORE_ERROR("Exception in parent transform computation for component: {}", GetName());
      // Fallback to local transforms
      m_WorldLocation = m_RelativeLocation;
      m_WorldScale = m_RelativeScale;
      m_WorldRotation = m_RelativeRotation;
    }
  }
  else
  {
    m_WorldLocation = m_RelativeLocation;
    m_WorldScale = m_RelativeScale;
    m_WorldRotation = m_RelativeRotation;
  }

  // Создаем матрицу трансформации: Translation * Rotation * Scale
  FMatrix translation = FMatrix::Translate(m_RelativeLocation);
  FMatrix rotation = m_RotationQuat.ToMatrix() ;
  FMatrix scale = FMatrix::Scale(m_RelativeScale);

  m_TransformMatrix = translation * rotation * scale;
}

void CSceneComponent::UpdateRotationFromQuat()
{
  m_RelativeRotation = m_RotationQuat.ToEuler() * CEMath::RAD_TO_DEG;
  ClampPitchRotation();
}

void CSceneComponent::UpdateQuatFromRotation()
{
  m_RotationQuat = FQuat::FromEuler(
      CEMath::DEG_TO_RAD*(m_RelativeRotation.x),
      CEMath::DEG_TO_RAD*(m_RelativeRotation.y),
      CEMath::DEG_TO_RAD*(m_RelativeRotation.z)
  );
}

void CSceneComponent::AddChild(CSceneComponent* Child)
{
  if (!Child || Child == this)
    return;

  // Проверяем, что ребенок еще не добавлен
  if (std::find(m_Children.begin(), m_Children.end(), Child) != m_Children.end())
  {
    CORE_WARN("Child " , Child->GetName()," already exists in parent ", GetName());
    return;
  }

  // Проверяем, что у ребенка нет другого родителя
  if (Child->m_Parent && Child->m_Parent != this)
  {
    CORE_WARN("Child " ,Child->GetName(), "  already has parent " , Child->m_Parent->GetName()," , cannot add to ", GetName());
    return;
  }

  m_Children.push_back(Child);
}

void CSceneComponent::RemoveChild(CSceneComponent* Child)
{
  auto it = std::find(m_Children.begin(), m_Children.end(), Child);
  if (it != m_Children.end())
  {
    m_Children.erase(it);
  }
}

void CSceneComponent::ClampPitchRotation()
{
  if (m_UsePitchLimits)
  {
    m_RelativeRotation.x = std::clamp(m_RelativeRotation.x, m_MinPitch, m_MaxPitch);
  }
}

bool CSceneComponent::WouldCreateCycle(CSceneComponent* PotentialParent) const
{
  // Проверяем, не создаст ли присоединение циклическую зависимость
  CSceneComponent* current = PotentialParent;
  while (current)
  {
    if (current == this)
      return true; // Найден цикл
    current = current->GetParent();
  }
  return false;
}
