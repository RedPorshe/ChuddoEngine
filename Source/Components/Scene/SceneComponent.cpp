#include "Components/Scene/SceneComponent.h"

namespace CE
{
  SceneComponent::SceneComponent(CEObject* Owner, FString NewName)
      : CEComponent(Owner, NewName)
  {
    UpdateTransformMatrix();
    CE_CORE_DEBUG("SceneComponent created: ", NewName);
  }

  void SceneComponent::SetPosition(const glm::vec3& Position)
  {
    m_RelativeLocation = Position;
    UpdateTransformMatrix();
  }

  void SceneComponent::SetPosition(float X, float Y, float Z)
  {
    SetPosition(glm::vec3(X, Y, Z));
  }

  void SceneComponent::SetRotation(const glm::vec3& Rotation)
  {
    m_RelativeRotation = Rotation;
    UpdateTransformMatrix();
  }

  void SceneComponent::SetRotation(float Pitch, float Yaw, float Roll)
  {
    SetRotation(glm::vec3(Pitch, Yaw, Roll));
  }

  void SceneComponent::SetScale(const glm::vec3& Scale)
  {
    m_RelativeScale = Scale;
    UpdateTransformMatrix();
  }

  void SceneComponent::SetScale(float X, float Y, float Z)
  {
    SetScale(glm::vec3(X, Y, Z));
  }

  void SceneComponent::Move(const glm::vec3& Delta)
  {
    m_RelativeLocation += Delta;
    UpdateTransformMatrix();
  }

  void SceneComponent::Rotate(const glm::vec3& Delta)
  {
    m_RelativeRotation += Delta;
    UpdateTransformMatrix();
  }

  void SceneComponent::AttachToComponent(SceneComponent* Parent)
  {
    if (m_Parent)
      m_Parent->RemoveChild(this);

    m_Parent = Parent;
    if (m_Parent)
      m_Parent->AddChild(this);
  }

  void SceneComponent::DetachFromParent()
  {
    if (m_Parent)
    {
      m_Parent->RemoveChild(this);
      m_Parent = nullptr;
    }
  }

  void SceneComponent::AddChild(SceneComponent* Child)
  {
    if (Child && std::find(m_Children.begin(), m_Children.end(), Child) == m_Children.end())
    {
      m_Children.push_back(Child);
    }
  }

  void SceneComponent::RemoveChild(SceneComponent* Child)
  {
    auto it = std::find(m_Children.begin(), m_Children.end(), Child);
    if (it != m_Children.end())
    {
      m_Children.erase(it);
    }
  }

  glm::mat4 SceneComponent::GetWorldTransform() const
  {
    if (m_Parent)
      return m_Parent->GetWorldTransform() * m_TransformMatrix;
    else
      return m_TransformMatrix;
  }

  glm::vec3 SceneComponent::GetWorldLocation() const
  {
    glm::mat4 worldTransform = GetWorldTransform();
    return glm::vec3(worldTransform[3]);
  }

  void SceneComponent::UpdateTransformMatrix()
  {
    m_TransformMatrix = glm::mat4(1.0f);
    m_TransformMatrix = glm::translate(m_TransformMatrix, m_RelativeLocation);
    m_TransformMatrix = glm::rotate(m_TransformMatrix, glm::radians(m_RelativeRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_TransformMatrix = glm::rotate(m_TransformMatrix, glm::radians(m_RelativeRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_TransformMatrix = glm::rotate(m_TransformMatrix, glm::radians(m_RelativeRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    m_TransformMatrix = glm::scale(m_TransformMatrix, m_RelativeScale);

    m_WorldLocation = GetWorldLocation();
  }

  void SceneComponent::Update(float DeltaTime)
  {
    CEComponent::Update(DeltaTime);
  }
}  // namespace CE