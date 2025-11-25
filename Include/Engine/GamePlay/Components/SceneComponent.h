#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "Engine/GamePlay/Components/Base/Component.h"

namespace CE
{
  class CSceneComponent : public CEComponent
  {
   public:
    CSceneComponent(CObject* Owner = nullptr, FString NewName = "SceneComponent");
    virtual ~CSceneComponent() = default;

    // position methods
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

    // Direction vectors
    glm::vec3 GetForwardVector() const;
    glm::vec3 GetRightVector() const;
    glm::vec3 GetUpVector() const;

    // Input methods
    void AddYawInput(float Value);
    void AddPitchInput(float Value);
    void SetPitchLimits(float MinPitch, float MaxPitch);

    // Quaternion access
    const glm::quat& GetRotationQuat() const
    {
      return m_RotationQuat;
    }

    // Hierarchy
    void AttachToComponent(CSceneComponent* Parent);
    void DetachFromParent();
    CSceneComponent* GetParent() const
    {
      return m_Parent;
    }
    const std::vector<CSceneComponent*>& GetChildren() const
    {
      return m_Children;
    }

    // Movement
    void Move(const glm::vec3& Delta);
    void Rotate(const glm::vec3& Delta);

    virtual void Update(float DeltaTime) override;

   protected:
    void UpdateTransformMatrix();
    void UpdateRotationFromQuat();
    void AddChild(CSceneComponent* Child);
    void RemoveChild(CSceneComponent* Child);

    // Internal methods
    void ClampPitchRotation();

    // relative transforms
    glm::vec3 m_RelativeLocation{0.0f, 0.0f, 0.0f};
    glm::vec3 m_RelativeRotation{0.0f, 0.0f, 0.0f};
    glm::vec3 m_RelativeScale{1.0f, 1.0f, 1.0f};

    // world transforms
    glm::vec3 m_WorldRotation{0.0f, 0.0f, 0.0f};
    glm::vec3 m_WorldScale{1.0f, 1.0f, 1.0f};
    glm::vec3 m_WorldLocation{0.0f, 0.0f, 0.0f};

    // matrix
    glm::mat4 m_TransformMatrix{1.0f};

    // hierarchy
    CSceneComponent* m_Parent = nullptr;
    std::vector<CSceneComponent*> m_Children;

    // quaternion for rotation
    glm::quat m_RotationQuat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    // pitch rotation limits
    float m_MinPitch = -89.0f;
    float m_MaxPitch = 89.0f;
    bool m_UsePitchLimits = false;
    friend class SpringArmComponent;
  };
}  // namespace CE