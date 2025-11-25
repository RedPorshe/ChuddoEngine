#include "Engine/GamePlay/Components/SceneComponent.h"

// Включение экспериментальных функций GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace CE
{
  CSceneComponent::CSceneComponent(CObject* Owner, FString NewName)
      : CEComponent(Owner, NewName)
  {
    UpdateTransformMatrix();
    CE_CORE_DEBUG("SceneComponent created: ", NewName);
  }

  void CSceneComponent::SetPitchLimits(float MinPitch, float MaxPitch)
  {
    m_MinPitch = MinPitch;
    m_MaxPitch = MaxPitch;
    m_UsePitchLimits = true;

    // Применяем ограничения к текущему вращению
    ClampPitchRotation();
  }

  void CSceneComponent::SetPosition(const glm::vec3& Position)
  {
    if (m_Parent)
    {
      CE_CORE_WARN("Cannot set world position directly on a component with a parent. Use SetRelativePosition instead.");
      return;
    }
    m_RelativeLocation = Position;
    UpdateTransformMatrix();
  }

  void CSceneComponent::SetPosition(float X, float Y, float Z)
  {
    SetPosition(glm::vec3(X, Y, Z));
  }

  void CSceneComponent::SetRelativePosition(const glm::vec3& Position)
  {
    m_RelativeLocation = Position;
    UpdateTransformMatrix();
  }

  void CSceneComponent::SetRelativePosition(float X, float Y, float Z)
  {
    SetRelativePosition(glm::vec3(X, Y, Z));
  }

  void CSceneComponent::SetRotation(const glm::vec3& Rotation)
  {
    if (m_Parent)
    {
      CE_CORE_WARN("Cannot set world rotation directly on a component with a parent. Use SetRelativeRotation instead.");
      return;
    }
    m_RelativeRotation = Rotation;

    // Update quaternion from euler angles
    m_RotationQuat = glm::quat(glm::radians(Rotation));

    UpdateTransformMatrix();
  }

  void CSceneComponent::SetRotation(float Pitch, float Yaw, float Roll)
  {
    SetRotation(glm::vec3(Pitch, Yaw, Roll));
  }

  void CSceneComponent::SetRelativeRotation(const glm::vec3& Rotation)
  {
    m_RelativeRotation = Rotation;

    // Update quaternion from euler angles
    m_RotationQuat = glm::quat(glm::radians(Rotation));

    UpdateTransformMatrix();
  }

  void CSceneComponent::SetRelativeRotation(float Pitch, float Yaw, float Roll)
  {
    SetRelativeRotation(glm::vec3(Pitch, Yaw, Roll));
  }

  void CSceneComponent::SetScale(const glm::vec3& Scale)
  {
    if (m_Parent)
    {
      CE_CORE_WARN("Cannot set world scale directly on a component with a parent. Use SetRelativeScale instead.");
      return;
    }
    m_RelativeScale = Scale;
    UpdateTransformMatrix();
  }

  void CSceneComponent::SetScale(float X, float Y, float Z)
  {
    SetScale(glm::vec3(X, Y, Z));
  }

  void CSceneComponent::SetScale(float value)
  {
    SetScale(value, value, value);
  }

  void CSceneComponent::SetRelativeScale(const glm::vec3& Scale)
  {
    m_RelativeScale = Scale;
    UpdateTransformMatrix();
  }

  void CSceneComponent::SetRelativeScale(float X, float Y, float Z)
  {
    SetRelativeScale(glm::vec3(X, Y, Z));
  }

  void CSceneComponent::SetRelativeScale(float value)
  {
    SetRelativeScale(value, value, value);
  }

  const glm::vec3& CSceneComponent::GetPosition() const
  {
    return m_WorldLocation;
  }

  const glm::vec3& CSceneComponent::GetRelativePosition() const
  {
    return m_RelativeLocation;
  }

  const glm::vec3& CSceneComponent::GetRotation() const
  {
    return m_WorldRotation;
  }

  const glm::vec3& CSceneComponent::GetRelativeRotation() const
  {
    return m_RelativeRotation;
  }

  const glm::vec3& CSceneComponent::GetScale() const
  {
    return m_WorldScale;
  }

  const glm::vec3& CSceneComponent::GetRelativeScale() const
  {
    return m_RelativeScale;
  }

  const glm::mat4& CSceneComponent::GetTransformMatrix() const
  {
    return m_TransformMatrix;
  }

  glm::vec3 CSceneComponent::GetForwardVector() const
  {
    return glm::normalize(m_RotationQuat * glm::vec3(0.0f, 0.0f, -1.0f));
  }

  glm::vec3 CSceneComponent::GetUpVector() const
  {
    return glm::normalize(m_RotationQuat * glm::vec3(0.0f, 1.0f, 0.0f));
  }

  glm::vec3 CSceneComponent::GetRightVector() const
  {
    return glm::normalize(m_RotationQuat * glm::vec3(1.0f, 0.0f, 0.0f));
  }

  void CSceneComponent::AddYawInput(float Value)
  {
    // Rotate around global Y axis (horizontal) - CORRECTED
    glm::quat yawRot = glm::angleAxis(glm::radians(Value), glm::vec3(0.0f, 1.0f, 0.0f));
    m_RotationQuat = yawRot * m_RotationQuat;

    UpdateRotationFromQuat();
  }

  void CSceneComponent::AddPitchInput(float Value)
  {
    // Rotate around LOCAL X axis (vertical) - use current right vector - CORRECTED
    glm::vec3 localRight = GetRightVector();
    glm::quat pitchRot = glm::angleAxis(glm::radians(Value), localRight);
    m_RotationQuat = m_RotationQuat * pitchRot;  // Local rotation

    UpdateRotationFromQuat();

    // Apply pitch limits after rotation
    if (m_UsePitchLimits)
    {
      ClampPitchRotation();
    }
  }

  void CSceneComponent::ClampPitchRotation()
  {
    // Получаем текущее относительное вращение
    glm::vec3 currentRotation = GetRelativeRotation();

    // Ограничиваем угол pitch (X-ось)
    float clampedPitch = glm::clamp(currentRotation.x, m_MinPitch, m_MaxPitch);

    // Если угол выходит за пределы, устанавливаем ограниченное значение
    if (currentRotation.x != clampedPitch)
    {
      SetRelativeRotation(clampedPitch, currentRotation.y, currentRotation.z);
    }
  }

  void CSceneComponent::UpdateRotationFromQuat()
  {
    // Convert quaternion back to euler angles for compatibility
    glm::vec3 euler = glm::degrees(glm::eulerAngles(m_RotationQuat));

    // Update relative rotation
    m_RelativeRotation = euler;

    // Update transform
    UpdateTransformMatrix();
  }

  void CSceneComponent::Move(const glm::vec3& Delta)
  {
    m_RelativeLocation += Delta;
    UpdateTransformMatrix();
  }

  void CSceneComponent::Rotate(const glm::vec3& Delta)
  {
    m_RelativeRotation += Delta;
    m_RotationQuat = glm::quat(glm::radians(m_RelativeRotation));
    UpdateTransformMatrix();
  }

  void CSceneComponent::AttachToComponent(CSceneComponent* Parent)
  {
    if (m_Parent)
      m_Parent->RemoveChild(this);

    m_Parent = Parent;
    if (m_Parent)
      m_Parent->AddChild(this);

    UpdateTransformMatrix();
  }

  void CSceneComponent::DetachFromParent()
  {
    if (m_Parent)
    {
      m_Parent->RemoveChild(this);
      m_Parent = nullptr;
    }
    UpdateTransformMatrix();
  }

  void CSceneComponent::AddChild(CSceneComponent* Child)
  {
    if (Child && std::find(m_Children.begin(), m_Children.end(), Child) == m_Children.end())
    {
      m_Children.push_back(Child);
    }
  }

  void CSceneComponent::RemoveChild(CSceneComponent* Child)
  {
    auto it = std::find(m_Children.begin(), m_Children.end(), Child);
    if (it != m_Children.end())
    {
      m_Children.erase(it);
    }
  }

  glm::mat4 CSceneComponent::GetWorldTransform() const
  {
    return m_TransformMatrix;
  }

  glm::vec3 CSceneComponent::GetWorldLocation() const
  {
    return m_WorldLocation;
  }

  void CSceneComponent::UpdateTransformMatrix()
  {
    // Build local transform matrix using quaternion for rotation
    glm::mat4 localTransform = glm::mat4(1.0f);
    localTransform = glm::translate(localTransform, m_RelativeLocation);

    // Use quaternion for rotation (avoids gimbal lock)
    // CORRECT: Apply rotation matrix from quaternion
    glm::mat4 rotationMatrix = glm::mat4_cast(m_RotationQuat);
    localTransform = localTransform * rotationMatrix;

    localTransform = glm::scale(localTransform, m_RelativeScale);

    // Calculate world matrix
    if (m_Parent)
    {
      m_TransformMatrix = m_Parent->GetWorldTransform() * localTransform;
    }
    else
    {
      m_TransformMatrix = localTransform;
    }

    // Extract world coordinates from matrix
    // SIMPLIFIED: Just extract position from matrix directly
    m_WorldLocation = glm::vec3(m_TransformMatrix[3]);

    // For rotation and scale, use the relative values (simplified)
    // In a full implementation, you'd decompose the matrix
    m_WorldRotation = m_RelativeRotation;
    m_WorldScale = m_RelativeScale;

    // Update all children
    for (auto* child : m_Children)
    {
      child->UpdateTransformMatrix();
    }
  }

  void CSceneComponent::Update(float DeltaTime)
  {
    CEComponent::Update(DeltaTime);
  }
}  // namespace CE