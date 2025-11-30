#pragma once
#include <vector>

#include "Engine/Utils/Math/Vector3.h"
#include "Engine/Utils/Math/Quaternion.h"
#include "Engine/Utils/Math/Matrix4.h"
#include "Engine/GamePlay/Components/Base/Component.h"


  class CSceneComponent : public CComponent
  {
   public:
    CSceneComponent(CObject* Owner = nullptr, FString NewName = "SceneComponent");
    virtual ~CSceneComponent() = default;

    // position methods
    void SetPosition(const CEMath::Vector3f& Position);
    void SetPosition(float X, float Y, float Z);
    void SetRelativePosition(const CEMath::Vector3f& Position);
    void SetRelativePosition(float X, float Y, float Z);

    // rotation methods
    void SetRotation(const CEMath::Vector3f& Rotation);
    void SetRotation(float Pitch, float Yaw, float Roll);
    void SetRelativeRotation(const CEMath::Vector3f& Rotation);
    void SetRelativeRotation(float Pitch, float Yaw, float Roll);
    void SetRotation(const CEMath::Quaternionf& Rotation);
    void SetRelativeRotation(const CEMath::Quaternionf& Rotation);

    // scale methods
    void SetScale(const CEMath::Vector3f& Scale);
    void SetScale(float X, float Y, float Z);
    void SetScale(float value);
    void SetRelativeScale(const CEMath::Vector3f& Scale);
    void SetRelativeScale(float X, float Y, float Z);
    void SetRelativeScale(float value);

    // getters
    const CEMath::Vector3f& GetPosition() const;
    const CEMath::Vector3f& GetRelativePosition() const;
    const CEMath::Vector3f& GetRotation() const;
    const CEMath::Vector3f& GetRelativeRotation() const;
    const CEMath::Vector3f& GetScale() const;
    const CEMath::Vector3f& GetRelativeScale() const;
    const CEMath::Matrix4f& GetTransformMatrix() const;

    CEMath::Matrix4f GetWorldTransform() const;
    CEMath::Vector3f GetWorldLocation() const;

    // Direction vectors
    CEMath::Vector3f GetForwardVector() const;
    CEMath::Vector3f GetRightVector() const;
    CEMath::Vector3f GetUpVector() const;

    // Input methods
    void AddYawInput(float Value);
    void AddPitchInput(float Value);
    void SetPitchLimits(float MinPitch, float MaxPitch);

    // Quaternion access
    const CEMath::Quaternionf& GetRotationQuat() const
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
    void Move(const CEMath::Vector3f& Delta);
    void Rotate(const CEMath::Vector3f& Delta);
    void Rotate(const CEMath::Quaternionf& Delta);

    virtual void Update(float DeltaTime) override;

   protected:
    void UpdateTransformMatrix();
    void UpdateRotationFromQuat();
    void UpdateQuatFromRotation();
    void AddChild(CSceneComponent* Child);
    void RemoveChild(CSceneComponent* Child);

    // Internal methods
    void ClampPitchRotation();
    bool WouldCreateCycle(CSceneComponent* PotentialParent) const;

    // relative transforms
    CEMath::Vector3f m_RelativeLocation{0.0f, 0.0f, 0.0f};
    CEMath::Vector3f m_RelativeRotation{0.0f, 0.0f, 0.0f};
    CEMath::Vector3f m_RelativeScale{1.0f, 1.0f, 1.0f};

    // world transforms
    CEMath::Vector3f m_WorldRotation{0.0f, 0.0f, 0.0f};
    CEMath::Vector3f m_WorldScale{1.0f, 1.0f, 1.0f};
    CEMath::Vector3f m_WorldLocation{0.0f, 0.0f, 0.0f};

    // matrix
    CEMath::Matrix4f m_TransformMatrix;

    // hierarchy
    CSceneComponent* m_Parent = nullptr;
    std::vector<CSceneComponent*> m_Children;

    // quaternion for rotation
    CEMath::Quaternionf m_RotationQuat = CEMath::Quaternionf::Identity();

      bool m_IsUpdating = false;
    mutable bool m_IsComputingWorldTransform = false;
    uint32_t m_UpdateDepth = 0;
    // pitch rotation limits
    float m_MinPitch = -89.0f;
    float m_MaxPitch = 89.0f;
    bool m_UsePitchLimits = false;
    friend class CSpringArmComponent;
  };