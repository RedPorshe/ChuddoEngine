#include "Engine/GamePlay/Components/SceneComponent.h"
#include "Engine/Utils/Math/MathConstants.h"

CSceneComponent::CSceneComponent(CObject* Owner, FString NewName)
    : CComponent(Owner, NewName)
{
  m_TransformMatrix.SetIdentity();
}

void CSceneComponent::SetPosition(const Math::Vector3f& Position)
{
  m_RelativeLocation = Position;
  UpdateTransformMatrix();
}

void CSceneComponent::SetPosition(float X, float Y, float Z)
{
  SetPosition(Math::Vector3f(X, Y, Z));
}

void CSceneComponent::SetRelativePosition(const Math::Vector3f& Position)
{
  m_RelativeLocation = Position;
  UpdateTransformMatrix();
}

void CSceneComponent::SetRelativePosition(float X, float Y, float Z)
{
  SetRelativePosition(Math::Vector3f(X, Y, Z));
}

void CSceneComponent::SetRotation(const Math::Vector3f& Rotation)
{
  m_WorldRotation = Rotation;
  m_RotationQuat = Math::Quaternionf::FromEulerAngles(
      Math::ToRadians(Rotation.x),
      Math::ToRadians(Rotation.y),
      Math::ToRadians(Rotation.z)
  );
  UpdateTransformMatrix();
}

void CSceneComponent::SetRotation(float Pitch, float Yaw, float Roll)
{
  SetRotation(Math::Vector3f(Pitch, Yaw, Roll));
}

void CSceneComponent::SetRelativeRotation(const Math::Vector3f& Rotation)
{
  m_RelativeRotation = Rotation;
  m_RotationQuat = Math::Quaternionf::FromEulerAngles(
      Math::ToRadians(Rotation.x),
      Math::ToRadians(Rotation.y),
      Math::ToRadians(Rotation.z)
  );
  UpdateTransformMatrix();
}

void CSceneComponent::SetRelativeRotation(float Pitch, float Yaw, float Roll)
{
  SetRelativeRotation(Math::Vector3f(Pitch, Yaw, Roll));
}

void CSceneComponent::SetRotation(const Math::Quaternionf& Rotation)
{
  m_RotationQuat = Rotation;
  UpdateRotationFromQuat();
  UpdateTransformMatrix();
}

void CSceneComponent::SetRelativeRotation(const Math::Quaternionf& Rotation)
{
  m_RotationQuat = Rotation;
  UpdateRotationFromQuat();
  UpdateTransformMatrix();
}

void CSceneComponent::SetScale(const Math::Vector3f& Scale)
{
  m_WorldScale = Scale;
  UpdateTransformMatrix();
}

void CSceneComponent::SetScale(float X, float Y, float Z)
{
  SetScale(Math::Vector3f(X, Y, Z));
}

void CSceneComponent::SetScale(float value)
{
  SetScale(Math::Vector3f(value, value, value));
}

void CSceneComponent::SetRelativeScale(const Math::Vector3f& Scale)
{
  m_RelativeScale = Scale;
  UpdateTransformMatrix();
}

void CSceneComponent::SetRelativeScale(float X, float Y, float Z)
{
  SetRelativeScale(Math::Vector3f(X, Y, Z));
}

void CSceneComponent::SetRelativeScale(float value)
{
  SetRelativeScale(Math::Vector3f(value, value, value));
}

const Math::Vector3f& CSceneComponent::GetPosition() const
{
  return m_WorldLocation;
}

const Math::Vector3f& CSceneComponent::GetRelativePosition() const
{
  return m_RelativeLocation;
}

const Math::Vector3f& CSceneComponent::GetRotation() const
{
  return m_WorldRotation;
}

const Math::Vector3f& CSceneComponent::GetRelativeRotation() const
{
  return m_RelativeRotation;
}

const Math::Vector3f& CSceneComponent::GetScale() const
{
  return m_WorldScale;
}

const Math::Vector3f& CSceneComponent::GetRelativeScale() const
{
  return m_RelativeScale;
}

const Math::Matrix4f& CSceneComponent::GetTransformMatrix() const
{
  return m_TransformMatrix;
}

Math::Matrix4f CSceneComponent::GetWorldTransform() const
{
  // Защита от рекурсии
  if (m_IsComputingWorldTransform)
  {
    CE_CORE_ERROR("Recursive world transform computation detected for component: {}", GetName());
    return m_TransformMatrix;
  }

  m_IsComputingWorldTransform = true;

  Math::Matrix4f result;

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
    CE_CORE_ERROR("Exception in GetWorldTransform for component: {}", GetName());
    result = m_TransformMatrix;
  }

  m_IsComputingWorldTransform = false;
  return result;
}

Math::Vector3f CSceneComponent::GetWorldLocation() const
{
  if (m_Parent)
  {
    Math::Matrix4f parentTransform = m_Parent->GetWorldTransform();
    return (parentTransform * Math::Vector4f(m_RelativeLocation, 1.0f)).XYZ();
  }
  return m_WorldLocation;
}

