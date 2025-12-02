#pragma once
#include <vector>

#include "Engine/Core/CoreTypes.h"
#include "Engine/GamePlay/Components/Base/Component.h"


  class CSceneComponent : public CComponent
  {
   public:
    CSceneComponent(CObject* Owner = nullptr, FString NewName = "SceneComponent");
    virtual ~CSceneComponent() = default;

    // position methods
    void SetPosition(const FVector& Position);
    void SetPosition(float X, float Y, float Z);
    void SetRelativePosition(const FVector& Position);
    void SetRelativePosition(float X, float Y, float Z);
     void SetRelativePosition(float val);

    // rotation methods
    void SetRotation(const FVector& Rotation);
    void SetRotation(float Pitch, float Yaw, float Roll);
    void SetRelativeRotation(const FVector& Rotation);
    void SetRelativeRotation(float Pitch, float Yaw, float Roll);
    void SetRotation(const FQuat& Rotation);
    void SetRelativeRotation(const FQuat& Rotation);

    // scale methods
    void SetScale(const FVector& Scale);
    void SetScale(float X, float Y, float Z);
    void SetScale(float value);
    void SetRelativeScale(const FVector& Scale);
    void SetRelativeScale(float X, float Y, float Z);
    void SetRelativeScale(float value);

    // getters
    const FVector& GetPosition() const;
    const FVector& GetRelativePosition() const;
    const FVector& GetRotation() const;
    const FVector& GetRelativeRotation() const;
    const FVector& GetScale() const;
    const FVector& GetRelativeScale() const;
    const FMatrix& GetTransformMatrix() const;

    FMatrix GetWorldTransform() const;
    FVector GetWorldLocation() const;

    // Direction vectors
    FVector GetForwardVector() const;
    FVector GetRightVector() const;
    FVector GetUpVector() const;

    // Input methods
    void AddYawInput(float Value);
    void AddPitchInput(float Value);
    void SetPitchLimits(float MinPitch, float MaxPitch);

    // Quaternion access
    const FQuat& GetRotationQuat() const
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
    void Move(const FVector& Delta);
    void Rotate(const FVector& Delta);
    void Rotate(const FQuat& Delta);

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
    FVector m_RelativeLocation{0.0f, 0.0f, 0.0f};
    FVector m_RelativeRotation{0.0f, 0.0f, 0.0f};
    FVector m_RelativeScale{1.0f, 1.0f, 1.0f};

    // world transforms
    FVector m_WorldRotation{0.0f, 0.0f, 0.0f};
    FVector m_WorldScale{1.0f, 1.0f, 1.0f};
    FVector m_WorldLocation{0.0f, 0.0f, 0.0f};

    // matrix
    FMatrix m_TransformMatrix;

    // hierarchy
    CSceneComponent* m_Parent = nullptr;
    std::vector<CSceneComponent*> m_Children;

    // quaternion for rotation
    FQuat m_RotationQuat = FQuat::Identity();

      bool m_IsUpdating = false;
    mutable bool m_IsComputingWorldTransform = false;
    uint32_t m_UpdateDepth = 0;
    // pitch rotation limits
    float m_MinPitch = -89.0f;
    float m_MaxPitch = 89.0f;
    bool m_UsePitchLimits = false;
    friend class CSpringArmComponent;
  };