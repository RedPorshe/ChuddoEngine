#include "Engine/GamePlay/Components/SceneComponent.h"

// Включение экспериментальных функций GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

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
    if (m_Parent)
    {
      CE_CORE_WARN("Cannot set world position directly on a component with a parent. Use SetRelativePosition instead.");
      return;
    }
    m_RelativeLocation = Position;
    UpdateTransformMatrix();
  }

  void SceneComponent::SetPosition(float X, float Y, float Z)
  {
    SetPosition(glm::vec3(X, Y, Z));
  }

  void SceneComponent::SetRelativePosition(const glm::vec3& Position)
  {
    m_RelativeLocation = Position;
    UpdateTransformMatrix();
  }

  void SceneComponent::SetRelativePosition(float X, float Y, float Z)
  {
    SetRelativePosition(glm::vec3(X, Y, Z));
  }

  void SceneComponent::SetRotation(const glm::vec3& Rotation)
  {
    if (m_Parent)
    {
      CE_CORE_WARN("Cannot set world rotation directly on a component with a parent. Use SetRelativeRotation instead.");
      return;
    }
    m_RelativeRotation = Rotation;
    UpdateTransformMatrix();
  }

  void SceneComponent::SetRotation(float Pitch, float Yaw, float Roll)
  {
    SetRotation(glm::vec3(Pitch, Yaw, Roll));
  }

  void SceneComponent::SetRelativeRotation(const glm::vec3& Rotation)
  {
    m_RelativeRotation = Rotation;
    UpdateTransformMatrix();
  }

  void SceneComponent::SetRelativeRotation(float Pitch, float Yaw, float Roll)
  {
    SetRelativeRotation(glm::vec3(Pitch, Yaw, Roll));
  }

  void SceneComponent::SetScale(const glm::vec3& Scale)
  {
    if (m_Parent)
    {
      CE_CORE_WARN("Cannot set world scale directly on a component with a parent. Use SetRelativeScale instead.");
      return;
    }
    m_RelativeScale = Scale;
    UpdateTransformMatrix();
  }

  void SceneComponent::SetScale(float X, float Y, float Z)
  {
    SetScale(glm::vec3(X, Y, Z));
  }

  void SceneComponent::SetScale(float value)
  {
    SetScale(value, value, value);
  }

  void SceneComponent::SetRelativeScale(const glm::vec3& Scale)
  {
    m_RelativeScale = Scale;
    UpdateTransformMatrix();
  }

  void SceneComponent::SetRelativeScale(float X, float Y, float Z)
  {
    SetRelativeScale(glm::vec3(X, Y, Z));
  }

  void SceneComponent::SetRelativeScale(float value)
  {
    SetRelativeScale(value, value, value);
  }

  const glm::vec3& SceneComponent::GetPosition() const
  {
    return m_WorldLocation;
  }

  const glm::vec3& SceneComponent::GetRelativePosition() const
  {
    return m_RelativeLocation;
  }

  const glm::vec3& SceneComponent::GetRotation() const
  {
    return m_WorldRotation;
  }

  const glm::vec3& SceneComponent::GetRelativeRotation() const
  {
    return m_RelativeRotation;
  }

  const glm::vec3& SceneComponent::GetScale() const
  {
    return m_WorldScale;
  }

  const glm::vec3& SceneComponent::GetRelativeScale() const
  {
    return m_RelativeScale;
  }

  const glm::mat4& SceneComponent::GetTransformMatrix() const
  {
    return m_TransformMatrix;
  }

  glm::vec3 SceneComponent::GetForwardVector() const
  {
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    // Тот же порядок: Yaw (Y), Pitch (X), Roll (Z)
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_WorldRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Yaw first
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_WorldRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // Pitch second
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_WorldRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Roll last

    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    return glm::normalize(glm::vec3(rotationMatrix * glm::vec4(forward, 0.0f)));
  }

  glm::vec3 SceneComponent::GetUpVector() const
  {
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    // Тот же порядок: Yaw (Y), Pitch (X), Roll (Z)
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_WorldRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Yaw first
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_WorldRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // Pitch second
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_WorldRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Roll last

    glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f);
    return glm::normalize(glm::vec3(rotationMatrix * glm::vec4(up, 0.0f)));
  }

  glm::vec3 SceneComponent::GetRightVector() const
  {
    glm::vec3 forward = GetForwardVector();
    glm::vec3 up = GetUpVector();
    return glm::normalize(glm::cross(forward, up));
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

    UpdateTransformMatrix();
  }

  void SceneComponent::DetachFromParent()
  {
    if (m_Parent)
    {
      m_Parent->RemoveChild(this);
      m_Parent = nullptr;
    }
    UpdateTransformMatrix();
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
    return m_TransformMatrix;
  }

  glm::vec3 SceneComponent::GetWorldLocation() const
  {
    return m_WorldLocation;
  }

  void SceneComponent::UpdateTransformMatrix()
  {
    // Строим локальную матрицу трансформации из относительных координат
    glm::mat4 localTransform = glm::mat4(1.0f);
    localTransform = glm::translate(localTransform, m_RelativeLocation);

    // ИЗМЕНИТЕ ПОРЯДОК ВРАЩЕНИЙ: Yaw (Y), Pitch (X), Roll (Z)
    localTransform = glm::rotate(localTransform, glm::radians(m_RelativeRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Yaw first
    localTransform = glm::rotate(localTransform, glm::radians(m_RelativeRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // Pitch second
    localTransform = glm::rotate(localTransform, glm::radians(m_RelativeRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Roll last

    localTransform = glm::scale(localTransform, m_RelativeScale);

    // Вычисляем мировую матрицу
    if (m_Parent)
    {
      m_TransformMatrix = m_Parent->GetWorldTransform() * localTransform;
    }
    else
    {
      m_TransformMatrix = localTransform;
    }

    // Извлекаем мировые координаты из матрицы с помощью glm::decompose
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    if (glm::decompose(m_TransformMatrix, scale, rotation, translation, skew, perspective))
    {
      m_WorldLocation = translation;
      m_WorldScale = scale;

      // Конвертируем кватернион в углы Эйлера
      glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));
      m_WorldRotation = euler;
    }
    else
    {
      // Fallback если decompose не сработал
      m_WorldLocation = glm::vec3(m_TransformMatrix[3]);
      m_WorldScale = glm::vec3(1.0f);
      m_WorldRotation = glm::vec3(0.0f);
      CE_CORE_WARN("Failed to decompose matrix for component: ", GetName());
    }

    // Обновляем всех детей
    for (auto* child : m_Children)
    {
      child->UpdateTransformMatrix();
    }
  }

  void SceneComponent::Update(float DeltaTime)
  {
    CEComponent::Update(DeltaTime);
  }
}  // namespace CE