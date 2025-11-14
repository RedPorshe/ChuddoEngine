#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Engine/GamePlay/Components/Base/Component.h"

namespace CE
{
  class SceneComponent : public CEComponent
  {
   public:
    SceneComponent(CEObject* Owner = nullptr, FString NewName = "SceneComponent");
    virtual ~SceneComponent() = default;

    // positon methods
    void SetPosition(const glm::vec3& Position);
    void SetPosition(float X, float Y, float Z);
    void SetRelativePosition(const glm::vec3& Position);
    void SetRelativePosition(float X, float Y, float Z);

    // rotation methods
    void SetRotation(const glm::vec3& Rotation);
    void SetRotation(float Pitch, float Yaw, float Roll);
    void SetRelativeRotation(const glm::vec3& Rotation);
    void SetRelativeRotation(float Pitch, float Yaw, float Roll);
    // scale methods
    void SetScale(const glm::vec3& Scale);
    void SetScale(float X, float Y, float Z);
    void SetScale(float value);
    void SetRelativeScale(const glm::vec3& Scale);
    void SetRelativeScale(float X, float Y, float Z);
    void SetRelativeScale(float value);
    // getters
    const glm::vec3& GetPosition() const;
    const glm::vec3& GetRelativePosition() const;
    const glm::vec3& GetRotation() const;
    const glm::vec3& GetRelativeRotation() const;
    const glm::vec3& GetScale() const;
    const glm::vec3& GetRelativeScale() const;
    const glm::mat4& GetTransformMatrix() const;

    glm::mat4 GetWorldTransform() const;
    glm::vec3 GetWorldLocation() const;

    glm::vec3 GetForwardVector() const;
    glm::vec3 GetRightVector() const;
    glm::vec3 GetUpVector() const;

    // Hierarchy
    void AttachToComponent(SceneComponent* Parent);
    void DetachFromParent();
    SceneComponent* GetParent() const
    {
      return m_Parent;
    }
    const std::vector<SceneComponent*>& GetChildren() const
    {
      return m_Children;
    }

    // Movement
    void Move(const glm::vec3& Delta);
    void Rotate(const glm::vec3& Delta);

    virtual void Update(float DeltaTime) override;

   protected:
    void UpdateTransformMatrix();
    void AddChild(SceneComponent* Child);
    void RemoveChild(SceneComponent* Child);
    // relative transforms
    glm::vec3 m_RelativeLocation{0.0f, 0.0f, 0.0f};
    glm::vec3 m_RelativeRotation{0.0f, 0.0f, 0.0f};
    glm::vec3 m_RelativeScale{1.0f, 1.0f, 1.0f};
    // world transforms
    glm::vec3 m_WorldRotation{0.0f, 0.0f, 0.0f};
    glm::vec3 m_WorldScale{1.0f, 1.0f, 1.0f};

    glm::vec3 m_WorldLocation{0.0f, 0.0f, 0.0f};
    glm::mat4 m_TransformMatrix{1.0f};
    SceneComponent* m_Parent = nullptr;
    std::vector<SceneComponent*> m_Children;
  };
}  // namespace CE