Math::Vector3f CSceneComponent::GetForwardVector() const
{
  Math::Vector3f euler = m_RotationQuat.ToEulerAngles();
  float pitch = euler.x;
  float yaw = euler.y;

  return Math::Vector3f(
      std::sin(yaw) * std::cos(pitch),
      -std::sin(pitch),
      std::cos(yaw) * std::cos(pitch)
  ).Normalized();
}

Math::Vector3f CSceneComponent::GetRightVector() const
{
  Math::Vector3f forward = GetForwardVector();
  Math::Vector3f worldUp(0.0f, 1.0f, 0.0f);
  return forward.Cross(worldUp).Normalized();
}

Math::Vector3f CSceneComponent::GetUpVector() const
{
  Math::Vector3f forward = GetForwardVector();
  Math::Vector3f right = GetRightVector();
  return right.Cross(forward).Normalized();
}

void CSceneComponent::AddYawInput(float Value)
{
  Math::Quaternionf yawRot = Math::Quaternionf::FromAxisAngle(Math::Vector3f::Up(), Math::ToRadians(Value));
  m_RotationQuat = yawRot * m_RotationQuat;
  UpdateRotationFromQuat();
  UpdateTransformMatrix();
}

void CSceneComponent::AddPitchInput(float Value)
{
  float newPitch = m_RelativeRotation.x + Value;

  if (m_UsePitchLimits)
  {
    newPitch = Math::Clamp(newPitch, m_MinPitch, m_MaxPitch);
    Value = newPitch - m_RelativeRotation.x;
  }

  Math::Quaternionf pitchRot = Math::Quaternionf::FromAxisAngle(GetRightVector(), Math::ToRadians(Value));
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
    CE_CORE_ERROR("Cannot attach component: would create cyclic dependency");
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

void CSceneComponent::Move(const Math::Vector3f& Delta)
{
  m_RelativeLocation += Delta;
  UpdateTransformMatrix();
}

void CSceneComponent::Rotate(const Math::Vector3f& Delta)
{
  m_RelativeRotation += Delta;
  ClampPitchRotation();
  m_RotationQuat = Math::Quaternionf::FromEulerAngles(
      Math::ToRadians(m_RelativeRotation.x),
      Math::ToRadians(m_RelativeRotation.y),
      Math::ToRadians(m_RelativeRotation.z)
  );
  UpdateTransformMatrix();
}

void CSceneComponent::Rotate(const Math::Quaternionf& Delta)
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
  // Вычисляем мировые трансформации
  if (m_Parent && m_Parent != this)
  {
    try
    {
      Math::Matrix4f parentTransform = m_Parent->GetWorldTransform();

      // Вычисляем мировую позицию
      m_WorldLocation = (parentTransform * Math::Vector4f(m_RelativeLocation, 1.0f)).XYZ();

      // Вычисляем мировой масштаб (приблизительно)
      Math::Vector3f parentScale = m_Parent->GetScale();
      m_WorldScale = Math::Vector3f(
          m_RelativeScale.x * parentScale.x,
          m_RelativeScale.y * parentScale.y,
          m_RelativeScale.z * parentScale.z
      );

      // Вычисляем мировое вращение
      Math::Quaternionf parentRot = m_Parent->GetRotationQuat();
      Math::Quaternionf worldRot = parentRot * m_RotationQuat;
      m_WorldRotation = worldRot.ToEulerAngles() * Math::RAD_TO_DEG;
    }
    catch (...)
    {
      CE_CORE_ERROR("Exception in parent transform computation for component: {}", GetName());
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

  // Создаем матрицу трансформации: Scale * Rotation * Translation
  Math::Matrix4f translation = Math::Matrix4f::Translation(m_RelativeLocation);
  Math::Matrix4f rotation = m_RotationQuat.ToMatrix();
  Math::Matrix4f scale = Math::Matrix4f::Scale(m_RelativeScale);

  m_TransformMatrix = translation * rotation * scale;
}

void CSceneComponent::UpdateRotationFromQuat()
{
  m_RelativeRotation = m_RotationQuat.ToEulerAngles() * Math::RAD_TO_DEG;
  ClampPitchRotation();
}

void CSceneComponent::UpdateQuatFromRotation()
{
  m_RotationQuat = Math::Quaternionf::FromEulerAngles(
      Math::ToRadians(m_RelativeRotation.x),
      Math::ToRadians(m_RelativeRotation.y),
      Math::ToRadians(m_RelativeRotation.z)
  );
}

void CSceneComponent::AddChild(CSceneComponent* Child)
{
  if (!Child || Child == this)
    return;

  // Проверяем, что ребенок еще не добавлен
  if (std::find(m_Children.begin(), m_Children.end(), Child) != m_Children.end())
  {
    CE_CORE_WARN("Child " , Child->GetName()," already exists in parent ", GetName());
    return;
  }

  // Проверяем, что у ребенка нет другого родителя
  if (Child->m_Parent && Child->m_Parent != this)
  {
    CE_CORE_WARN("Child " ,Child->GetName(), "  already has parent " , Child->m_Parent->GetName()," , cannot add to ", GetName());
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
    m_RelativeRotation.x = Math::Clamp(m_RelativeRotation.x, m_MinPitch, m_MaxPitch);
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
